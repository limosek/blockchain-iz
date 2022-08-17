#pragma once

#include "cryptonote_basic/cryptonote_basic.h"
#include "crypto/hash.h"

#define SWAP_PUBLIC_ADDRESS_BASE58_PREFIX           0x73f7   // 'iT'
#define SWAP_PUBLIC_INTEG_ADDRESS_BASE58_PREFIX     0x6af7 // 'iTH'

namespace cryptonote {
#pragma pack(push, 1)

  struct swap_addr_extra_userdata_entry {
    account_public_address_base addr;
    uint32_t checksum;

    void calc_checksum();
    bool is_checksum_valid();
  };

#pragma pack(pop)

  inline void swap_addr_extra_userdata_entry::calc_checksum()
  {
    checksum = static_cast<uint32_t>(cn_fast_hash_64(&addr, sizeof addr));
  }

  inline bool swap_addr_extra_userdata_entry::is_checksum_valid()
  {
    return (checksum == static_cast<uint32_t>(cn_fast_hash_64(&addr, sizeof addr)));
  }
}
