#! /bin/bash
# -----------------------------------------------------------------------------
# Copyright © 2014-2022 Konstantin Shmelkov <mrcashe@gmail.com>.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and#or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# -----------------------------------------------------------------------------

arg0="$(basename $0)"
red_color=''
green_color=''
yellow_color=''
no_color=''

# Check program existance using 'which' command.
# Here is:
#   $1 is for variable name, which has form of 'which_$1', for example, 'which_pkgconfig'.
#   $2 is for searching command name (or names), for example, 'pkg-config'.
#   $3 is 'MANDATORY' for strict dependency and everything else for optional dependency.
chk_which() {
    arr=($2)
    printf "$arg0: checking for ${arr[0]}..."
    p=''

    for cmd in $2; do
        p=$(which $cmd 2>/dev/null)
        [ -n $p ] && break;
    done;

    if test -z $p; then
        if [ 'MANDATORY' == $3 ]; then
            printf "$red_color NOT FOUND\n"
            echo "$arg0: existance of '${arr[0]}' is mandatory, configure failed, exitting with status 1" 1>&2
            printf $no_color
            exit 1
        else
            printf "$yellow_color NOT FOUND\n"
            echo "$arg0: existance of '${arr[0]}' is optional, corresponding feature will be disabled"
            printf $no_color
            eval which_$1=''
        fi
    else
        printf "$green_color $p$no_color\n"
        eval which_$1=$p
    fi
}

topdir="$(realpath $(dirname $(dirname $0)))"
echo $topdir
cwdir=$(pwd)
plat=$(uname)
PREFIX=''
link="ln -vsf"
pkg_required=''
headers_required=''

builddir="$cwdir/build"
doxydir="$builddir/doxygen"
bindir="$cwdir/bin"
confdir="$cwdir/conf"
supdir="$topdir/sup"
srcdir="$topdir/src"
shdir="$topdir/share"

CXXFLAGS="-std=c++14"

disable_doc='NO'
disable_mxe='NO'
enable_static='NO'
enable_test='NO'

mxe_prefix=''
mxe_target='i686-w64-mingw32.static'

echo "$arg0: configuring platform $plat..."
which which >/dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "$arg0: 'which' not found, configure failed, exitting with status 1" 1>&2
    exit 1
fi

