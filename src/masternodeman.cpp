// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2020 The BCZ developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "masternodeman.h"

#include "addrman.h"
#include "evo/deterministicmns.h"
#include "fs.h"
#include "masternode-payments.h"
#include "masternode-sync.h"
#include "masternode.h"
#include "messagesigner.h"
#include "netbase.h"
#include "netmessagemaker.h"
#include "shutdown.h"
#include "spork.h"
#include "tiertwo/tiertwo_sync_state.h"
#include "validation.h"

#include <boost/thread/thread.hpp>

#define MN_WINNER_MINIMUM_AGE 8000    // Age in seconds. This should be > MASTERNODE_REMOVAL_SECONDS to avoid misconfigured new nodes in the list.

/** Masternode manager */
CMasternodeMan mnodeman;
/** Keep track of the active Masternode */
CActiveMasternode activeMasternode;

struct CompareScoreMN {
    template <typename T>
    bool operator()(const std::pair<int64_t, T>& t1,
        const std::pair<int64_t, T>& t2) const
    {
        return t1.first < t2.first;
    }
};

//
// CMasternodeDB
//

static const int MASTERNODE_DB_VERSION_BIP155 = 2;

CMasternodeDB::CMasternodeDB()
{
    pathMN = GetDataDir() / "mncache.dat";
    strMagicMessage = "MasternodeCache";
}

bool CMasternodeDB::Write(const CMasternodeMan& mnodemanToSave)
{
    int64_t nStart = GetTimeMillis();
    const auto& params = Params();

    // serialize, checksum data up to that point, then append checksum
    // Always done in the latest format.
    CDataStream ssMasternodes(SER_DISK, CLIENT_VERSION | ADDRV2_FORMAT);
    ssMasternodes << MASTERNODE_DB_VERSION_BIP155;
    ssMasternodes << strMagicMessage;                   // masternode cache file specific magic message
    ssMasternodes << params.MessageStart(); // network specific magic number
    ssMasternodes << mnodemanToSave;
    uint256 hash = Hash(ssMasternodes.begin(), ssMasternodes.end());
    ssMasternodes << hash;

    // open output file, and associate with CAutoFile
    FILE* file = fsbridge::fopen(pathMN, "wb");
    CAutoFile fileout(file, SER_DISK, CLIENT_VERSION);
    if (fileout.IsNull())
        return error("%s : Failed to open file %s", __func__, pathMN.string());

    // Write and commit header, data
    try {
        fileout << ssMasternodes;
    } catch (const std::exception& e) {
        return error("%s : Serialize or I/O error - %s", __func__, e.what());
    }
    //    FileCommit(fileout);
    fileout.fclose();

    LogPrint(BCLog::MASTERNODE,"Written info to mncache.dat  %dms\n", GetTimeMillis() - nStart);
    LogPrint(BCLog::MASTERNODE,"  %s\n", mnodemanToSave.ToString());

    return true;
}

