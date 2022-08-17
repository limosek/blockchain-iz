#include "gtest/gtest.h"
#include "cryptonote_basic/cryptonote_basic.h"
#include "crypto/random.h"
#include "cryptonote_core/swap_address.h"

TEST(swap_address, swap_addr_extra_userdata_entry_from_addr)
{
  cryptonote::account_public_address swap_addr = AUTO_VAL_INIT(swap_addr);
  generate_random_bytes_not_thread_safe(sizeof(swap_addr.m_view_public_key), &swap_addr.m_view_public_key);
  generate_random_bytes_not_thread_safe(sizeof(swap_addr.m_spend_public_key), &swap_addr.m_spend_public_key);
  swap_addr.is_swap_addr = true;

  cryptonote::swap_addr_extra_userdata_entry entry;
  entry.addr = static_cast<const cryptonote::account_public_address_base&>(swap_addr);
  entry.calc_checksum();
  ASSERT_TRUE(entry.is_checksum_valid());
}
