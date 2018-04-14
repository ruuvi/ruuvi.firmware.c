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
// * 0x04: second 16 bytes of challenge nrf_crypto_ecc_private_key_to_rawure
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
static uint8_t tag_hash[32];
static uint8_t tag_raw_private_key[32];
static uint8_t tag_raw_public_key [64];
static nrf_crypto_ecc_private_key_t tag_private_key;
static nrf_crypto_ecc_public_key_t  tag_public_key;
static nrf_crypto_ecdsa_secp256r1_signature_t tag_signature;
static uint8_t app_hash[32];
static uint8_t app_raw_public_key [64];
static nrf_crypto_ecc_public_key_t  app_public_key;
static nrf_crypto_ecdsa_secp256r1_signature_t app_signature;
static size_t  signature_size;
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

ruuvi_status_t task_generate_keys(void)
{
  nrf_crypto_ecc_key_pair_generate_context_t  keygen_context;
  ret_code_t err_code = nrf_crypto_ecc_key_pair_generate(
                          &keygen_context,                             // Keygen context
                          &g_nrf_crypto_ecc_secp256r1_curve_info,     // Info structure
                          &tag_private_key,                            // Output private key
                          &tag_public_key);                            // Output public key

  size_t raw_key_size = sizeof(tag_raw_public_key);
  err_code |= nrf_crypto_ecc_public_key_to_raw(&tag_public_key,           // Public key
              tag_raw_public_key, // Raw key
              &raw_key_size);        // Raw size

  raw_key_size = sizeof(tag_raw_private_key);
  err_code |= nrf_crypto_ecc_private_key_to_raw  (&tag_private_key,
              tag_raw_private_key,
              &raw_key_size);

  PLATFORM_LOG_INFO("Generated private key")
  PLATFORM_LOG_HEXDUMP_INFO(tag_raw_private_key, sizeof(tag_raw_private_key));

  PLATFORM_LOG_INFO("Generated public key")
  PLATFORM_LOG_HEXDUMP_INFO(tag_raw_public_key, sizeof(tag_raw_public_key));

  return platform_to_ruuvi_error(&err_code);
}

ruuvi_status_t task_generate_challenge_hash(void)
{
  ret_code_t err_code = NRF_SUCCESS;
  err_code |= nrf_crypto_rng_vector_generate(tag_hash, sizeof(tag_hash));
  PLATFORM_LOG_INFO("Generated challenge hash, status 0x%X", err_code);
  PLATFORM_LOG_HEXDUMP_INFO(tag_hash, sizeof(tag_hash));
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
  uint8_t* p_digest = (void*) & (hash_digest[0]);
  err_code = nrf_crypto_hash_calculate(
               p_context,                     // Context
               &g_nrf_crypto_hash_sha256_info,    // Info structure
               message,                           // Input buffer
               message_size,                      // Input size
               p_digest,                       // Result buffer
               p_size);                     // Result size
  PLATFORM_LOG_INFO("Generated data hash of %d bytes, status 0x%X", message_size, err_code);
  memcpy(&tag_hash, &hash_digest, sizeof(hash_digest));
  PLATFORM_LOG_HEXDUMP_INFO(tag_hash, sizeof(tag_hash));
  return platform_to_ruuvi_error(&err_code);
}

ruuvi_status_t task_sign_hash(void)
{
  ret_code_t err_code = NRF_SUCCESS;
  signature_size = sizeof(tag_signature);
  err_code = nrf_crypto_ecdsa_sign(NULL,
                                   &tag_private_key,
                                   tag_hash,
                                   sizeof(tag_hash),
                                   tag_signature,
                                   &signature_size);
  PLATFORM_LOG_INFO("Generated signature")
  PLATFORM_LOG_HEXDUMP_INFO(tag_signature, signature_size);

  return platform_to_ruuvi_error(&err_code);
}



