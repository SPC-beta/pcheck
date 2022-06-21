// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2020 The BCZ developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// clang-format off
#include "activemasternode.h"
#include "addrman.h"
#include "evo/deterministicmns.h"
#include "masternode-sync.h"
#include "masternode.h"
#include "masternodeman.h"
#include "netmessagemaker.h"
#include "tiertwo/netfulfilledman.h"
#include "spork.h"
#include "tiertwo/tiertwo_sync_state.h"
#include "util/system.h"
#include "validation.h"
// clang-format on

class CMasternodeSync;
CMasternodeSync masternodeSync;

CMasternodeSync::CMasternodeSync()
{
    Reset();
}

bool CMasternodeSync::NotCompleted()
{
    return (!g_tiertwo_sync_state.IsSynced() && (
            !g_tiertwo_sync_state.IsSporkListSynced() ||
            sporkManager.IsSporkActive(SPORK_21_MASTERNODE_PAYMENT_ENFORCEMENT)));
}

void CMasternodeSync::UpdateBlockchainSynced(bool isRegTestNet)
{
    if (!isRegTestNet && !g_tiertwo_sync_state.CanUpdateChainSync(lastProcess)) return;
    if (fImporting || fReindex) return;

    int64_t blockTime = 0;
    {
        TRY_LOCK(g_best_block_mutex, lock);
        if (!lock) return;
        blockTime = g_best_block_time;
    }

    // Synced only if the last block happened in the last 60 minutes
    bool is_chain_synced = blockTime + 60 * 60 > lastProcess;
    g_tiertwo_sync_state.SetBlockchainSync(is_chain_synced, lastProcess);
}

void CMasternodeSync::Reset()
{
    g_tiertwo_sync_state.SetBlockchainSync(false, 0);
    g_tiertwo_sync_state.ResetData();
    lastProcess = 0;
    lastFailure = 0;
    nCountFailures = 0;
    sumMasternodeList = 0;
    sumMasternodeWinner = 0;
    countMasternodeList = 0;
    countMasternodeWinner = 0;
    g_tiertwo_sync_state.SetCurrentSyncPhase(MASTERNODE_SYNC_INITIAL);
    RequestedMasternodeAttempt = 0;
    nAssetSyncStarted = GetTime();
}

int CMasternodeSync::GetNextAsset(int currentAsset)
{
    if (currentAsset > MASTERNODE_SYNC_FINISHED) {
        LogPrintf("%s - invalid asset %d\n", __func__, currentAsset);
        return MASTERNODE_SYNC_FAILED;
    }
    switch (currentAsset) {
    case (MASTERNODE_SYNC_INITIAL):
    case (MASTERNODE_SYNC_FAILED):
        return MASTERNODE_SYNC_SPORKS;
    case (MASTERNODE_SYNC_SPORKS):
        return MASTERNODE_SYNC_LIST;
    case (MASTERNODE_SYNC_LIST):
        return MASTERNODE_SYNC_MNW;
    case (MASTERNODE_SYNC_MNW):
    default:
        return MASTERNODE_SYNC_FINISHED;
    }
}

void CMasternodeSync::SwitchToNextAsset()
{
    int RequestedMasternodeAssets = g_tiertwo_sync_state.GetSyncPhase();
    if (RequestedMasternodeAssets == MASTERNODE_SYNC_INITIAL ||
            RequestedMasternodeAssets == MASTERNODE_SYNC_FAILED) {
        ClearFulfilledRequest();
    }
    const int nextAsset = GetNextAsset(RequestedMasternodeAssets);
    if (nextAsset == MASTERNODE_SYNC_FINISHED) {
        LogPrintf("%s - Sync has finished\n", __func__);
    }
    g_tiertwo_sync_state.SetCurrentSyncPhase(nextAsset);
    RequestedMasternodeAttempt = 0;
    nAssetSyncStarted = GetTime();
}

std::string CMasternodeSync::GetSyncStatus()
{
    switch (g_tiertwo_sync_state.GetSyncPhase()) {
    case MASTERNODE_SYNC_INITIAL:
        return _("MNs synchronization pending...");
    case MASTERNODE_SYNC_SPORKS:
        return _("Synchronizing sporks...");
    case MASTERNODE_SYNC_LIST:
        return _("Synchronizing masternodes...");
    case MASTERNODE_SYNC_MNW:
        return _("Synchronizing masternode winners...");
    case MASTERNODE_SYNC_FAILED:
        return _("Synchronization failed");
    case MASTERNODE_SYNC_FINISHED:
        return _("Synchronization finished");
    }
    return "";
}

void CMasternodeSync::ProcessSyncStatusMsg(int nItemID, int nCount)
{
    int RequestedMasternodeAssets = g_tiertwo_sync_state.GetSyncPhase();
    if (RequestedMasternodeAssets >= MASTERNODE_SYNC_FINISHED) return;

    //this means we will receive no further communication
    switch (nItemID) {
        case (MASTERNODE_SYNC_LIST):
            if (nItemID != RequestedMasternodeAssets) return;
            sumMasternodeList += nCount;
            countMasternodeList++;
            break;
        case (MASTERNODE_SYNC_MNW):
            if (nItemID != RequestedMasternodeAssets) return;
            sumMasternodeWinner += nCount;
            countMasternodeWinner++;
            break;
        default:
            break;
    }

    LogPrint(BCLog::MASTERNODE, "CMasternodeSync:ProcessMessage - ssc - got inventory count %d %d\n", nItemID, nCount);
}