CMasternodeDB::ReadResult CMasternodeDB::Read(CMasternodeMan& mnodemanToLoad)
{
    int64_t nStart = GetTimeMillis();
    // open input file, and associate with CAutoFile
    FILE* file = fsbridge::fopen(pathMN, "rb");
    CAutoFile filein(file, SER_DISK, CLIENT_VERSION);
    if (filein.IsNull()) {
        error("%s : Failed to open file %s", __func__, pathMN.string());
        return FileError;
    }

    // use file size to size memory buffer
    int fileSize = fs::file_size(pathMN);
    int dataSize = fileSize - sizeof(uint256);
    // Don't try to resize to a negative number if file is small
    if (dataSize < 0)
        dataSize = 0;
    std::vector<unsigned char> vchData;
    vchData.resize(dataSize);
    uint256 hashIn;

    // read data and checksum from file
    try {
        filein.read((char*)vchData.data(), dataSize);
        filein >> hashIn;
    } catch (const std::exception& e) {
        error("%s : Deserialize or I/O error - %s", __func__, e.what());
        return HashReadError;
    }
    filein.fclose();

    const auto& params = Params();
    // serialize, checksum data up to that point, then append checksum
    CDataStream ssMasternodes(vchData, SER_DISK,  CLIENT_VERSION);

    // verify stored checksum matches input data
    uint256 hashTmp = Hash(ssMasternodes.begin(), ssMasternodes.end());
    if (hashIn != hashTmp) {
        error("%s : Checksum mismatch, data corrupted", __func__);
        return IncorrectHash;
    }

    int version;
    std::string strMagicMessageTmp;
    try {
        // de-serialize file header
        ssMasternodes >> version;
        ssMasternodes >> strMagicMessageTmp;

        // ... verify the message matches predefined one
        if (strMagicMessage != strMagicMessageTmp) {
            error("%s : Invalid masternode cache magic message", __func__);
            return IncorrectMagicMessage;
        }

        // de-serialize file header (network specific magic number) and ..
        std::vector<unsigned char> pchMsgTmp(4);
        ssMasternodes >> MakeSpan(pchMsgTmp);

        // ... verify the network matches ours
        if (memcmp(pchMsgTmp.data(), params.MessageStart(), pchMsgTmp.size()) != 0) {
            error("%s : Invalid network magic number", __func__);
            return IncorrectMagicNumber;
        }
        // de-serialize data into CMasternodeMan object.
        if (version == MASTERNODE_DB_VERSION_BIP155) {
            OverrideStream<CDataStream> s(&ssMasternodes, ssMasternodes.GetType(), ssMasternodes.GetVersion() | ADDRV2_FORMAT);
            s >> mnodemanToLoad;
        } else {
            // Old format
            ssMasternodes >> mnodemanToLoad;
        }
    } catch (const std::exception& e) {
        mnodemanToLoad.Clear();
        error("%s : Deserialize or I/O error - %s", __func__, e.what());
        return IncorrectFormat;
    }

    LogPrint(BCLog::MASTERNODE,"Loaded info from mncache.dat (dbversion=%d) %dms\n", version, GetTimeMillis() - nStart);
    LogPrint(BCLog::MASTERNODE,"  %s\n", mnodemanToLoad.ToString());

    return Ok;
}

void DumpMasternodes()
{
    int64_t nStart = GetTimeMillis();

    CMasternodeDB mndb;
    LogPrint(BCLog::MASTERNODE,"Writing info to mncache.dat...\n");
    mndb.Write(mnodeman);

    LogPrint(BCLog::MASTERNODE,"Masternode dump finished  %dms\n", GetTimeMillis() - nStart);
}

CMasternodeMan::CMasternodeMan():
        cvLastBlockHashes(CACHED_BLOCK_HASHES, UINT256_ZERO),
        nDsqCount(0)
{}

bool CMasternodeMan::Add(CMasternode& mn)
{
    return false;
}

void CMasternodeMan::AskForMN(CNode* pnode, const CTxIn& vin)
{
        return;
}

int CMasternodeMan::CheckAndRemove(bool forceExpiredRemoval)
{
    Clear();
    return 0;
}

void CMasternodeMan::Clear()
{
    LOCK(cs);
    mapMasternodes.clear();
    mAskedUsForMasternodeList.clear();
    mWeAskedForMasternodeList.clear();
    mWeAskedForMasternodeListEntry.clear();
    mapSeenMasternodeBroadcast.clear();
    mapSeenMasternodePing.clear();
    nDsqCount = 0;
}

static void CountNetwork(const CService& addr, int& ipv4, int& ipv6, int& onion)
{
    std::string strHost;
    int port;
    SplitHostPort(addr.ToString(), port, strHost);
    CNetAddr node;
    LookupHost(strHost, node, false);
    switch(node.GetNetwork()) {
        case NET_IPV4:
            ipv4++;
            break;
        case NET_IPV6:
            ipv6++;
            break;
        case NET_ONION:
            onion++;
            break;
        default:
            break;
    }
}

