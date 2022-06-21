// Copyright (c) 2014-2020 The Dash Core developers
// Copyright (c) 2022 The BCZ Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php.

#ifndef BCZ_NETFULFILLEDMAN_H
#define BCZ_NETFULFILLEDMAN_H

#include "serialize.h"
#include "sync.h"

#include <map>

class CService;

static const std::string NET_REQUESTS_CACHE_FILENAME = "netrequests.dat";
static const std::string NET_REQUESTS_CACHE_FILE_ID = "magicNetRequestsCache";

// Fulfilled requests are used to prevent nodes from asking the same data on sync
// and being banned for doing it too often.
class CNetFulfilledRequestManager
{
private:
    typedef std::map<std::string, int64_t> fulfilledreqmapentry_t;
    typedef std::map<CService, fulfilledreqmapentry_t> fulfilledreqmap_t;

    // Keep track of what node has/was asked for and when
    fulfilledreqmap_t mapFulfilledRequests;
    mutable Mutex cs_mapFulfilledRequests;

public:
    CNetFulfilledRequestManager() = default;

    SERIALIZE_METHODS(CNetFulfilledRequestManager, obj) {
        LOCK(obj.cs_mapFulfilledRequests);
        READWRITE(obj.mapFulfilledRequests);
    }

    void AddFulfilledRequest(const CService& addr, const std::string& strRequest);
    bool HasFulfilledRequest(const CService& addr, const std::string& strRequest) const;

    void CheckAndRemove();
    void Clear();

    std::string ToString() const;
    int Size() const { return WITH_LOCK(cs_mapFulfilledRequests, return mapFulfilledRequests.size();); }

    void DoMaintenance();
};

extern CNetFulfilledRequestManager g_netfulfilledman;

#endif // BCZ_NETFULFILLEDMAN_H
