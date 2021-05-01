AC_PREREQ([2.67])
AC_INIT([lightsoff], [1.2], [https://github.com/jburguete/lightsoff])

# Checks for operative systems
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE
AC_MSG_NOTICE([Build: $build])
AC_MSG_NOTICE([Host: $host])
win=0
if test $host = "x86_64-w64-mingw32"; then
	AC_MSG_NOTICE([Microsoft Windows 64 bits])
	AC_SUBST(PREFIX, "/mingw64")
	AC_SUBST(ICON, "logo.o")
	AC_SUBST(WINCFLAGS, "-mwindows")
	AC_SUBST(WIN,"win64")
	win=1
elif test $host = "i686-w64-mingw32"; then
	AC_MSG_NOTICE([Microsoft Windows 32 bits])
	AC_SUBST(PREFIX, "/mingw32")
	AC_SUBST(ICON, "logo.o")
	AC_SUBST(WINCFLAGS, "-mwindows")
	AC_SUBST(WIN,"win32")
	win=1
elif test `uname -s` = "NetBSD"; then
	AC_MSG_NOTICE([NetBSD])
	AC_SUBST(PREFIX,"/usr/pkg")
elif test `uname -s` = "OpenBSD"; then
	AC_MSG_NOTICE([OpenBSD])
	AC_SUBST(PREFIX,"/usr/local")
	openbsd=1
elif test `uname -s` = "DragonFly"; then
	AC_MSG_NOTICE([DragonFly BSD])
	AC_SUBST(PREFIX,"/usr/local")
else
	AC_SUBST(PREFIX, "/usr")
fi
AC_MSG_NOTICE([prefix=$PREFIX])

# Checks for programs
AC_PROG_CC([egcc gcc clang])
AC_MSG_NOTICE([Compiler: $CC])
AC_PROG_MAKE_SET
AC_LANG([C])
PKG_PROG_PKG_CONFIG
if test $win = 1; then
	AC_CHECK_TOOL(WINDRES, windres)
	AC_SUBST(EXE, ".exe")
fi

# Checks for libraries
PKG_CHECK_MODULES([GLIB], [glib-2.0])
AC_ARG_WITH([gtk],
	AS_HELP_STRING([--with-gtk],
		[Build using a version of GTK (3 (--with-gtk=3 or 
		 4 (--with-gtk=4 default))]))
if test "x$with_gtk" = "x3"; then
	gtk4=0
else
	PKG_CHECK_MODULES([GTK], [gtk4], [gtk4=1], [gtk4=0])
fi
if test $gtk4 = 1; then
	AC_SUBST(GTK4, "-DGTK4=1")
else
	AC_SUBST(GTK4, "-DGTK4=0")
	PKG_CHECK_MODULES([GTK], [gtk+-3.0])
fi

# Checks for header files
AC_CHECK_HEADERS([stdio.h stdlib.h stdint.h string.h libintl.h])

# Checking -march=native compiler flag
AC_ARG_WITH([native],
	AS_HELP_STRING([--with-native],
		[Build using -march=native compiler flag]))
if test "x$with_native" = "xyes"; then
	if test "x$native" = "x1"; then
		compiler=$CC
		CC="$CC -march=native"
		AC_TRY_LINK([],
			[int main(int argc, char **argv){return 0;}], [arch=1],
			[arch=0])
		CC=$compiler
		if test $arch = 0; then
			AC_MSG_NOTICE([checking for -march=native... no])
			AC_SUBST(ARCH, "")
		else
			AC_MSG_NOTICE([checking for -march=native... yes])
			AC_SUBST(ARCH, "-march=native")
		fi
	fi
fi

# Checking -flto compiler flag
compiler=$CC
CC="$CC -flto"
AC_TRY_LINK([], [int main(int argc, char **argv){return 0;}], [lto=1], [lto=0])
CC=$compiler
if test $lto = 0; then
	AC_MSG_NOTICE([checking for -flto... no])
	AC_SUBST(LTO, "")
else
	AC_MSG_NOTICE([checking for -flto... yes])
	AC_SUBST(LTO, "-flto")
fi

# Checking -std compiler flag
compiler=$CC
CC="$CC -std=gnu11"
AC_TRY_LINK([], [int main(int argc, char **argv){return 0;}], [std=1], [std=0])
CC=$compiler
if test $std = 0; then
	AC_MSG_NOTICE("checking for -std=gnu11... no")
	CC="$CC -std=gnu99"
	AC_TRY_LINK([], [int main(int argc, char **argv){return 0;}], [std=1],
		[std=0])
	CC=$compiler
	if test $std = 0; then
		AC_MSG_NOTICE("checking for -std=gnu99... no")
		AC_SUBST(STD, "")
	else
		AC_MSG_NOTICE("checking for -std=gnu99... yes")
		AC_SUBST(STD, "-std=gnu99")
	fi
else
	AC_MSG_NOTICE("checking for -std=gnu11... yes")
	AC_SUBST(STD, "-std=gnu11")
fi

# Checks for library functions
AC_FUNC_MALLOC
AC_CHECK_FUNCS([])

# Final output
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