CMasternodeMan::MNsInfo CMasternodeMan::getMNsInfo() const
{
    MNsInfo info;
    int nMinProtocol = ActiveProtocol();
    bool spork_21_active = sporkManager.IsSporkActive(SPORK_21_MASTERNODE_PAYMENT_ENFORCEMENT);

    // legacy masternodes
    {
        LOCK(cs);
        for (const auto& it : mapMasternodes) {
            const MasternodeRef& mn = it.second;
            info.total++;
            CountNetwork(mn->addr, info.ipv4, info.ipv6, info.onion);
            if (mn->protocolVersion < nMinProtocol || !mn->IsEnabled()) {
                continue;
            }
            info.enabledSize++;
            // Eligible for payments
            if (spork_21_active && (GetAdjustedTime() - mn->sigTime < MN_WINNER_MINIMUM_AGE)) {
                continue; // Skip masternodes younger than (default) 8000 sec (MUST be > MASTERNODE_REMOVAL_SECONDS)
            }
            info.stableSize++;
        }
    }

    // deterministic masternodes
        auto mnList = deterministicMNManager->GetListAtChainTip();
        mnList.ForEachMN(false, [&](const CDeterministicMNCPtr& dmn) {
            info.total++;
            CountNetwork(dmn->pdmnState->addr, info.ipv4, info.ipv6, info.onion);
            if (!dmn->IsPoSeBanned()) {
                info.enabledSize++;
                info.stableSize++;
            }
        });

    return info;
}

int CMasternodeMan::CountEnabled() const
{
    int count_enabled = 0;
    int protocolVersion = ActiveProtocol();

    {
        LOCK(cs);
        for (const auto& it : mapMasternodes) {
            const MasternodeRef& mn = it.second;
            if (mn->protocolVersion < protocolVersion || !mn->IsEnabled()) continue;
            count_enabled++;
        }
    }

    count_enabled += deterministicMNManager->GetListAtChainTip().GetValidMNsCount();

    return count_enabled;
}

bool CMasternodeMan::RequestMnList(CNode* pnode)
{
    return false;
}

CMasternode* CMasternodeMan::Find(const COutPoint& collateralOut)
{
    LOCK(cs);
    auto it = mapMasternodes.find(collateralOut);
    return it != mapMasternodes.end() ? it->second.get() : nullptr;
}

const CMasternode* CMasternodeMan::Find(const COutPoint& collateralOut) const
{
    LOCK(cs);
    auto const& it = mapMasternodes.find(collateralOut);
    return it != mapMasternodes.end() ? it->second.get() : nullptr;
}

CMasternode* CMasternodeMan::Find(const CPubKey& pubKeyMasternode)
{
    LOCK(cs);

    for (auto& it : mapMasternodes) {
        MasternodeRef& mn = it.second;
        if (mn->pubKeyMasternode == pubKeyMasternode)
            return mn.get();
    }
    return nullptr;
}

void CMasternodeMan::CheckSpentCollaterals(const std::vector<CTransactionRef>& vtx)
{
    return;
}

static bool canScheduleMN(bool fFilterSigTime, const MasternodeRef& mn, int minProtocol,
                          int nMnCount, int nBlockHeight)
{
    // check protocol version
    if (mn->protocolVersion < minProtocol) return false;

    // it's in the list (up to 8 entries ahead of current block to allow propagation) -- so let's skip it
    if (masternodePayments.IsScheduled(*mn, nBlockHeight)) return false;

    // it's too new, wait for a cycle
    if (fFilterSigTime && mn->sigTime + (nMnCount * 2.6 * 60) > GetAdjustedTime()) return false;

    // make sure it has as many confirmations as there are masternodes
    if (pcoinsTip->GetCoinDepthAtHeight(mn->vin.prevout, nBlockHeight) < nMnCount) return false;

    return true;
}

//
// Deterministically select the oldest/best masternode to pay on the network
//
MasternodeRef CMasternodeMan::GetNextMasternodeInQueueForPayment(int nBlockHeight, bool fFilterSigTime, int& nCount, const CBlockIndex* pChainTip) const
{
    return nullptr;
}

