PHP_ARG_WITH([ed25519],
  [for Ed25519 support],
  [AS_HELP_STRING([--with-ed25519], [Include Ed25519 support])])

if test "$PHP_ED25519" != "no"; then
  AC_MSG_CHECKING([for crypto_sign_ed25519_amd64_64_24k_keypair in libcrypto_sign_ed25519])

  # Temporarily save the current linker/compiler flags
  LIBS_BAK="$LIBS"
  CPPFLAGS_BAK="$CPPFLAGS"

  # Explicitly add our library and include paths to the flags for the test.
  # LDFLAGS and CPPFLAGS are passed in from the build script's environment.
  LIBS="$LIBS $LDFLAGS -lcrypto_sign_ed25519"
  CPPFLAGS="$CPPFLAGS $CPPFLAGS"

  # AC_LINK_IFELSE: The most reliable way to check for a function.
  # It compiles and links a small test program.
  AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
      [#include "crypto_sign.h"],
      [crypto_sign_ed25519_amd64_64_24k_keypair(NULL, NULL);]
    )],
    [ # Success case
      AC_MSG_RESULT([found])
      PHP_ADD_LIBRARY(crypto_sign_ed25519, 1, ED25519_SHARED_LIBADD)
      AC_DEFINE(HAVE_ED25519, 1, [Define if Ed25519 library is available])
    ],
    [ # Failure case
      AC_MSG_RESULT([not found])
      AC_MSG_ERROR([Cannot link against libcrypto_sign_ed25519. Check config.log for details, but ensure the library was built correctly and CPPFLAGS/LDFLAGS point to the output directory.])
    ]
  )

  # Restore the original flags
  LIBS="$LIBS_BAK"
  CPPFLAGS="$CPPFLAGS_BAK"

  PHP_NEW_EXTENSION(ed25519, ed25519.c, $ext_shared)
fi
