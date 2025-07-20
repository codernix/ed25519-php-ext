#ifndef crypto_sign_H
#define crypto_sign_H

#define crypto_sign_BYTES crypto_sign_ed25519_amd64_64_24k_BYTES
#define crypto_sign_PUBLICKEYBYTES crypto_sign_ed25519_amd64_64_24k_PUBLICKEYBYTES
#define crypto_sign_SECRETKEYBYTES crypto_sign_ed25519_amd64_64_24k_SECRETKEYBYTES

/* Map generic API to the specific, namespaced implementation */
#define crypto_sign_keypair crypto_sign_ed25519_amd64_64_24k_keypair
#define crypto_sign crypto_sign_ed25519_amd64_64_24k_sign
#define crypto_sign_open crypto_sign_ed25519_amd64_64_24k_open
#define crypto_sign_open_batch crypto_sign_ed25519_amd64_64_24k_open_batch

#ifdef __cplusplus
extern "C" {
#endif

extern int crypto_sign_ed25519_amd64_64_24k_keypair(unsigned char *,unsigned char *);
extern int crypto_sign_ed25519_amd64_64_24k_sign(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_ed25519_amd64_64_24k_open(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_ed25519_amd64_64_24k_open_batch(
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
