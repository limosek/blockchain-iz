#include "gtest/gtest.h"
#include "cryptonote_basic/cryptonote_basic.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "cryptonote_core/cryptonote_tx_utils.h"
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

TEST(swap_address, swap_parse_addr_from_str)
{
  cryptonote::account_public_address new_chain_addr = AUTO_VAL_INIT(new_chain_addr);
  cryptonote::account_public_address old_chain_addr = AUTO_VAL_INIT(old_chain_addr);
  cryptonote::account_public_address invalid_addr = AUTO_VAL_INIT(invalid_addr);

  std::string new_chain_addr_str = "iTxtQTLo1H8aGripS8mzG6c1Y3WcoSzzBFb4o7o2xUJmfJgLxxscutm9s9EMYXYnaaUN1E1XpMUHREcFbqAeSBzk61pYu2Tjhs";
  std::string old_chain_addr_str = "iz5vDmtTSk7cyKdHr6Jv7vVSpx3thD6wGdXuyETg81KBfSVsZKeBewQUBF25uAtbJ3j9sHJzVHKfSNPu9aYbBnSv239GjxfER";
  std::string invalid_addr_str = "n05vDmtTSk7cyKdHr6Jv7vVSpx3thD6wGdXuyETg81KBfSVsZKeBewQUBF25uAtbJ3j9sHJzVHKfSNPu9aYbBnSv239GjxfER";

  bool has_payment_id;
  crypto::hash8 payment_id;

  ASSERT_TRUE(
      cryptonote::get_account_integrated_address_from_str(new_chain_addr,
      has_payment_id,
      payment_id,
      false,
      new_chain_addr_str)
      );
  ASSERT_TRUE(new_chain_addr.is_swap_addr);

  ASSERT_TRUE(
      cryptonote::get_account_integrated_address_from_str(old_chain_addr,
      has_payment_id,
      payment_id,
      false,
      old_chain_addr_str)
      );
  ASSERT_FALSE(old_chain_addr.is_swap_addr);

  ASSERT_FALSE(
      cryptonote::get_account_integrated_address_from_str(invalid_addr,
      has_payment_id,
      payment_id,
      false,
      invalid_addr_str)
      );
}

TEST(swap_tx, fill_tx_extra)
{
  cryptonote::transaction tx = AUTO_VAL_INIT(tx);
  cryptonote::account_public_address swap_addr = AUTO_VAL_INIT(swap_addr);
  generate_random_bytes_not_thread_safe(sizeof(swap_addr.m_view_public_key), &swap_addr.m_view_public_key);
  generate_random_bytes_not_thread_safe(sizeof(swap_addr.m_spend_public_key), &swap_addr.m_spend_public_key);
  swap_addr.is_swap_addr = true;

  crypto::hash payment_id;
  generate_random_bytes_not_thread_safe(sizeof(crypto::hash), &payment_id);

  cryptonote::keypair txkey = cryptonote::keypair::generate();

  ASSERT_TRUE(set_swap_tx_extra(tx.extra, payment_id, swap_addr));
  ASSERT_TRUE(add_tx_pub_key_to_extra(tx, txkey.pub));
  ASSERT_TRUE(cryptonote::encrypt_user_data_with_tx_secret_key(txkey.sec, tx.extra));

  std::vector<cryptonote::tx_extra_field> extra_fields;

  ASSERT_TRUE(cryptonote::parse_tx_extra(tx.extra, extra_fields));

  cryptonote::tx_extra_nonce extra_nonce;
  if (find_tx_extra_field_by_type(extra_fields, extra_nonce)) {
    crypto::hash payment_id_fresh;
    ASSERT_TRUE(cryptonote::get_payment_id_from_swap_tx_extra_nonce(extra_nonce.nonce, payment_id_fresh));
    ASSERT_EQ(payment_id, payment_id_fresh);
  } else {
    ASSERT_TRUE(false);
  }

  crypto::secret_key swap_encrypt_sec_key = AUTO_VAL_INIT(swap_encrypt_sec_key);
  ASSERT_TRUE(epee::string_tools::hex_to_pod(SWAP_ADDRESS_ENCRYPTION_SEC_KEY, swap_encrypt_sec_key));

  cryptonote::account_public_address swap_addr2 = AUTO_VAL_INIT(swap_addr2);
  ASSERT_TRUE(cryptonote::get_swap_data_from_tx(tx, swap_encrypt_sec_key, swap_addr2));

  ASSERT_EQ(swap_addr.m_spend_public_key, swap_addr2.m_spend_public_key);
  ASSERT_EQ(swap_addr.m_view_public_key, swap_addr2.m_view_public_key);

  vector<cryptonote::tx_destination_entry> fake_dest = {
    cryptonote::tx_destination_entry(80085, swap_addr)
  };

  ASSERT_TRUE(cryptonote::is_swap_tx(tx, fake_dest));
}