MasternodeRef CMasternodeMan::GetCurrentMasterNode(const uint256& hash) const
{
    int minProtocol = ActiveProtocol();
    int64_t score = 0;
    MasternodeRef winner = nullptr;

    // scan for winner
    for (const auto& it : mapMasternodes) {
        const MasternodeRef& mn = it.second;
        if (mn->protocolVersion < minProtocol || !mn->IsEnabled()) continue;
        // calculate the score of the masternode
        const int64_t n = mn->CalculateScore(hash).GetCompact(false);
        // determine the winner
        if (n > score) {
            score = n;
            winner = mn;
        }
    }

    // scan also dmns
        auto mnList = deterministicMNManager->GetListAtChainTip();
        mnList.ForEachMN(true, [&](const CDeterministicMNCPtr& dmn) {
            const MasternodeRef mn = MakeMasternodeRefForDMN(dmn);
            // calculate the score of the masternode
            const int64_t n = mn->CalculateScore(hash).GetCompact(false);
            // determine the winner
            if (n > score) {
                score = n;
                winner = mn;
            }
        });

    return winner;
}

std::vector<std::pair<MasternodeRef, int>> CMasternodeMan::GetMnScores(int nLast) const
{
    std::vector<std::pair<MasternodeRef, int>> ret;
    int nChainHeight = GetBestHeight();
    if (nChainHeight < 0) return ret;

    for (int nHeight = nChainHeight - nLast; nHeight < nChainHeight + 20; nHeight++) {
        const uint256& hash = GetHashAtHeight(nHeight - 101);
        MasternodeRef winner = GetCurrentMasterNode(hash);
        if (winner) {
            ret.emplace_back(winner, nHeight);
        }
    }
    return ret;
}

int CMasternodeMan::GetMasternodeRank(const CTxIn& vin, int64_t nBlockHeight) const
{
    const uint256& hash = GetHashAtHeight(nBlockHeight - 1);
    // height outside range
    if (hash == UINT256_ZERO) return -1;

    // scan for winner
    int minProtocol = ActiveProtocol();
    std::vector<std::pair<int64_t, CTxIn> > vecMasternodeScores;
    {
        LOCK(cs);
        for (const auto& it : mapMasternodes) {
            const MasternodeRef& mn = it.second;
            if (!mn->IsEnabled()) {
                continue; // Skip not enabled
            }
            if (mn->protocolVersion < minProtocol) {
                LogPrint(BCLog::MASTERNODE,"Skipping Masternode with obsolete version %d\n", mn->protocolVersion);
                continue; // Skip obsolete versions
            }
            if (sporkManager.IsSporkActive(SPORK_21_MASTERNODE_PAYMENT_ENFORCEMENT) &&
                    GetAdjustedTime() - mn->sigTime < MN_WINNER_MINIMUM_AGE) {
                continue; // Skip masternodes younger than (default) 1 hour
            }
            vecMasternodeScores.emplace_back(mn->CalculateScore(hash).GetCompact(false), mn->vin);
        }
    }

    // scan also dmns
        auto mnList = deterministicMNManager->GetListAtChainTip();
        mnList.ForEachMN(true, [&](const CDeterministicMNCPtr& dmn) {
            const MasternodeRef mn = MakeMasternodeRefForDMN(dmn);
            vecMasternodeScores.emplace_back(mn->CalculateScore(hash).GetCompact(false), mn->vin);
        });

    sort(vecMasternodeScores.rbegin(), vecMasternodeScores.rend(), CompareScoreMN());

    int rank = 0;
    for (std::pair<int64_t, CTxIn> & s : vecMasternodeScores) {
        rank++;
        if (s.second.prevout == vin.prevout) {
            return rank;
        }
    }

    return -1;
}

std::vector<std::pair<int64_t, MasternodeRef>> CMasternodeMan::GetMasternodeRanks(int nBlockHeight) const
{
    std::vector<std::pair<int64_t, MasternodeRef>> vecMasternodeScores;
    const uint256& hash = GetHashAtHeight(nBlockHeight - 1);
    // height outside range
    if (hash == UINT256_ZERO) return vecMasternodeScores;
    {
        LOCK(cs);
        // scan for winner
        for (const auto& it : mapMasternodes) {
            const MasternodeRef mn = it.second;
            const uint32_t score = mn->IsEnabled() ? mn->CalculateScore(hash).GetCompact(false) : 9999;

            vecMasternodeScores.emplace_back(score, mn);
        }
    }
    // scan also dmns
        auto mnList = deterministicMNManager->GetListAtChainTip();
        mnList.ForEachMN(false, [&](const CDeterministicMNCPtr& dmn) {
            const MasternodeRef mn = MakeMasternodeRefForDMN(dmn);
            const uint32_t score = dmn->IsPoSeBanned() ? 9999 : mn->CalculateScore(hash).GetCompact(false);

            vecMasternodeScores.emplace_back(score, mn);
        });
    sort(vecMasternodeScores.rbegin(), vecMasternodeScores.rend(), CompareScoreMN());
    return vecMasternodeScores;
}

