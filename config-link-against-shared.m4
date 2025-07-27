dnl ======================================================================
dnl config.m4 for the ed25519 PHP extension.
dnl This file enables support for the Ed25519 library.
dnl ======================================================================

PHP_ARG_WITH(ed25519, for Ed25519 support,
[  --with-ed25519[=DIR]  Include Ed25519 library support])

if test "$PHP_ED25519" != "no"; then
    dnl Set the base directory - either from user input or use our build output
    if test "$PHP_ED25519" = "yes"; then
        ED25519_DIR="/usr/src/build/output"
    else
        ED25519_DIR="$PHP_ED25519"
    fi

    dnl Add the new extension
    PHP_NEW_EXTENSION(ed25519, ed25519.c, $ext_shared)

    dnl Add the include directory
    PHP_ADD_INCLUDE([$ED25519_DIR/include])

    dnl Add the main Ed25519 library (using the static library for bundling)
    PHP_ADD_LIBRARY_WITH_PATH(crypto_sign_ed25519, $ED25519_DIR/lib, ED25519_SHARED_LIBADD)
    PHP_SUBST(ED25519_SHARED_LIBADD)

    dnl Debug output
    AC_MSG_NOTICE([Using ED25519_DIR = $ED25519_DIR])
    AC_MSG_NOTICE([Expecting header files in: $ED25519_DIR/include])
    AC_MSG_NOTICE([Expecting library file: $ED25519_DIR/lib/libcrypto_sign_ed25519.a])

    dnl Check that the Ed25519 library contains the required symbol
    AC_MSG_CHECKING([for crypto_sign_ed25519_amd64_64_24k_keypair in libcrypto_sign_ed25519])

    dnl Save current flags
    SAVE_LIBS="$LIBS"
    SAVE_CPPFLAGS="$CPPFLAGS"

    dnl Set up the test environment
    CPPFLAGS="$CPPFLAGS -I$ED25519_DIR/include"
    LIBS="-L$ED25519_DIR/lib -lcrypto_sign_ed25519 $LIBS"

    dnl Try to compile and link a test program
    AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [#include <stddef.h>
            #include "crypto_sign.h"],
           [crypto_sign_ed25519_amd64_64_24k_keypair(NULL, NULL); return 0;]
        )],
        [AC_MSG_RESULT([yes])],
        [AC_MSG_ERROR([Cannot find libcrypto_sign_ed25519.a with symbol crypto_sign_ed25519_amd64_64_24k_keypair.
Please check:
  1. That libcrypto_sign_ed25519.a exists in $ED25519_DIR/lib
  2. That the header "crypto_sign.h" is in $ED25519_DIR/include
  3. That the library was built with -fPIC
See config.log for compilation details.])]
    )

    dnl Restore flags
    CPPFLAGS="$SAVE_CPPFLAGS"
    LIBS="$SAVE_LIBS"
fi