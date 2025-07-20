#ifndef crypto_sign_H
#define crypto_sign_H
#define crypto_sign_ed25519_BYTES 64
#define crypto_sign_ed25519_PUBLICKEYBYTES 32
#define crypto_sign_ed25519_SECRETKEYBYTES 64
#define crypto_sign_ed25519 crypto_sign_ed25519_amd64_64_24k
#define crypto_sign crypto_sign_ed25519
#define crypto_sign_BYTES crypto_sign_ed25519_BYTES
#define crypto_sign_PUBLICKEYBYTES crypto_sign_ed25519_PUBLICKEYBYTES
#define crypto_sign_SECRETKEYBYTES crypto_sign_ed25519_SECRETKEYBYTES
#ifdef __cplusplus
extern "C" {
#endif
extern int crypto_sign_keypair(unsigned char *,unsigned char *);
extern int crypto_sign(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_open(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
/* Declaration added for the PHP extension's batch verification function */
extern int crypto_sign_open_batch(
    unsigned char* const m[],
    unsigned long long mlen[],
    unsigned char* const sm[],
    const unsigned long long smlen[],
    unsigned char* const pk[],
    unsigned long long num
);
#ifdef __cplusplus
}
#endif
#endif