bool CMasternodeMan::CheckInputs(CMasternodeBroadcast& mnb, int nChainHeight, int& nDoS)
{
    const auto& consensus = Params().GetConsensus();
    // incorrect ping or its sigTime
    if(mnb.lastPing.IsNull() || !mnb.lastPing.CheckAndUpdate(nDoS, false, true)) {
        return false;
    }

    // search existing Masternode list
    CMasternode* pmn = Find(mnb.vin.prevout);
    if (pmn != nullptr) {
        // nothing to do here if we already know about this masternode and it's enabled
        if (pmn->IsEnabled()) return true;
        // if it's not enabled, remove old MN first and continue
        else
            mnodeman.Remove(pmn->vin.prevout);
    }

    const Coin& collateralUtxo = pcoinsTip->AccessCoin(mnb.vin.prevout);
    if (collateralUtxo.IsSpent()) {
        LogPrint(BCLog::MASTERNODE,"mnb - vin %s spent\n", mnb.vin.prevout.ToString());
        return false;
    }

    // Check collateral value
    if (collateralUtxo.out.nValue != consensus.nMNCollateralAmt) {
        LogPrint(BCLog::MASTERNODE,"mnb - invalid amount for mnb collateral %s\n", mnb.vin.prevout.ToString());
        nDoS = 33;
        return false;
    }

    // Check collateral association with mnb pubkey
    CScript payee = GetScriptForDestination(mnb.pubKeyCollateralAddress.GetID());
    if (collateralUtxo.out.scriptPubKey != payee) {
        LogPrint(BCLog::MASTERNODE,"mnb - collateral %s not associated with mnb pubkey\n", mnb.vin.prevout.ToString());
        nDoS = 33;
        return false;
    }

    LogPrint(BCLog::MASTERNODE, "mnb - Accepted Masternode entry\n");
    const int utxoHeight = (int) collateralUtxo.nHeight;
    int collateralUtxoDepth = nChainHeight - utxoHeight + 1;
    if (collateralUtxoDepth < consensus.MasternodeCollateralMinConf()) {
        LogPrint(BCLog::MASTERNODE,"mnb - Input must have at least %d confirmations\n", consensus.MasternodeCollateralMinConf());
        // maybe we miss few blocks, let this mnb to be checked again later
        mapSeenMasternodeBroadcast.erase(mnb.GetHash());
        g_tiertwo_sync_state.EraseSeenMNB(mnb.GetHash());
        return false;
    }

    // verify that sig time is legit in past
    // should be at least not earlier than block when BCZ tx got MASTERNODE_MIN_CONFIRMATIONS
    CBlockIndex* pConfIndex = WITH_LOCK(cs_main, return chainActive[utxoHeight + consensus.MasternodeCollateralMinConf() - 1]); // block where tx got MASTERNODE_MIN_CONFIRMATIONS
    if (pConfIndex->GetBlockTime() > mnb.sigTime) {
        LogPrint(BCLog::MASTERNODE,"mnb - Bad sigTime %d for Masternode %s (%i conf block is at %d)\n",
                 mnb.sigTime, mnb.vin.prevout.hash.ToString(), consensus.MasternodeCollateralMinConf(), pConfIndex->GetBlockTime());
        return false;
    }

    // Good input
    return true;
}

