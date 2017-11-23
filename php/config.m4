PHP_ARG_WITH(habib, for habib support,
Make sure that the comment is aligned:
[  --with-habib             Include habib support])
if test "$PHP_HABIB" != "no"; then

HABIB_DIR="/usr/local"

if test -z "$HABIB_DIR"; then
  AC_MSG_RESULT([not found])
  AC_MSG_ERROR([Please reinstall the habib distribution])
fi

# --with-habib -> add include path
PHP_ADD_INCLUDE($HABIB_DIR/include)

# --with-habib -> check for lib and symbol presence
LIBNAME=fox # you may want to change this
LIBSYMBOL=init_gc # you most likely want to change this 

PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
[
  PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $HABIB_DIR/lib, HABIB_SHARED_LIBADD)
  AC_DEFINE(HAVE_HABIBLIB,1,[ ])
],[
  AC_MSG_ERROR([wrong fox lib version or lib not found])
],[
  -L$HABIB_DIR/$PHP_LIBDIR -lm
])

PHP_SUBST(HABIB_SHARED_LIBADD)

  PHP_NEW_EXTENSION(habib, habib.c habibphp.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
