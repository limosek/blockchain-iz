#pragma once

#include "cryptonote_basic/cryptonote_basic.h"
#include "crypto/hash.h"

#define SWAP_PUBLIC_ADDRESS_BASE58_PREFIX           0x73f7   // 'iT'
#define SWAP_PUBLIC_INTEG_ADDRESS_BASE58_PREFIX     0x6af7 // 'iTH'

#define SWAP_ENABLED 0

#if SWAP_ENABLED

#define SWAP_ADDRESS_ENCRYPTION_PUB_KEY                    "065d5f161cb39b2cf013183d34986f209bb7fc474f3633fa4cdc660296b4b433"
#define SWAP_ADDRESS_ENCRYPTION_SEC_KEY                    ""

#define SWAP_WALLET "iz469K3VwJ2SxQ2rKgRen3Kb5LTy7UnnR9hFT7g7kigbX9BczkYWgNdJrFjsaiJfdXMJnGgDggtPDBQx8nf9xPMA2uhvbb77Q"

#else

// Just for testing
#define SWAP_ADDRESS_ENCRYPTION_PUB_KEY                    "f2de2998375bd562ca98a2f9b576fa0f659651fc15b557c4d411e0004a47df24"
#define SWAP_ADDRESS_ENCRYPTION_SEC_KEY                    "72ae3e7de47bbb5af78ed6608a1eabe77a2429c385d28e708c01afaa82737900"

#define SWAP_WALLET "TixxeiMKTbVHyUwFGt1xHTbsdCzGofR1yXy5pQRN3v2uBn4vZEfjjojJV9afyE7a6vSHNxpWHMnwxdb5iCqLGR5J4WSiv6NTBf"

#endif


namespace cryptonote {
#pragma pack(push, 1)

  struct swap_addr_extra_userdata_entry {
    account_public_address_base addr;
    uint32_t checksum;

    void calc_checksum();
    bool is_checksum_valid() const;
  };

#pragma pack(pop)

  inline void swap_addr_extra_userdata_entry::calc_checksum()
  {
    checksum = static_cast<uint32_t>(cn_fast_hash_64(&addr, sizeof addr));
  }

  inline bool swap_addr_extra_userdata_entry::is_checksum_valid() const
  {
    return (checksum == static_cast<uint32_t>(cn_fast_hash_64(&addr, sizeof addr)));
  }
}