ruuvi_status_t task_pop_send_challenge(void)
{
  // Generate challenge
  ruuvi_status_t err_code = RUUVI_SUCCESS;
  err_code = task_generate_challenge_hash();
  if (RUUVI_SUCCESS != err_code) { PLATFORM_LOG_ERROR("POP Challenge generation failed"); }
  // Empty message
  uint8_t hash_payload[20] = {0};
  //Setup source, destination reply
  hash_payload[MESSAGE_SOURCE_INDEX] = message_destination;
  hash_payload[MESSAGE_DESTINATION_INDEX] = message_source;
  // Set message type and paylload length
  hash_payload[MESSAGE_TYPE_INDEX] = POP_CHALLENGE_1;
  hash_payload[MESSAGE_PAYLOAD_LENGTH_INDEX] = 16;
  memcpy(&hash_payload[MESSAGE_PAYLOAD_INDEX], &tag_hash[0], 16);
  err_code |= task_bluetooth_send_asynchronous(hash_payload, 20);
  hash_payload[MESSAGE_TYPE_INDEX] = POP_CHALLENGE_2;
  memcpy(&hash_payload[MESSAGE_PAYLOAD_INDEX], &tag_hash[16], 16);
  err_code |= task_bluetooth_send_asynchronous(hash_payload, 20);
  if (RUUVI_SUCCESS != err_code) { PLATFORM_LOG_ERROR("POP Challenge send failed"); }
  return err_code;
}

ruuvi_status_t task_pop_verify_challenge(void)
{
  ret_code_t err_code = NRF_SUCCESS;
  err_code = nrf_crypto_ecdsa_verify(NULL,
                                     &app_public_key,
                                     tag_hash,
                                     sizeof(tag_hash),
                                     app_signature,
                                     signature_size);
  //ADD debug prints here
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

ruuvi_status_t task_pop_process_incoming_data(uint8_t* message, size_t message_length)
{
  if (NULL == message) { return RUUVI_ERROR_NULL; }
  if (message_length < 4) { return RUUVI_ERROR_INVALID_LENGTH; }
  message_source = message[MESSAGE_SOURCE_INDEX];
  message_destination = message[MESSAGE_DESTINATION_INDEX];
  uint8_t data_type = message[MESSAGE_TYPE_INDEX];
  uint8_t payload_length = message[MESSAGE_PAYLOAD_LENGTH_INDEX];

  switch (data_type)
  {
  case POP_REQUEST_CHALLENGE:
    task_pop_send_challenge();
    break;

  case POP_SIGN_1:
    if (16 != message[MESSAGE_TYPE_INDEX])
    {
      task_pop_reset_state();
      return RUUVI_ERROR_INVALID_LENGTH;
    }
    memcpy(&app_hash[0], &message[MESSAGE_PAYLOAD_INDEX], payload_length);
    break;

  case POP_SIGN_2:
    if (16 != message[MESSAGE_TYPE_INDEX])
    {
      task_pop_reset_state();
      return RUUVI_ERROR_INVALID_LENGTH;
    }
    memcpy(&app_hash[16], &message[MESSAGE_PAYLOAD_INDEX], payload_length);
    break;

  case POP_HASH_1:
    if (16 != message[MESSAGE_TYPE_INDEX])
    {
      task_pop_reset_state();
      return RUUVI_ERROR_INVALID_LENGTH;
    }
    memcpy(&app_raw_public_key[0], &message[MESSAGE_PAYLOAD_INDEX], payload_length);
    break;

  case POP_HASH_2:
    if (16 != message[MESSAGE_TYPE_INDEX])
    {
      task_pop_reset_state();
      return RUUVI_ERROR_INVALID_LENGTH;
    }
    memcpy(&app_raw_public_key[16], &message[MESSAGE_PAYLOAD_INDEX], payload_length);
    break;

  case POP_HASH_3:
    if (16 != message[MESSAGE_TYPE_INDEX])
    {
      task_pop_reset_state();
      return RUUVI_ERROR_INVALID_LENGTH;
    }
    memcpy(&app_raw_public_key[32], &message[MESSAGE_PAYLOAD_INDEX], payload_length);
    break;
  case POP_HASH_4:
    if (16 != message[MESSAGE_TYPE_INDEX])
    {
      task_pop_reset_state();
      return RUUVI_ERROR_INVALID_LENGTH;
    }
    memcpy(&app_raw_public_key[48], &message[MESSAGE_PAYLOAD_INDEX], payload_length);
    ruuvi_status_t challenge_status = RUUVI_ERROR_NULL;
    challenge_status = task_pop_verify_challenge();
    if (RUUVI_SUCCESS == challenge_status) { task_pop_send_challenge_status(challenge_status); }
    else { task_pop_reset_state(); } // Reset state if there was an error.
    break;

  default:
    task_pop_reset_state();
    return RUUVI_ERROR_INVALID_PARAM;
  }
  return RUUVI_SUCCESS;
}

ruuvi_status_t task_pop_reset_state(void)
{
  return RUUVI_ERROR_NOT_IMPLEMENTED;
}

ruuvi_status_t task_pop_send_challenge_status(ruuvi_status_t challenge_status)
{
return RUUVI_ERROR_NOT_IMPLEMENTED; 
}