chk_which 'bash' 'bash' 'MANDATORY'
chk_which 'rm' 'rm' 'MANDATORY'
rm -vf $confdir/*
plat_script="$supdir/$plat.bash"

if [ -x $plat_script ]; then
    echo "$arg0: found platform configation script '$plat_script', exec it..."
    . $plat_script
else
    echo "$arg0: warning: platform script '$plat_script' not found, configure process may fail"
fi

chk_which 'cp' 'cp' 'MANDATORY'
chk_which 'ln' 'ln' 'MANDATORY'
chk_which 'ls' 'ls' 'MANDATORY'
chk_which 'mkdir' 'mkdir' 'MANDATORY'
chk_which 'mktemp' 'mktemp' 'MANDATORY'
chk_which 'sed' 'sed' 'MANDATORY'
chk_which 'find' 'find' 'MANDATORY'
chk_which 'cat' 'cat' 'MANDATORY'
chk_which 'envsubst' 'envsubst' 'MANDATORY'
chk_which 'tr' 'tr' 'MANDATORY'
chk_which 'cmp' 'cmp' 'MANDATORY'
chk_which 'basename' 'basename' 'MANDATORY'
chk_which 'dirname' 'dirname' 'MANDATORY'
chk_which 'tar' 'tar' 'MANDATORY'

verfile="$topdir/VERSION"

if [ ! -e "$verfile" ]; then
    echo "$arg0: version file '$verfile' not found, exitting with status 1" 1>&2
    exit 1
fi

ver=($(cat $verfile | tr "." "\n"))

if [ ${#ver[@]} -ne 3 ]; then
    echo "$arg0: version file '$verfile' has incorrect format, exitting with status 1" 1>&2
    exit 1
fi

Major_=${ver[0]}
Minor_=${ver[1]}
Micro_=${ver[2]}
version="$Major_.$Minor_.$Micro_"
echo "$arg0: API version is '$version'"

opts=$@
argv="$cwdir/configure.argv"

if [ -e $argv ]; then
    opts="$(cat $argv | envsubst) $opts"
    echo "$arg0: found $(basename $argv) file, using its content"
fi

usage() {
    echo "Usage: configure [options...]"
    echo "Options are:"
    echo "  --help, -h              show this help"
    echo "  --prefix=<PREFIX>       set install prefix"
    echo "  --mxe-prefix=<PREFIX>   set MXE prefix"
    echo "  --mxe-target=<TARGET>   set MXE target"
    echo "  --disable-mxe           disable MXE building"
    echo "  --disable-doc           disable documentation generation"
    echo "  --enable-static         enable static library building"
    echo "  --enable-test           enable test suite building"
}

for opt in $opts; do
    case "$opt" in
        --help | -h)
            usage && exit 0
        ;;

        --prefix=*)
            pfx=$(echo -n $opt |sed s+--prefix=++)

            if test -z "$pfx"; then
                echo "$arg0: got an empty prefix, use default one, which is '$PREFIX'"
            else
                PREFIX=$pfx
                echo "$arg0: prefix set to '$PREFIX'"
            fi
        ;;

        --mxe-prefix=*)
            pfx=$(echo -n $opt |sed s+--mxe-prefix=++)

            if test -z "$pfx"; then
                echo -n "$arg0: ignore empty MXE prefix"

                if test -z "$mxe_prefix"; then
                    echo ", will autodetect MXE cross plat environment existance"
                else
                    echo ", will use current value, which is $mxe_prefix"
                fi

            else
                mxe_prefix=$pfx
                echo "$arg0: MXE prefix set to '$mxe_prefix'"
            fi
        ;;

        --mxe-target=*)
            target=$(echo -n $opt |sed s/--mxe-target=//)

            if test -z "$target"; then
                echo "$arg0: ignore empty MXE target, will use current value, which is $mxe_target"
            else
                mxe_target=$target
                echo "$arg0: MXE target set to '$mxe_target'"
            fi
        ;;

        --disable-doc)
            echo "$arg0: documentation generation disabled by the user"
            disable_doc='YES'
        ;;

        --disable-mxe)
            echo "$arg0: MXE building disabled by the user"
            disable_mxe='YES'
        ;;

        --enable-static)
            echo "$arg0: static library building enabled"
            enable_static='YES'
        ;;

        --enable-test)
            echo "$arg0: test suite building enabled"
            enable_test='YES'
        ;;

        *)
            echo "$arg0: unrecognized parameter '$opt', exiting with status of 1" 1>&2
            usage 1>&2
            exit 1
        ;;
    esac
done

echo "$arg0: install prefix is '$PREFIX'"

# ---------------------------------------------------------------------------
# Compilers, archivers and other binutils checking.
# ---------------------------------------------------------------------------

chk_which 'cxx' 'c++ g++ clang++' 'MANDATORY'
chk_which 'ar' 'ar' 'MANDATORY'
chk_which 'pkgconfig' 'pkg-config' 'MANDATORY'
chk_which 'make' 'gmake' 'MANDATORY'

# ---------------------------------------------------------------------------
# Packages.
# ---------------------------------------------------------------------------

echo ""
echo "$arg0: checking package requirements..."
tmp=$(mktemp)
for pkg in $pkg_required; do
    echo -n "  - $pkg..."
    $which_pkgconfig --exists --print-errors "$pkg" 2>$tmp
    if [ 0 -ne $? ]; then
        printf "$red_color FAILED\n"
        cat "$tmp" 1>&2
        echo "$arg0: Package requirements check failed, exitting with status 1" 1>&2
        rm -f $tmp
        printf $no_color
        exit 1
    else
        printf "$green_color OK, version is $($which_pkgconfig --modversion $pkg)$no_color\n"
    fi
done
rm -f $tmp

# ---------------------------------------------------------------------------
# Headers searching.
# ---------------------------------------------------------------------------

echo ""
echo "$arg0: checking for header files existance..."

for h in $headers_required; do
    echo -n "  searching for header file '$h'..."
    paths=`find /usr -name "$h" 2>/dev/null`

    if test -z "$paths"; then
        printf "$red_color NOT FOUND\n"
        echo "  $arg0: header file '$h' not found, exitting with status 1" 1>&2
        printf $no_color
        exit 1
    fi

    printf "$green_color OK$no_color\n"
    echo "    header file '$h' found within the following directories:"
    for d in $paths; do echo "      - $d"; done
done

# ---------------------------------------------------------------------------
# Doxygen (optional) checking.
# ---------------------------------------------------------------------------

echo ''
which_doxygen=''

if [ 'YES' != "$disable_doc" ]; then
    chk_which 'doxygen' 'doxygen' 'OPTIONAL'
else
    printf "$yellow_color$arg0: documentation generation disabled by user$no_color\n"
fi

# ---------------------------------------------------------------------------
# MXE (optional) checking.
# ---------------------------------------------------------------------------

echo ""
mxe_syslibs=''

if [ 'YES' != "$disable_mxe" ]; then
    echo -n "$arg0: checking for optional M cross environment existance..."

    if test -z $mxe_prefix; then
        mxe_prefix=$(which $mxe_target-g++ 2>/dev/null)

        if test -z "$mxe_prefix"; then
            printf "$yellow_color NOT FOUND\n"
            printf "$arg0: building for MXE will not be performed$no_color\n"
        else
            printf "$green_color OK$no_color\n"
            mxe_prefix=$(dirname $(dirname $mxe_prefix))
            printf "$arg0: mxe_prefix=$green_color$mxe_prefix$no_color\n"
            printf "$arg0: mxe_target=$green_color$mxe_target$no_color\n"
        fi
    else
        echo ""
        echo -n "$arg0: have user defined MXE prefix: <$mxe_prefix>, checking it..."

        if [ ! -e "$mxe_prefix/bin/$mxe_target-g++" ]; then
            printf "$yellow_color NOT FOUND\n"
            printf "$arg0: M cross environment not found, building will be disabled$no_color\n"
            mxe_prefix=""
            mxe_target=""
        else
            printf "$green_color OK$no_color\n"
            printf "$arg0: mxe_prefix=$green_color$mxe_prefix$no_color\n"
            printf "$arg0: mxe_target=$green_color$mxe_target$no_color\n"
        fi
    fi

    # Checking for MXE libraries existance
    echo ""
    echo "$arg0: checking for MXE libraries existance..."
    mxe_syslibs='libpng.a libz.a libgdi32.a libmsimg32.a libpthread.a libintl.a libiconv.a libole32.a libshlwapi.a'
    mxe_libs_failed='no'

    for lib in $mxe_syslibs; do
        printf "  - $lib: "
        paths=$(find $mxe_prefix/$mxe_target/lib -name "$lib" 2>/dev/null)

        if test -z $paths; then
            printf "$yellow_color NOT FOUND\n"
            printf "$arg0: library file $lib not found, MXE building will be disabled$no_color\n"
            mxe_libs_failed='yes'
            break
        else
            printf "$green_color OK$no_color\n"
        fi
    done

    if [ "$mxe_libs_failed" == "yes" ]; then
        mxe_prefix=''
        mxe_target=''
        mxe_syslibs=''
    fi
else
    printf "$yellow_color$arg0: MXE building disabled by the user$no_color\n"
fi

# ---------------------------------------------------------------------------
# Generating configuration.
# ---------------------------------------------------------------------------

echo ""
echo "$arg0: configuration OK, writing files..."
mkdir -vp $confdir
lib_prefix="$PREFIX/lib"
pc_prefix="$lib_prefix/pkgconfig"
hh_prefix="$PREFIX/include/tau-$Major_.$Minor_"
share_prefix="$PREFIX/share/tau-$Major_.$Minor_"

# ---------------------------------------------------------------------------
# Generating conf.mk
# ---------------------------------------------------------------------------

conf_mk="$confdir/conf.mk"
cp "$supdir/LICENSE.mkt" $conf_mk
echo "export topdir = $topdir" >>$conf_mk
echo "export builddir = $builddir" >>$conf_mk
echo "export doxydir = $doxydir" >>$conf_mk
echo "export confdir = $confdir" >>$conf_mk
echo "export supdir = $supdir" >>$conf_mk
echo "export bindir = $bindir" >>$conf_mk
echo "export srcdir = $srcdir" >>$conf_mk
echo "export Major_ = $Major_" >>$conf_mk
echo "export Minor_ = $Minor_" >>$conf_mk
echo "export Micro_ = $Major_" >>$conf_mk
echo "export PREFIX = $PREFIX" >>$conf_mk
echo "export CXXFLAGS = $CXXFLAGS" >>$conf_mk
echo "export lib_prefix = $lib_prefix" >>$conf_mk
echo "export hh_prefix = $hh_prefix" >>$conf_mk
echo "export pc_prefix = $pc_prefix" >>$conf_mk
echo "export share_prefix = $share_prefix" >>$conf_mk
echo "export pkg_required = $pkg_required" >>$conf_mk
echo "export link = $link" >>$conf_mk

if test -n $mxe_prefix; then
    echo "export mxe_prefix = $mxe_prefix" >>$conf_mk
    echo "export mxe_target = $mxe_target" >>$conf_mk
    echo "export mxe_syslibs = $mxe_syslibs" >>$conf_mk
fi

echo "" >>$conf_mk
echo "include $supdir/$plat.mk" >>$conf_mk
if test -z $mxe_prefix; then echo "include $supdir/mxe-empty.mk" >>$conf_mk
else echo "include $supdir/mxe.mk" >>$conf_mk; fi

# ---------------------------------------------------------------------------
# Documentation stuff.
# ---------------------------------------------------------------------------

if test -z $which_doxygen; then
    echo "include $supdir/doc-empty.mk" >>$conf_mk
else
    echo "include $supdir/doc.mk" >>$conf_mk
    tmp=$(mktemp)
    echo "s+PROJECT_NUMBER *=.*+PROJECT_NUMBER = $version+" >>$tmp
    echo "s+OUTPUT_DIRECTORY *=.*+OUTPUT_DIRECTORY = $doxydir+" >>$tmp
    echo "s+EXAMPLE_PATH *=.*+EXAMPLE_PATH = $topdir+" >>$tmp
    echo "s+INPUT *=.*+INPUT = $srcdir/include $srcdir/doxygen+" >>$tmp
    echo "s+IMAGE_PATH *=.*+IMAGE_PATH = $shdir/pixmaps $shdir/icons/actions/12 $shdir/icons/actions/22 $shdir/icons/devices/22 $shdir/icons/places/22+" >>$tmp
    sed -f "$tmp" "$topdir/doc/Doxyfile" >"$confdir/Doxyfile"
    rm -f $tmp
fi

# ---------------------------------------------------------------------------
# Generate src/conf.cc file.
# ---------------------------------------------------------------------------

tmp=$(mktemp)
cp "$supdir/LICENSE.cct" $tmp
echo "#include <tau/apiver.hh>">>$tmp
echo "">>$tmp
echo "namespace tau {">>$tmp
echo "">>$tmp
echo "const int Major_ = $Major_;">>$tmp
echo "const int Minor_ = $Minor_;">>$tmp
echo "const int Micro_ = $Micro_;">>$tmp
echo "">>$tmp
echo "} // namespace tau">>$tmp
echo "">>$tmp
echo "//END">>$tmp

confcc="$confdir/conf.cc"
cmp $tmp $confcc >/dev/null 2>&1 || cp -vf $tmp $confcc
rm -f $tmp

# ---------------------------------------------------------------------------
# Create build directory if does not exists and Makefiles.
# ---------------------------------------------------------------------------

[ ! -e Makefile ] && $link "$topdir/Makefile" Makefile
mkdir -vp $builddir
[ ! -e "$builddir/Makefile" ] && $link "$supdir/build.mk" "$builddir/Makefile"

if [ -z "$(ls $builddir/*.mk 2>/dev/null)" ]; then
    $which_make en-host-so
    [ 'YES' == "$enable_static" ] && $which_make en-a
    [ 'YES' == "$enable_test" ] && $which_make en-test
fi
