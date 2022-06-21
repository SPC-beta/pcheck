// Copyright (c) 2014-2016 The Dash developers
// Copyright (c) 2016-2020 The BCZ developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SPORKID_H
#define SPORKID_H

/*
    Don't ever reuse these IDs for other sporks
    - This would result in old clients getting confused about which spork is for what
*/

enum SporkId : int32_t {
    SPORK_2_SWIFTTX                             = 10002,
    SPORK_3_SWIFTTX_BLOCK_FILTERING             = 10003,
    SPORK_4_BAN_MISBEHAVING                     = 10004,
    SPORK_7_SAPLING_MAINTENANCE                 = 10007,
    SPORK_9_LLMQ_DKG_MAINTENANCE                = 10009,
    SPORK_14_NEW_PROTOCOL_ENFORCEMENT           = 10014,
    SPORK_21_MASTERNODE_PAYMENT_ENFORCEMENT     = 10021,
    SPORK_22_MASTERNODE_PAYMENT                 = 10022,
    SPORK_26_COLDSTAKING_MAINTENANCE            = 10026,
    SPORK_INVALID                               = -1
};

// Default values
struct CSporkDef
{
    CSporkDef(): sporkId(SPORK_INVALID), defaultValue(0) {}
    CSporkDef(SporkId id, int64_t val, std::string n): sporkId(id), defaultValue(val), name(n) {}
    SporkId sporkId;
    int64_t defaultValue;
    std::string name;
};

#endif
