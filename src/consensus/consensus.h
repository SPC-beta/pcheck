// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2019-2020 The BCZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_CONSENSUS_H
#define BITCOIN_CONSENSUS_CONSENSUS_H

#include "amount.h"
#include <stdint.h>

/** The maximum allowed size for a block, in bytes (only for buffer size limits) */
static const unsigned int MAX_BLOCK_SIZE = 4000000;

/** The maximum size of a transaction after Sapling activation (network rule) */
static const unsigned int MAX_STANDARD_TX_SIZE = 800000;

/** The maximum cumulative size of all Shielded txes inside a block */
static const unsigned int MAX_BLOCK_SHIELDED_TXES_SIZE = MAX_STANDARD_TX_SIZE;

/** The maximum allowed number of signature check operations in a block (network rule) */
static const unsigned int MAX_BLOCK_SIGOPS = MAX_BLOCK_SIZE / 50;

/** The maximum number of sigops we're willing to relay/mine in a single tx */
static const unsigned int MAX_TX_SIGOPS = MAX_BLOCK_SIGOPS / 5;

/** The minimum amount for the value of a P2CS output */
static const CAmount MIN_COLDSTAKING_AMOUNT = 100 * COIN;

/** The default maximum reorganization depth **/
static const int DEFAULT_MAX_REORG_DEPTH = 100;

/** Flags for nSequence and nLockTime locks */
/** Interpret sequence numbers as relative lock-time constraints. */
static constexpr unsigned int LOCKTIME_VERIFY_SEQUENCE = (1 << 0);
/** Use GetMedianTimePast() instead of nTime for end point timestamp. */
static constexpr unsigned int LOCKTIME_MEDIAN_TIME_PAST = (1 << 1);

#endif // BITCOIN_CONSENSUS_CONSENSUS_H
