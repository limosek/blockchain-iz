// Copyright (c) 2014-2017, The Monero Project
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//		conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//		of conditions and the following disclaimer in the documentation and/or other
//		materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//		used to endorse or promote products derived from this software without specific
//		prior written permission.
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
// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2014-2018, The Aeon Project
// Copyright (c) 2018-2019, The TurtleCoin Developers

#pragma once

#include <stddef.h>


#include "argon2.h"
#include "common/pod-class.h"
#include "generic-ops.h"

// Chukwa Common Definitions
#define CHUKWA_HASHLEN 32 // The length of the resulting hash in bytes
#define CHUKWA_SALTLEN 16 // The length of our salt in bytes

// Chukwa v1 Definitions
#define CHUKWA_THREADS_V1 1 // How many threads to use at once
#define CHUKWA_ITERS_V1 3 // How many iterations we perform as part of our slow-hash
#define CHUKWA_MEMORY_V1 512 // This value is in KiB (0.5MB)

// Chukwa v2 Definitions
#define CHUKWA_THREADS_V2 1 // How many threads to use at once
#define CHUKWA_ITERS_V2 4 // How many iterations we perform as part of our slow-hash
#define CHUKWA_MEMORY_V2 1024 // This value is in KiB (1.00MB)

namespace crypto {

	extern "C" {
#include "hash-ops.h"
	}

#pragma pack(push, 1)
	POD_CLASS hash {
		char data[HASH_SIZE];
	};
	POD_CLASS hash8 {
		char data[8];
	};
#pragma pack(pop)

	static_assert(sizeof(hash) == HASH_SIZE, "Invalid structure size");
	static_assert(sizeof(hash8) == 8, "Invalid structure size");

    static bool argon2_optimization_selected = false;

	/*
		Cryptonight hash functions
	*/

	inline void cn_fast_hash(const void *data, std::size_t length, hash &hash) {
		cn_fast_hash(data, length, reinterpret_cast<char *>(&hash));
	}

	inline hash cn_fast_hash(const void *data, std::size_t length) {
		hash h;
		cn_fast_hash(data, length, reinterpret_cast<char *>(&h));
		return h;
	}

  inline uint64_t cn_fast_hash_64(const void *data, size_t length) {
    hash result_hash = cn_fast_hash(data, length);
    uint8_t idx = reinterpret_cast<uint8_t*>(&result_hash)[5] % 4;
    return reinterpret_cast<uint64_t*>(&result_hash)[idx];
  }

	inline void cn_slow_hash(const void *data, std::size_t length, hash &hash, int variant = 0, uint64_t height = 0) {
		cn_slow_hash(data, length, reinterpret_cast<char *>(&hash), variant, 0/*prehashed*/, height);
	}

	inline void cn_slow_hash_prehashed(const void *data, std::size_t length, hash &hash, int variant = 0, uint64_t height = 0) {
		cn_slow_hash(data, length, reinterpret_cast<char *>(&hash), variant, 1/*prehashed*/, height);
	}

	inline void chukwa_slow_hash_base(
		const void *data,
		size_t length,
		hash &out_hash,
		const size_t iterations,
		const size_t memory,
		const size_t threads)
	{
		uint8_t salt[CHUKWA_SALTLEN];
		memcpy(salt, data, sizeof(salt));

		/* If this is the first time we've called this hash function then
			 we need to have the Argon2 library check to see if any of the
			 available CPU instruction sets are going to help us out */
		if (!argon2_optimization_selected)
		{
				/* Call the library quick benchmark test to set which CPU
					 instruction sets will be used */
				argon2_select_impl(NULL, NULL);

				argon2_optimization_selected = true;
		}

		argon2id_hash_raw(iterations, memory, threads, data, length, salt, CHUKWA_SALTLEN, out_hash.data, CHUKWA_HASHLEN);
	}

	inline void chukwa_slow_hash_v1(const void *data, size_t length, hash &out_hash)
	{
		chukwa_slow_hash_base(data, length, out_hash, CHUKWA_ITERS_V1, CHUKWA_MEMORY_V1, CHUKWA_THREADS_V1);
	}

	inline void chukwa_slow_hash_v2(const void *data, size_t length, hash &out_hash)
	{
		chukwa_slow_hash_base(data, length, out_hash, CHUKWA_ITERS_V2, CHUKWA_MEMORY_V2, CHUKWA_THREADS_V2);
	}

	inline void tree_hash(const hash *hashes, std::size_t count, hash &root_hash) {
		tree_hash(reinterpret_cast<const char (*)[HASH_SIZE]>(hashes), count, reinterpret_cast<char *>(&root_hash));
	}

	inline void tree_branch(const hash* hashes, std::size_t count, hash* branch)	{
		tree_branch(reinterpret_cast<const char(*)[HASH_SIZE]>(hashes), count, reinterpret_cast<char(*)[HASH_SIZE]>(branch));
	}

	inline void tree_hash_from_branch(const hash* branch, std::size_t depth, const hash& leaf, const void* path, hash& root_hash) {
	tree_hash_from_branch(reinterpret_cast<const char(*)[HASH_SIZE]>(branch), depth, reinterpret_cast<const char*>(&leaf), path, reinterpret_cast<char*>(&root_hash));
	}

}

CRYPTO_MAKE_HASHABLE(hash)
CRYPTO_MAKE_COMPARABLE(hash8)