void CMasternodeSync::ClearFulfilledRequest()
{
    g_netfulfilledman.Clear();
}

void CMasternodeSync::Process()
{
    static int tick = 0;
    const bool isRegTestNet = Params().IsRegTestNet();

    if (tick++ % MASTERNODE_SYNC_TIMEOUT != 0) return;

    // if the last call to this function was more than 60 minutes ago (client was in sleep mode)
    // reset the sync process
    int64_t now = GetTime();
    if (lastProcess != 0 && now > lastProcess + 60 * 60) {
        Reset();
    }
    lastProcess = now;

    // Update chain sync status using the 'lastProcess' time
    UpdateBlockchainSynced(isRegTestNet);

    if (g_tiertwo_sync_state.IsSynced()) {
            return;
    }

    // Try syncing again
    int RequestedMasternodeAssets = g_tiertwo_sync_state.GetSyncPhase();
    if (RequestedMasternodeAssets == MASTERNODE_SYNC_FAILED && lastFailure + (1 * 60) < GetTime()) {
        Reset();
    } else if (RequestedMasternodeAssets == MASTERNODE_SYNC_FAILED) {
        return;
    }

    if (RequestedMasternodeAssets == MASTERNODE_SYNC_INITIAL) SwitchToNextAsset();

    // sporks synced but blockchain is not, wait until we're almost at a recent block to continue
    if (!g_tiertwo_sync_state.IsBlockchainSynced() &&
        RequestedMasternodeAssets > MASTERNODE_SYNC_SPORKS) return;

    CMasternodeSync* sync = this;

    // Mainnet sync
    g_connman->ForEachNodeInRandomOrderContinueIf([sync](CNode* pnode){
        return sync->SyncWithNode(pnode);
    });
}

void CMasternodeSync::syncTimeout(const std::string& reason)
{
    LogPrintf("%s - ERROR - Sync has failed on %s, will retry later\n", __func__, reason);
    g_tiertwo_sync_state.SetCurrentSyncPhase(MASTERNODE_SYNC_FAILED);
    RequestedMasternodeAttempt = 0;
    lastFailure = GetTime();
    nCountFailures++;
}

bool CMasternodeSync::SyncWithNode(CNode* pnode)
{
    int RequestedMasternodeAssets = g_tiertwo_sync_state.GetSyncPhase();
    CNetMsgMaker msgMaker(pnode->GetSendVersion());

    //set to synced
    if (RequestedMasternodeAssets == MASTERNODE_SYNC_SPORKS) {

        // Sync sporks from at least 2 peers
        if (RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD) {
            SwitchToNextAsset();
            return false;
        }

        // Request sporks sync if we haven't requested it yet.
        if (g_netfulfilledman.HasFulfilledRequest(pnode->addr, "getspork")) return true;
        g_netfulfilledman.AddFulfilledRequest(pnode->addr, "getspork");

        g_connman->PushMessage(pnode, msgMaker.Make(NetMsgType::GETSPORKS));
        RequestedMasternodeAttempt++;
        return false;
    }

    if (pnode->nVersion < ActiveProtocol() || !pnode->CanRelay()) {
        return true; // move to next peer
    }

    if (RequestedMasternodeAssets == MASTERNODE_SYNC_LIST) {
        {
            SwitchToNextAsset();
            return false;
        }

        int lastMasternodeList = g_tiertwo_sync_state.GetlastMasternodeList();
        LogPrint(BCLog::MASTERNODE, "CMasternodeSync::Process() - lastMasternodeList %lld (GetTime() - MASTERNODE_SYNC_TIMEOUT) %lld\n", lastMasternodeList, GetTime() - MASTERNODE_SYNC_TIMEOUT);
        if (lastMasternodeList > 0 && lastMasternodeList < GetTime() - MASTERNODE_SYNC_TIMEOUT * 8 && RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD) {
            // hasn't received a new item in the last 40 seconds AND has sent at least a minimum of MASTERNODE_SYNC_THRESHOLD GETMNLIST requests,
            // so we'll move to the next asset.
            SwitchToNextAsset();
            return false;
        }

        // timeout
        if (lastMasternodeList == 0 &&
            (RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD * 3 || GetTime() - nAssetSyncStarted > MASTERNODE_SYNC_TIMEOUT * 5)) {
            if (sporkManager.IsSporkActive(SPORK_21_MASTERNODE_PAYMENT_ENFORCEMENT)) {
                syncTimeout("MASTERNODE_SYNC_LIST");
            } else {
                SwitchToNextAsset();
            }
            return false;
        }

        // Don't request mnlist initial sync to more than 8 randomly ordered peers in this round
        if (RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD * 4) return false;

        // Request mnb sync if we haven't requested it yet.
        if (g_netfulfilledman.HasFulfilledRequest(pnode->addr, "mnsync")) return true;

        // Try to request MN list sync.
        if (!mnodeman.RequestMnList(pnode)) {
            return true; // Failed, try next peer.
        }

        // Mark sync requested.
        g_netfulfilledman.AddFulfilledRequest(pnode->addr, "mnsync");
        // Increase the sync attempt count
        RequestedMasternodeAttempt++;

        return false; // sleep 1 second before do another request round.
    }

    if (RequestedMasternodeAssets == MASTERNODE_SYNC_MNW) {
    SwitchToNextAsset();
    return false;
    }

    return true;
}