int CMasternodeMan::ProcessMNBroadcast(CNode* pfrom, CMasternodeBroadcast& mnb)
{
    const uint256& mnbHash = mnb.GetHash();
    if (mapSeenMasternodeBroadcast.count(mnbHash)) { //seen
        g_tiertwo_sync_state.AddedMasternodeList(mnbHash);
        return 0;
    }

    int chainHeight = GetBestHeight();
    int nDoS = 0;
    if (!mnb.CheckAndUpdate(nDoS)) {
        return nDoS;
    }

    // make sure it's still unspent
    if (!CheckInputs(mnb, chainHeight, nDoS)) {
        return nDoS; // error set internally
    }

    // now that did the mnb checks, can add it.
    mapSeenMasternodeBroadcast.emplace(mnbHash, mnb);

    // All checks performed, add it
    LogPrint(BCLog::MASTERNODE,"%s - Got NEW Masternode entry - %s - %lli \n", __func__,
             mnb.vin.prevout.hash.ToString(), mnb.sigTime);
    CMasternode mn(mnb);
    if (!Add(mn)) {
        LogPrint(BCLog::MASTERNODE, "%s - Rejected Masternode entry %s\n", __func__,
                 mnb.vin.prevout.hash.ToString());
        return 0;
    }

    // if it matches our MN pubkey, then we've been remotely activated
    if (mnb.pubKeyMasternode == activeMasternode.pubKeyMasternode && mnb.protocolVersion == PROTOCOL_VERSION) {
        activeMasternode.EnableHotColdMasterNode(mnb.vin, mnb.addr);
    }

    // Relay only if we are synchronized and if the mnb address is not local.
    // Makes no sense to relay MNBs to the peers from where we are syncing them.
    bool isLocal = (mnb.addr.IsRFC1918() || mnb.addr.IsLocal()) && !Params().IsRegTestNet();
    if (!isLocal && g_tiertwo_sync_state.IsSynced()) mnb.Relay();

    // Add it as a peer
    g_connman->AddNewAddress(CAddress(mnb.addr, NODE_NETWORK), pfrom->addr, 2 * 60 * 60);

    // Update sync status
    g_tiertwo_sync_state.AddedMasternodeList(mnbHash);

    // All good
    return 0;
}

int CMasternodeMan::ProcessMNPing(CNode* pfrom, CMasternodePing& mnp)
{
    const uint256& mnpHash = mnp.GetHash();
    if (mapSeenMasternodePing.count(mnpHash)) return 0; //seen

    int nDoS = 0;
    if (mnp.CheckAndUpdate(nDoS)) return 0;

    if (nDoS > 0) {
        // if anything significant failed, mark that node
        return nDoS;
    } else {
        // if nothing significant failed, search existing Masternode list
        CMasternode* pmn = Find(mnp.vin.prevout);
        // if it's known, don't ask for the mnb, just return
        if (pmn != NULL) return 0;
    }

    // something significant is broken or mn is unknown,
    // we might have to ask for the mn entry (while we aren't syncing).
    if (g_tiertwo_sync_state.IsSynced()) {
        AskForMN(pfrom, mnp.vin);
    }

    // All good
    return 0;
}

void CMasternodeMan::BroadcastInvMN(CMasternode* mn, CNode* pfrom)
{
    CMasternodeBroadcast mnb = CMasternodeBroadcast(*mn);
    const uint256& hash = mnb.GetHash();
    pfrom->PushInventory(CInv(MSG_MASTERNODE_ANNOUNCE, hash));

    // Add to mapSeenMasternodeBroadcast in case that isn't there for some reason.
    if (!mapSeenMasternodeBroadcast.count(hash)) mapSeenMasternodeBroadcast.emplace(hash, mnb);
}

