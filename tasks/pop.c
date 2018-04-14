/** Proof-of-presence prototocol demo. **/
/** TODO: Move nrf_crypto dependencies to platform **/
/** TODO: Move to library? **/
#include "tasks.h"
#include "ruuvi_error.h"

#include "nrf_crypto.h"
#include "nrf_crypto_ecc.h"
#include "nrf_crypto_error.h"
#include "nrf_crypto_ecdsa.h"

#define PLATFORM_LOG_MODULE_NAME POP
#if MAIN_LOG_ENABLED
#define PLATFORM_LOG_LEVEL       MAIN_LOG_LEVEL
#define PLATFORM_LOG_INFO_COLOR  MAIN_INFO_COLOR
#else // ANT_BPWR_LOG_ENABLED
#define PLATFORM_LOG_LEVEL       0
#endif // ANT_BPWR_LOG_ENABLED
#include "platform_log.h"
PLATFORM_LOG_MODULE_REGISTER();

 // * 0x00: Request challenge
 // * 0x01: first 16 bytes of challenge hash
 // * 0x02: second 16 bytes of challenge hash
 // * 0x03: first  16 bytes of challenge signature
 // * 0x04: second 16 bytes of challenge signature
 // * 0x05: first  16 bytes of app public key
 // * 0x06: second 16 bytes of app public key
 // * 0x07: third  16 bytes of app public key
 // * 0x08: fourth 16 bytes of app public key
 // * 0x0A: Challenge accepted
 // * 0x0B: Challenge rejected
 // * 0x0F: Internal error in challenge, challenge must be restarted. 

#define POP_REQUEST_CHALLENGE (0x00) // APP: Request random string
#define POP_CHALLENGE_1       (0x01) // APP: RESPOND to challenge
#define POP_CHALLENGE_2       (0x02) // APP: Request signed message
#define POP_SIGN_1            (0x03) // APP: Request signed message
#define POP_SIGN_2            (0x04) // APP: Request signed message
#define POP_HASH_1            (0x05) // APP: Request signed message
#define POP_HASH_2            (0x06) // APP: Request signed message
#define POP_HASH_3            (0x07) // APP: Request signed message
#define POP_HASH_4            (0x08) // APP: Request signed message

#define MESSAGE_SOURCE_INDEX 0
#define MESSAGE_DESTINATION_INDEX 1
#define MESSAGE_TYPE_INDEX    2
#define MESSAGE_PAYLOAD_LENGTH_INDEX 3
#define MESSAGE_PAYLOAD_INDEX 4

// static uint8_t m_sign[32];
static uint8_t m_hash[32];
static uint8_t m_tag_raw_private_key[32];
static uint8_t m_tag_raw_public_key [64];
// static uint8_t m_app_raw_public_key [64];
static nrf_crypto_ecc_private_key_t my_private_key;
static nrf_crypto_ecc_public_key_t  my_public_key;
static nrf_crypto_ecdsa_secp256r1_signature_t m_signature;
static size_t m_signature_size;
static uint8_t message_source;
static uint8_t message_destination;

ruuvi_status_t task_bluetooth_bulk_read_ascii(uint8_t* data, size_t data_length)
{
  return RUUVI_ERROR_NOT_IMPLEMENTED;
}

ruuvi_status_t task_bluetooth_bulk_read_bin(uint8_t* data, size_t data_length)
{
  return RUUVI_ERROR_NOT_IMPLEMENTED;
}

ruuvi_status_t task_bluetooth_bulk_read_reset(uint8_t* data, size_t data_length)
{
  return RUUVI_ERROR_NOT_IMPLEMENTED;
}

ruuvi_status_t task_generate_keys(void)
{
  nrf_crypto_ecc_key_pair_generate_context_t  keygen_context;
  ret_code_t err_code = nrf_crypto_ecc_key_pair_generate(
                          &keygen_context,                             // Keygen context
                          &g_nrf_crypto_ecc_secp256r1_curve_info,     // Info structure
                          &my_private_key,                            // Output private key
                          &my_public_key);                            // Output public key

  size_t raw_key_size = sizeof(my_public_key);
  err_code |= nrf_crypto_ecc_public_key_to_raw(&my_public_key,           // Public key
              m_tag_raw_public_key, // Raw key
              &raw_key_size);        // Raw size

  raw_key_size = sizeof(my_private_key);
  err_code |= nrf_crypto_ecc_private_key_to_raw  (&my_private_key,
              m_tag_raw_private_key,
              &raw_key_size);

  PLATFORM_LOG_INFO("Generated private key")
  PLATFORM_LOG_HEXDUMP_INFO(m_tag_raw_private_key, sizeof(m_tag_raw_private_key));

  PLATFORM_LOG_INFO("Generated public key")
  PLATFORM_LOG_HEXDUMP_INFO(m_tag_raw_public_key, sizeof(m_tag_raw_public_key));

  return platform_to_ruuvi_error(&err_code);
}

