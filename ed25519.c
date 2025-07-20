/*
  +----------------------------------------------------------------------+
  | BLAKE3 PHP Extension                                                 |
  +----------------------------------------------------------------------+
  | Copyright (c) 2025 The Codernix ED25519 PHP Extension Authors        |
  +----------------------------------------------------------------------+
  | This source file is subject to the Apache 2.0 license that is        |
  | bundled with this package in the file LICENSE, and is available      |
  | through the world-wide-web at the following url:                     |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  |                                                                      |
  | If you did not receive a copy of the license and are unable to       |
  | obtain it through the world-wide-web, please send a note to          |
  | aboubakr@codernix.com so we can mail you a copy immediately.         |
  +----------------------------------------------------------------------+
  | Author: Abou-Bakr Seddik Ouahabi <aboubakr@codernix.com>             |
  | For the latest information, visit:                                   |
  | https://codernix.com/open-source/cryptography/ed25519-php-extension/  |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <string.h>

// This header will now be patched during the build to include the open_batch declaration.
#include "crypto_sign.h"

/*
 * ArgInfo definitions
 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_ed25519_keypair, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ed25519_sign, 0, 0, 2)
    ZEND_ARG_INFO(0, message)
    ZEND_ARG_INFO(0, secret_key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ed25519_open, 0, 0, 2)
    ZEND_ARG_INFO(0, signed_message)
    ZEND_ARG_INFO(0, public_key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ed25519_open_batch, 0, 0, 2)
    ZEND_ARG_ARRAY_INFO(0, signed_messages, 0)
    ZEND_ARG_ARRAY_INFO(0, public_keys, 0)
ZEND_END_ARG_INFO()

/*
 * ed25519_keypair()
 */
PHP_FUNCTION(ed25519_keypair)
{
    unsigned char pk[crypto_sign_PUBLICKEYBYTES];
    unsigned char sk[crypto_sign_SECRETKEYBYTES];

    if (crypto_sign_keypair(pk, sk) != 0) {
        RETURN_FALSE;
    }

    array_init(return_value);
    add_assoc_stringl(return_value, "public_key", (const char *)pk, crypto_sign_PUBLICKEYBYTES);
    add_assoc_stringl(return_value, "secret_key", (const char *)sk, crypto_sign_SECRETKEYBYTES);
}

/*
 * ed25519_sign()
 */
PHP_FUNCTION(ed25519_sign)
{
    char *message;
    size_t message_len;
    char *secret_key;
    size_t secret_key_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &message, &message_len, &secret_key, &secret_key_len) == FAILURE) {
        RETURN_FALSE;
    }

    if (secret_key_len != crypto_sign_SECRETKEYBYTES) {
        zend_error(E_WARNING, "ed25519_sign: secret_key must be %d bytes", crypto_sign_SECRETKEYBYTES);
        RETURN_FALSE;
    }

    unsigned long long sm_buffer_len = message_len + crypto_sign_BYTES;
    unsigned char *sm = emalloc(sm_buffer_len);
    unsigned long long actual_smlen = 0;

    if (crypto_sign(sm, &actual_smlen, (const unsigned char *)message, message_len, (const unsigned char *)secret_key) != 0) {
        efree(sm);
        RETURN_FALSE;
    }

    RETURN_STRINGL((char *)sm, (size_t)actual_smlen);
}

/*
 * ed25519_open()
 */
PHP_FUNCTION(ed25519_open)
{
    char *signed_message;
    size_t signed_message_len;
    char *public_key;
    size_t public_key_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &signed_message, &signed_message_len, &public_key, &public_key_len) == FAILURE) {
        RETURN_FALSE;
    }

    if (signed_message_len < crypto_sign_BYTES) {
        zend_error(E_WARNING, "ed25519_open: signed_message must be >= %d bytes", crypto_sign_BYTES);
        RETURN_FALSE;
    }

    if (public_key_len != crypto_sign_PUBLICKEYBYTES) {
        zend_error(E_WARNING, "ed25519_open: public_key must be %d bytes", crypto_sign_PUBLICKEYBYTES);
        RETURN_FALSE;
    }

    unsigned long long msg_buffer_len = signed_message_len - crypto_sign_BYTES;
    unsigned char *msg = emalloc(msg_buffer_len);
    unsigned long long actual_msg_len = 0;

    if (crypto_sign_open(msg, &actual_msg_len, (const unsigned char *)signed_message, signed_message_len, (const unsigned char *)public_key) != 0) {
        efree(msg);
        RETURN_FALSE;
    }

    RETURN_STRINGL((char *)msg, (size_t)actual_msg_len);
}