int CMasternodeMan::ProcessGetMNList(CNode* pfrom, CTxIn& vin)
{
    // Single MN request
    if (!vin.IsNull()) {
        CMasternode* mn = Find(vin.prevout);
        if (!mn || !mn->IsEnabled()) return 0; // Nothing to return.

        // Relay the MN.
        BroadcastInvMN(mn, pfrom);
        LogPrint(BCLog::MASTERNODE, "dseg - Sent 1 Masternode entry to peer %i\n", pfrom->GetId());
        return 0;
    }

    // Check if the node asked for mn list sync before.
    bool isLocal = (pfrom->addr.IsRFC1918() || pfrom->addr.IsLocal());
    if (!isLocal) {
        auto itAskedUsMNList = mAskedUsForMasternodeList.find(pfrom->addr);
        if (itAskedUsMNList != mAskedUsForMasternodeList.end()) {
            int64_t t = (*itAskedUsMNList).second;
            if (GetTime() < t) {
                LogPrintf("CMasternodeMan::ProcessMessage() : dseg - peer already asked me for the list\n");
                return 20;
            }
        }
        int64_t askAgain = GetTime() + MASTERNODES_REQUEST_SECONDS;
        mAskedUsForMasternodeList[pfrom->addr] = askAgain;
    }

    int nInvCount = 0;
    {
        LOCK(cs);
        for (auto& it : mapMasternodes) {
            MasternodeRef& mn = it.second;
            if (mn->addr.IsRFC1918()) continue; //local network
            if (mn->IsEnabled()) {
                LogPrint(BCLog::MASTERNODE, "dseg - Sending Masternode entry - %s \n", mn->vin.prevout.hash.ToString());
                BroadcastInvMN(mn.get(), pfrom);
                nInvCount++;
            }
        }
    }

    g_connman->PushMessage(pfrom, CNetMsgMaker(pfrom->GetSendVersion()).Make(NetMsgType::SYNCSTATUSCOUNT, MASTERNODE_SYNC_LIST, nInvCount));
    LogPrint(BCLog::MASTERNODE, "dseg - Sent %d Masternode entries to peer %i\n", nInvCount, pfrom->GetId());

    // All good
    return 0;
}

bool CMasternodeMan::ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv, int& dosScore)
{
    dosScore = ProcessMessageInner(pfrom, strCommand, vRecv);
    return dosScore == 0;
}

int CMasternodeMan::ProcessMessageInner(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)
{
    return 0;
}

void CMasternodeMan::Remove(const COutPoint& collateralOut)
{
    LOCK(cs);
    const auto it = mapMasternodes.find(collateralOut);
    if (it != mapMasternodes.end()) {
        mapMasternodes.erase(it);
    }
}

void CMasternodeMan::UpdateMasternodeList(CMasternodeBroadcast& mnb)
{
    return;
}

int64_t CMasternodeMan::SecondsSincePayment(const MasternodeRef& mn, int count_enabled, const CBlockIndex* BlockReading) const
{
    int64_t sec = (GetAdjustedTime() - GetLastPaid(mn, count_enabled, BlockReading));
    int64_t month = 60 * 60 * 24 * 30;
    if (sec < month) return sec; //if it's less than 30 days, give seconds

    CHashWriter ss(SER_GETHASH, PROTOCOL_VERSION);
    ss << mn->vin;
    ss << mn->sigTime;
    const arith_uint256& hash = UintToArith256(ss.GetHash());

    // return some deterministic value for unknown/unpaid but force it to be more than 30 days old
    return month + hash.GetCompact(false);
}

int64_t CMasternodeMan::GetLastPaid(const MasternodeRef& mn, int count_enabled, const CBlockIndex* BlockReading) const
{
    if (BlockReading == nullptr) return false;

    const CScript& mnpayee = mn->GetPayeeScript();

    CHashWriter ss(SER_GETHASH, PROTOCOL_VERSION);
    ss << mn->vin;
    ss << mn->sigTime;
    const uint256& hash = ss.GetHash();

    // use a deterministic offset to break a tie -- 2.5 minutes
    int64_t nOffset = UintToArith256(hash).GetCompact(false) % 150;

    int max_depth = count_enabled * 1.25;
    for (int n = 0; n < max_depth; n++) {
        const auto& it = masternodePayments.mapMasternodeBlocks.find(BlockReading->nHeight);
        if (it != masternodePayments.mapMasternodeBlocks.end()) {
            // Search for this payee, with at least 2 votes. This will aid in consensus
            // allowing the network to converge on the same payees quickly, then keep the same schedule.
            if (it->second.HasPayeeWithVotes(mnpayee, 2))
                return BlockReading->nTime + nOffset;
        }
        BlockReading = BlockReading->pprev;

        if (BlockReading == nullptr || BlockReading->nHeight <= 0) {
            break;
        }
    }

    return 0;
}

