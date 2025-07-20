PHP_ARG_WITH([ed25519],
  [for Ed25519 support],
  [AS_HELP_STRING([--with-ed25519], [Include Ed25519 support])])

if test "$PHP_ED25519" != "no"; then
  AC_MSG_CHECKING([for crypto_sign_keypair in -lcrypto_sign_ed25519])

  # Check for the library and a required function.
  # Search paths are provided by the LDFLAGS and CPPFLAGS environment variables.
  PHP_CHECK_LIBRARY([crypto_sign_ed25519], [crypto_sign_keypair],
  [
    # Success! Add the library to the linker flags and define a preprocessor macro.
    PHP_ADD_LIBRARY(crypto_sign_ed25519, 1, ED25519_SHARED_LIBADD)
    AC_DEFINE(HAVE_ED25519, 1, [Define if Ed25519 library is available])
    AC_MSG_RESULT([found])
  ],[
    # Failure!
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Cannot find libcrypto_sign_ed25519. Check that it is installed and CPPFLAGS/LDFLAGS are set.])
  ])

  PHP_NEW_EXTENSION(ed25519, ed25519.c, $ext_shared)
fi
