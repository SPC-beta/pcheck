// Copyright (c) 2022 The BCZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php.

#ifndef BCZ_BLS_KEY_IO_H
#define BCZ_BLS_KEY_IO_H

#include "optional.h"

class CChainParams;
class CBLSPublicKey;
class CBLSSecretKey;

namespace bls {

    std::string EncodeSecret(const CChainParams& params, const CBLSSecretKey& key);
    Optional<CBLSSecretKey> DecodeSecret(const CChainParams& params, const std::string& keyStr);

    std::string EncodePublic(const CChainParams& params, const CBLSPublicKey& pk);
    Optional<CBLSPublicKey> DecodePublic(const CChainParams& params, const std::string& keyStr);

} // end bls namespace

#endif //BCZ_BLS_KEY_IO_H