std::string CMasternodeMan::ToString() const
{
    std::ostringstream info;
    info << "Masternodes: " << (int)mapMasternodes.size()
         << ", peers who asked us for Masternode list: " << (int)mAskedUsForMasternodeList.size()
         << ", peers we asked for Masternode list: " << (int)mWeAskedForMasternodeList.size()
         << ", entries in Masternode list we asked for: " << (int)mWeAskedForMasternodeListEntry.size();
    return info.str();
}

void CMasternodeMan::CacheBlockHash(const CBlockIndex* pindex)
{
    cvLastBlockHashes.Set(pindex->nHeight, pindex->GetBlockHash());
}

void CMasternodeMan::UncacheBlockHash(const CBlockIndex* pindex)
{
    cvLastBlockHashes.Set(pindex->nHeight, UINT256_ZERO);
}

uint256 CMasternodeMan::GetHashAtHeight(int nHeight) const
{
    // return zero if outside bounds
    if (nHeight < 0) {
        LogPrint(BCLog::MASTERNODE, "%s: Negative height. Returning 0\n",  __func__);
        return UINT256_ZERO;
    }
    int nCurrentHeight = GetBestHeight();
    if (nHeight > nCurrentHeight) {
        LogPrint(BCLog::MASTERNODE, "%s: height %d over current height %d. Returning 0\n",
                __func__, nHeight, nCurrentHeight);
        return UINT256_ZERO;
    }

    if (nHeight > nCurrentHeight - (int) CACHED_BLOCK_HASHES) {
        // Use cached hash
        return cvLastBlockHashes.Get(nHeight);
    } else {
        // Use chainActive
        LOCK(cs_main);
        return chainActive[nHeight]->GetBlockHash();
    }
}

bool CMasternodeMan::IsWithinDepth(const uint256& nHash, int depth) const
{
    // Sanity checks
    if (nHash.IsNull()) {
        return error("%s: Called with null hash\n", __func__);
    }
    if (depth < 0 || (unsigned) depth >= CACHED_BLOCK_HASHES) {
        return error("%s: Invalid depth %d. Cached block hashes: %d\n", __func__, depth, CACHED_BLOCK_HASHES);
    }
    // Check last depth blocks to find one with matching hash
    const int nCurrentHeight = GetBestHeight();
    int nStopHeight = std::max(0, nCurrentHeight - depth);
    for (int i = nCurrentHeight; i >= nStopHeight; i--) {
        if (GetHashAtHeight(i) == nHash)
            return true;
    }
    return false;
}

void ThreadCheckMasternodes()
{
    // Make this thread recognisable as the wallet flushing thread
    util::ThreadRename("bcz-masternodeman");
    LogPrintf("Masternodes thread started\n");

    unsigned int c = 0;

    try {
        // first clean up stale masternode payments data
        masternodePayments.CleanPaymentList(mnodeman.CheckAndRemove(), mnodeman.GetBestHeight());

        // Startup-only, clean any stored seen MN broadcast with an invalid service that
        // could have been invalidly stored on a previous release
        auto itSeenMNB = mnodeman.mapSeenMasternodeBroadcast.begin();
        while (itSeenMNB != mnodeman.mapSeenMasternodeBroadcast.end()) {
            if (!itSeenMNB->second.addr.IsValid()) {
                itSeenMNB = mnodeman.mapSeenMasternodeBroadcast.erase(itSeenMNB);
            } else {
                itSeenMNB++;
            }
        }

        while (true) {

            if (ShutdownRequested()) {
                break;
            }

            MilliSleep(1000);
            boost::this_thread::interruption_point();

            // try to sync from all available nodes, one step at a time
            masternodeSync.Process();

            if (g_tiertwo_sync_state.IsBlockchainSynced()) {
                c++;

                // check if we should activate or ping every few minutes,
                // start right after sync is considered to be done
                if (c % (MasternodePingSeconds()/2) == 0)
                    activeMasternode.ManageStatus();

                if (c % (MasternodePingSeconds()/5) == 0) {
                    masternodePayments.CleanPaymentList(mnodeman.CheckAndRemove(), mnodeman.GetBestHeight());
                }
            }
        }
    } catch (boost::thread_interrupted&) {
        // nothing, thread interrupted.
    }
}