/*
 * ed25519_open_batch()
 */
PHP_FUNCTION(ed25519_open_batch)
{
    zval *z_sm_array, *z_pk_array;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "aa", &z_sm_array, &z_pk_array) == FAILURE) {
        RETURN_FALSE;
    }

    HashTable *ht_sm = Z_ARRVAL_P(z_sm_array);
    HashTable *ht_pk = Z_ARRVAL_P(z_pk_array);

    zend_long count_sm = zend_hash_num_elements(ht_sm);
    zend_long count_pk = zend_hash_num_elements(ht_pk);

    if (count_sm != count_pk) {
        zend_error(E_WARNING, "ed25519_open_batch: number of signed_messages and public_keys must match");
        RETURN_FALSE;
    }
    if (count_sm < 1) {
        array_init(return_value);
        return;
    }

    unsigned long long num = (unsigned long long)count_sm;

    unsigned char **sm = ecalloc(num, sizeof(unsigned char*));
    unsigned char **pk = ecalloc(num, sizeof(unsigned char*));
    unsigned char **m = ecalloc(num, sizeof(unsigned char*));
    unsigned long long *smlen = ecalloc(num, sizeof(unsigned long long));
    unsigned long long *mlen = ecalloc(num, sizeof(unsigned long long));

    array_init(return_value);
    zend_hash_real_init(Z_ARRVAL_P(return_value), 1);
    ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
        for (zend_long i = 0; i < count_sm; i++) {
            ZEND_HASH_FILL_ADD(NULL);
        }
    } ZEND_HASH_FILL_END();

    HashPosition pos_sm, pos_pk;
    zend_hash_internal_pointer_reset_ex(ht_sm, &pos_sm);
    zend_hash_internal_pointer_reset_ex(ht_pk, &pos_pk);

    zend_long idx = 0;
    while (idx < count_sm) {
        zval *z_sm_val = zend_hash_get_current_data_ex(ht_sm, &pos_sm);
        zval *z_pk_val = zend_hash_get_current_data_ex(ht_pk, &pos_pk);

        if (Z_TYPE_P(z_sm_val) != IS_STRING || Z_TYPE_P(z_pk_val) != IS_STRING) {
            mlen[idx] = (unsigned long long)-1;
            sm[idx] = NULL; pk[idx] = NULL; m[idx] = NULL;
        } else {
            sm[idx] = (unsigned char *)Z_STRVAL_P(z_sm_val);
            smlen[idx] = (unsigned long long)Z_STRLEN_P(z_sm_val);
            pk[idx] = (unsigned char *)Z_STRVAL_P(z_pk_val);

            if (smlen[idx] < crypto_sign_BYTES) {
                m[idx] = NULL;
                mlen[idx] = (unsigned long long)-1;
            } else {
                m[idx] = emalloc(smlen[idx]);
                mlen[idx] = 0;
            }
        }
        zend_hash_move_forward_ex(ht_sm, &pos_sm);
        zend_hash_move_forward_ex(ht_pk, &pos_pk);
        idx++;
    }

    crypto_sign_open_batch(m, mlen, sm, smlen, pk, num);

    HashTable *ht_result = Z_ARRVAL_P(return_value);
    zend_hash_internal_pointer_reset(ht_result);

    for (zend_long i = 0; i < count_sm; i++) {
        zval *z_result_slot = zend_hash_get_current_data(ht_result);
        if (mlen[i] == (unsigned long long)-1 || m[i] == NULL) {
            ZVAL_FALSE(z_result_slot);
        } else {
            ZVAL_STRINGL(z_result_slot, (char *)m[i], (size_t)mlen[i]);
        }
        zend_hash_move_forward(ht_result);
    }

    for (zend_long i = 0; i < count_sm; i++) {
        if (m[i]) efree(m[i]);
    }
    efree(m); efree(mlen); efree(sm); efree(pk); efree(smlen);
}

/*
 * Module Entry
 */
static const zend_function_entry ed25519_functions[] = {
    PHP_FE(ed25519_keypair,      arginfo_ed25519_keypair)
    PHP_FE(ed25519_sign,         arginfo_ed25519_sign)
    PHP_FE(ed25519_open,         arginfo_ed25519_open)
    PHP_FE(ed25519_open_batch,   arginfo_ed25519_open_batch)
    PHP_FE_END
};

zend_module_entry ed25519_module_entry = {
    STANDARD_MODULE_HEADER,
    "ed25519",
    ed25519_functions,
    NULL, NULL, NULL, NULL, NULL,
    "0.1.0",
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ED25519
ZEND_GET_MODULE(ed25519)
#endif