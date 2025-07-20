PHP_ARG_WITH([ed25519],
  [for Ed25519 support],
  [AS_HELP_STRING([--with-ed25519], [Include Ed25519 support])])

if test "$PHP_ED25519" != "no"; then
  # The symbol we check for must be the exact, namespaced symbol name
  # that exists in the library, as confirmed by the 'nm' command.
  AC_MSG_CHECKING([for crypto_sign_ed25519_amd64_64_24k_keypair in -lcrypto_sign_ed25519])

  # PHP_CHECK_LIBRARY will compile a small test program to see if it can link
  # against the library and find the specified function.
  PHP_CHECK_LIBRARY([crypto_sign_ed25519], [crypto_sign_ed25519_amd64_64_24k_keypair],
  [
    # Success!
    AC_MSG_RESULT([found])
    PHP_ADD_LIBRARY(crypto_sign_ed25519, 1, ED25519_SHARED_LIBADD)
    AC_DEFINE(HAVE_ED25519, 1, [Define if Ed25519 library is available])
  ],[
    # Failure!
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Cannot find libcrypto_sign_ed25519 with the required symbols. Check that it is installed and that CPPFLAGS and LDFLAGS are set correctly.])
  ])

  PHP_NEW_EXTENSION(ed25519, ed25519.c, $ext_shared)
fi