ruuvi_status_t task_generate_challenge_hash(void)
{
  ret_code_t err_code = NRF_SUCCESS;
  err_code |= nrf_crypto_rng_vector_generate(m_hash, sizeof(m_hash));
  PLATFORM_LOG_INFO("Generated challenge hash, status 0x%X", err_code);
  PLATFORM_LOG_HEXDUMP_INFO(m_hash, sizeof(m_hash));
  return platform_to_ruuvi_error(&err_code);
}

ruuvi_status_t task_sha256_message(uint8_t* message, size_t message_size)
{
  PLATFORM_LOG_INFO("Generating data hash of %d bytes", message_size);
  ret_code_t                                 err_code;
  nrf_crypto_hash_context_t           hash_context;
  nrf_crypto_hash_sha256_digest_t    hash_digest;
  uint32_t digest_size = sizeof(hash_digest);
  nrf_crypto_hash_context_t * const p_context = (void*)&hash_context;
  size_t * const p_size = (void*)&digest_size;
  uint8_t* p_digest = (void*)&(hash_digest[0]);
  err_code = nrf_crypto_hash_calculate(
               p_context,                     // Context
               &g_nrf_crypto_hash_sha256_info,    // Info structure
               message,                           // Input buffer
               message_size,                      // Input size
               p_digest,                       // Result buffer
               p_size);                     // Result size
  PLATFORM_LOG_INFO("Generated data hash of %d bytes, status 0x%X", message_size, err_code);
  memcpy(&m_hash, &hash_digest, sizeof(hash_digest));
  PLATFORM_LOG_HEXDUMP_INFO(m_hash, sizeof(m_hash));
  return platform_to_ruuvi_error(&err_code);
}

ruuvi_status_t task_sign_hash(void)
{
  ret_code_t err_code = NRF_SUCCESS;
  m_signature_size = sizeof(m_signature);
  err_code = nrf_crypto_ecdsa_sign(NULL,
                                   &my_private_key,
                                   m_hash,
                                   sizeof(m_hash),
                                   m_signature,
                                   &m_signature_size);
  PLATFORM_LOG_INFO("Generated signature")
  PLATFORM_LOG_HEXDUMP_INFO(m_signature, m_signature_size);

  return platform_to_ruuvi_error(&err_code);
}

ruuvi_status_t task_verify_hash(void)
{
  ret_code_t err_code = NRF_SUCCESS;
  err_code = nrf_crypto_ecdsa_verify(NULL,
                                     &my_public_key,
                                     m_hash,
                                     sizeof(m_hash),
                                     m_signature,
                                     m_signature_size);

  if (err_code == NRF_SUCCESS)
  {
    PLATFORM_LOG_INFO("Signature is valid. Message is authentic.");
  }
  else if (err_code == NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE)
  {
    PLATFORM_LOG_WARNING("Signature is invalid. Message is not authentic.");
  }

  return platform_to_ruuvi_error(&err_code);
}

// #define MESSAGE_SOURCE_INDEX         0
// #define MESSAGE_DESTINATION_INDEX    1
// #define MESSAGE_TYPE_INDEX           2
// #define MESSAGE_PAYLOAD_LENGTH_INDEX 3
// #define MESSAGE_PAYLOAD_INDEX        4

ruuvi_status_t task_pop_send_challenge(void)
{
  // Generate challenge
  ruuvi_status_t err_code = RUUVI_SUCCESS;
  err_code = task_generate_challenge_hash();
  if(RUUVI_SUCCESS != err_code) { PLATFORM_LOG_ERROR("POP Challenge generation failed"); }
  // Empty message
  uint8_t hash_payload[20] = {0};
  //Setup source, destination reply
  hash_payload[MESSAGE_SOURCE_INDEX] = message_destination;
  hash_payload[MESSAGE_DESTINATION_INDEX] = message_source;
  // Set message type and paylload length
  hash_payload[MESSAGE_TYPE_INDEX] = POP_CHALLENGE_1;
  hash_payload[MESSAGE_PAYLOAD_LENGTH_INDEX] = 16;
  memcpy(&hash_payload[MESSAGE_PAYLOAD_INDEX], &m_hash[0], 16);
  err_code |= task_bluetooth_send_asynchronous(m_hash, 16);
  hash_payload[MESSAGE_TYPE_INDEX] = POP_CHALLENGE_2;
  memcpy(&hash_payload[MESSAGE_PAYLOAD_INDEX], &m_hash[16], 16); 
  err_code |= task_bluetooth_send_asynchronous(m_hash, 16);
  if(RUUVI_SUCCESS != err_code) { PLATFORM_LOG_ERROR("POP Challenge send failed"); }
  return err_code;
}

ruuvi_status_t task_pop_reset_state(void)
{
  return RUUVI_ERROR_NOT_IMPLEMENTED;
}