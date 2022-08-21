// Copyright (c) 2014-2018, The Monero Project
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#pragma once

#include <stdint.h>
#include <stddef.h>

#define CHACHA_KEY_SIZE 32
#define CHACHA_IV_SIZE 8

#if defined(__cplusplus)
#include <memory.h>

#include "common/memwipe.h"
#include "mlocker.h"
#include "hash.h"

namespace crypto {
  extern "C" {
#endif
    void chacha8(const void* data, size_t length, const uint8_t* key, const uint8_t* iv, char* cipher);
    void chacha20(const void* data, size_t length, const uint8_t* key, const uint8_t* iv, char* cipher);
#if defined(__cplusplus)
  }

  using chacha8_key = epee::mlocked<tools::scrubbed_arr<uint8_t, CHACHA_KEY_SIZE>>;

#pragma pack(push, 1)
  // MS VC 2012 doesn't interpret `class chacha8_iv` as POD in spite of [9.0.10], so it is a struct
  struct chacha8_iv {
    uint8_t data[CHACHA_IV_SIZE];
  };
#pragma pack(pop)

  static_assert(sizeof(chacha8_key) == CHACHA_KEY_SIZE && sizeof(chacha8_iv) == CHACHA_IV_SIZE, "Invalid structure size");

  inline void chacha8(const void* data, std::size_t length, const chacha8_key& key, const chacha8_iv& iv, char* cipher) {
    chacha8(data, length, key.data(), reinterpret_cast<const uint8_t*>(&iv), cipher);
  }

  inline void chacha20(const void* data, std::size_t length, const chacha8_key& key, const chacha8_iv& iv, char* cipher) {
    chacha20(data, length, key.data(), reinterpret_cast<const uint8_t*>(&iv), cipher);
  }

  inline void generate_chacha8_key(const void *data, size_t size, chacha8_key& key, int cn_variant = 0, bool prehashed=false) {
    static_assert(sizeof(chacha8_key) <= sizeof(hash), "Size of hash must be at least that of chacha8_key");
    epee::mlocked<tools::scrubbed_arr<char, HASH_SIZE>> pwd_hash;
    crypto::cn_slow_hash(data, size, pwd_hash.data(), cn_variant, prehashed, 0/*height*/);
    memcpy(&unwrap(unwrap(key)), pwd_hash.data(), sizeof(key));
  }

  inline void generate_chacha8_key_keccak(const void *data, size_t size, chacha8_key& key) {
    char result[HASH_SIZE];
    cn_fast_hash(data, size, result);
    memcpy(&key, data, sizeof(key));
    // Clean, for safety ???
    memset(result, 0, HASH_SIZE);
  }

  inline void do_chacha_crypt(const void *src, size_t src_size, void *target, const void *key, size_t key_size) {
    crypto::chacha8_key ckey;
    crypto::chacha8_iv civ;
    memset(&civ, 0, sizeof(civ));
    crypto::generate_chacha8_key_keccak(key, key_size, ckey);
    crypto::chacha8(src, src_size, ckey, civ, (char*)target);
  }

  template<typename T>
  inline bool do_chacha_crypt(std::string& buff, const T& pass)
  {
    std::string buff_target;
    buff_target.resize(buff.size());
    do_chacha_crypt(buff.data(), buff.size(), (void*)buff_target.data(), &pass, sizeof(pass));
    buff = buff_target;
    return true;
  }

  inline void generate_chacha8_key(std::string password, chacha8_key& key) {
    return generate_chacha8_key(password.data(), password.size(), key);
  }
}

#endif
