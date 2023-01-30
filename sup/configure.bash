#! /bin/bash
# -----------------------------------------------------------------------------
# Copyright © 2014-2023 Konstantin Shmelkov <mrcashe@gmail.com>.
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

arg0='configure'
red_color=''
green_color=''
yellow_color=''
no_color=''
CHK_FILE=''

# Check program existence using 'which' command.
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
            echo "$arg0: existence of '${arr[0]}' is mandatory, configure failed, exitting with status 1" 1>&2
            printf "$no_color"
            exit 1
        else
            printf "$yellow_color NOT FOUND\n"
            echo "$arg0: existence of '${arr[0]}' is optional, corresponding feature will be disabled"
            printf "$no_color"
            eval which_$1=''
        fi
    else
        printf "$green_color $p$no_color\n"
        eval which_$1=$p
    fi
}

# Check file existence.
#   $1 is path to the file.
#   $2 is 'MANDATORY' for strict dependency and everything else for optional dependency.
chk_file() {
    printf "$arg0: checking for $(basename $1)... "

    if test -e $1; then
        printf "$green_color$1$no_color\n"
    elif [ 'MANDATORY' == $2 ]; then
        printf "$red_color NOT FOUND\n"
        echo "$arg0: existence of '$1' is mandatory, configure failed, exitting with status 1" 1>&2
        printf $no_color
        exit 1
    else
        printf "$yellow_color NOT FOUND\n"
        echo "$arg0: existence of '$1' is optional, corresponding feature will be disabled"
        printf $no_color
        CHK_FILE='YES'
    fi
}

# Outputs usage.
usage() {
    echo "Usage: configure [options...]"
    echo "Options are:"
    echo "  --help, -h               show this help"
    echo "  --prefix=<PREFIX>        set install prefix"
    echo "  --conf-targets=<TARGETS> set post-configure targets"
    echo "  --mxe-prefix=<PREFIX>    set MXE prefix"
    echo "  --mxe-target=<TARGET>    set MXE target"
    echo "  --disable-mxe            disable MXE building"
    echo "  --disable-doc            disable documentation generation"
    echo "  --disable-pdf            disable pdf documentation generation"
    echo "  --enable-static          enable static library building"
    echo "  --enable-test            enable test suite building"
    echo "  --enable-devel           enable development files creation and install"
}

# Start here.

opts=$@

for opt in $opts; do
    case "$opt" in
        --help | -h)
        usage
        exit 0
    ;;

    *)
    ;;
    esac
done

cwdir=$(pwd)
builddir="$cwdir/build"
bindir="$cwdir/bin"
confdir="$cwdir/conf"
chk_which 'basename' 'basename' 'MANDATORY'
arg0="$(basename $0)"
chk_which 'cat' 'cat' 'MANDATORY'
chk_which 'envsubst' 'envsubst' 'MANDATORY'
argv="$cwdir/configure.argv"

if [ -e $argv ]; then
    opts="$(cat $argv | envsubst) $opts"
    echo "$arg0: found $(basename $argv) file, using its content"
fi

PREFIX=''
CXXFLAGS='-std=c++14 -Wall'
disable_doc='NO'
disable_mxe='NO'
disable_pdf='NO'
enable_static='NO'
enable_test='NO'
enable_devel='NO'
mxe_prefix=''
mxe_target='i686-w64-mingw32.static'
conf_targets=''

chk_which 'sed' 'sed' 'MANDATORY'
chk_which 'dirname' 'dirname' 'MANDATORY'
chk_which 'realpath' 'realpath' 'MANDATORY'
topdir="$(realpath $(dirname $(dirname $0)))"
doxydir="$builddir/doxygen"
supdir="$topdir/sup"
srcdir="$topdir/src"
shdir="$topdir/share"
chk_which 'uname' 'uname' 'MANDATORY'
plat=$(uname)

echo "$arg0: configuring platform $plat..."
chk_which 'bash' 'bash' 'MANDATORY'
chk_which 'rm' 'rm' 'MANDATORY'
rm -vrf $confdir/*

link='cp -vf'
pkg_required=''
headers_required=''
libdata=''
plat_script="$supdir/$plat.bash"

if [ -x $plat_script ]; then
    echo "$arg0: found platform configation script '$plat_script', exec it..."
    . $plat_script
else
    echo "$arg0: FATAL: platform script '$plat_script' not found, exit with status 1" 1>&2
    exit 1
fi

for opt in $opts; do
    case "$opt" in
        --prefix=*)
            pfx=$(echo -n $opt |sed s+--prefix=++)
            [ -n "$pfx" ] && PREFIX=$pfx
        ;;

        --mxe-prefix=*)
            pfx=$(echo -n $opt |sed s+--mxe-prefix=++)
            [ -n "$pfx" ] && mxe_prefix=$pfx
        ;;

        --mxe-target=*)
            target=$(echo -n $opt |sed s/--mxe-target=//)
            [ -n "$target" ] && mxe_target=$target
        ;;

        --conf-targets=*)
            targets=$(echo -n $opt |sed s/--conf-targets=//)
            [ -n "$targets" ] && conf_targets+=" $targets"
        ;;

        --disable-doc)
            disable_doc='YES'
        ;;

        --disable-pdf)
            disable_pdf='YES'
        ;;

        --disable-mxe)
            disable_mxe='YES'
        ;;

        --enable-static)
            enable_static='YES'
        ;;

        --enable-test)
            enable_test='YES'
        ;;

        --enable-devel)
            enable_devel='YES'
        ;;

        *)
            usage >&2
            echo "$arg0: unrecognized parameter '$opt', exiting with status 1" 1>&2
            exit 1
        ;;
    esac
done

chk_which 'cp' 'cp' 'MANDATORY'
chk_which 'ln' 'ln' 'MANDATORY'
chk_which 'ls' 'ls' 'MANDATORY'
chk_which 'mkdir' 'mkdir' 'MANDATORY'
chk_which 'mktemp' 'mktemp' 'MANDATORY'
chk_which 'find' 'find' 'MANDATORY'
chk_which 'tr' 'tr' 'MANDATORY'
chk_which 'grep' 'grep' 'MANDATORY'
chk_which 'tar' 'tar' 'OPTIONAL'
chk_which 'xz' 'xz' 'OPTIONAL'
chk_which 'latex' 'latex' 'OPTIONAL'

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

echo "$arg0: install prefix is '$PREFIX'"
bin_prefix="$PREFIX/bin"
lib_prefix="$PREFIX/lib"
pc_prefix="$PREFIX/$libdata/pkgconfig"
hh_prefix="$PREFIX/include/tau-$Major_.$Minor_"
share_prefix="$PREFIX/share/tau-$Major_.$Minor_"

# ---------------------------------------------------------------------------
# Compilers, archivers and other code related tools checking.
# ---------------------------------------------------------------------------

chk_which 'cxx' 'c++ g++ clang++' 'MANDATORY'
target=$($which_cxx -dumpmachine)
chk_which 'ar' 'ar' 'MANDATORY'
chk_which 'strip' 'strip' 'MANDATORY'
chk_which 'pkgconfig' 'pkg-config' 'MANDATORY'
chk_which 'make' 'gmake' 'MANDATORY'
chk_which 'ctags' 'ctags' 'OPTIONAL'

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
# Headers searching.
# ---------------------------------------------------------------------------

if test -n "$headers_required"; then
    echo ""
    echo "$arg0: checking for header files existence..."

    for h in $headers_required; do
        echo -n "  searching for header file '$h'..."
        paths=$(find /usr -name "$h" 2>/dev/null)

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
fi

# ---------------------------------------------------------------------------
# MXE (optional) checking.
# ---------------------------------------------------------------------------

echo ""
mxe_syslibs=''

if [ 'YES' != "$disable_mxe" ]; then
    echo -n "$arg0: checking for optional M cross environment existence..."

    if test -z $mxe_prefix; then
        mxe_prefix=$(which "$mxe_target-g++")

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
else
    printf "$yellow_color$arg0: MXE building disabled by the user$no_color\n"
fi

if test -n "$mxe_prefix"; then
    CHK_FILE='NO'

    # Checking for MXE tools existence
    echo ""
    echo "$arg0: checking for MXE tools existence..."
    chk_file "$mxe_prefix/bin/$mxe_target-g++" 'OPTIONAL'
    chk_file "$mxe_prefix/bin/$mxe_target-ar" 'OPTIONAL'
    chk_file "$mxe_prefix/bin/$mxe_target-strip" 'OPTIONAL'

    # Checking for MXE libraries existence
    echo ""
    echo "$arg0: checking for MXE libraries existence..."

    if [ 'YES' != "$CHK_FILE" ]; then
        mxe_syslibs='libgdi32.a libmsimg32.a libole32.a libshlwapi.a'
        mxe_sysbase='libpng libz libpthread'
        mxe_libtype='.a'
        [ -n "$(echo -n $mxe_target | grep shared)" ] && mxe_libtype='.dll.a'
        for sb in $mxe_sysbase; do mxe_syslibs+=" $sb$mxe_libtype"; done

        for lib in $mxe_syslibs; do
            printf "  - "; chk_file "$mxe_prefix/$mxe_target/lib/$lib" 'OPTIONAL'
        done
    fi

    if [ 'YES' == "$CHK_FILE" ]; then
        mxe_prefix=''
        mxe_target=''
        mxe_syslibs=''
        printf "$yellow_color$arg0: building for MXE will not be performed$no_color\n"
    fi
fi

# ---------------------------------------------------------------------------
# Generating configuration.
# ---------------------------------------------------------------------------

echo ""
echo "$arg0: configuration OK, writing files..."
mkdir -vp $confdir

# ---------------------------------------------------------------------------
# Generating conf.mk
# ---------------------------------------------------------------------------

conf_mk="$confdir/conf.mk"
cp "$supdir/LICENSE.mkt" $conf_mk
echo "export SHELL = $which_bash" >>$conf_mk
echo "export plat = $plat" >>$conf_mk
echo "export target = $target" >>$conf_mk
echo "export topdir = $topdir" >>$conf_mk
echo "export builddir = $builddir" >>$conf_mk
echo "export doxydir = $doxydir" >>$conf_mk
echo "export confdir = $confdir" >>$conf_mk
echo "export supdir = $supdir" >>$conf_mk
echo "export bindir = $bindir" >>$conf_mk
echo "export srcdir = $srcdir" >>$conf_mk
echo "export Major_ = $Major_" >>$conf_mk
echo "export Minor_ = $Minor_" >>$conf_mk
echo "export Micro_ = $Micro_" >>$conf_mk
echo "export PREFIX = $PREFIX" >>$conf_mk
echo "export CXXFLAGS = $CXXFLAGS" >>$conf_mk
echo "export bin_prefix = $bin_prefix" >>$conf_mk
echo "export lib_prefix = $lib_prefix" >>$conf_mk
echo "export hh_prefix = $hh_prefix" >>$conf_mk
echo "export pc_prefix = $pc_prefix" >>$conf_mk
echo "export share_prefix = $share_prefix" >>$conf_mk
echo "export doc_prefix = $PREFIX/share/doc/tau-$Major_.$Minor_" >>$conf_mk
echo "export pkg_required = $pkg_required" >>$conf_mk
echo "export link = $link" >>$conf_mk
echo "export ln = $link" >>$conf_mk
echo "export cp = cp -vfp" >>$conf_mk
echo "export cpr = cp -vRfp" >>$conf_mk
echo "export rm = rm -vf" >>$conf_mk
echo "export rmr = rm -vrf" >>$conf_mk
echo "export mkdir = mkdir -vp" >>$conf_mk
echo "export doxygen = $which_doxygen" >>$conf_mk

[ 'YES' == "$enable_static" ] && conf_targets+=' en-a'
[ 'YES' == "$enable_test" ] && conf_targets+=' en-test'
[ -z "$conf_targets" ] && conf_targets='en-host-test-so'
[ 'YES' == "$enable_devel" ] && conf_targets+=' en-dev'
echo "conf_targets = $conf_targets" >>$conf_mk

if test -n "$mxe_prefix"; then
    echo "export mxe_prefix = $mxe_prefix" >>$conf_mk
    echo "export mxe_target = $mxe_target" >>$conf_mk
    echo "export mxe_syslibs = $mxe_syslibs" >>$conf_mk
fi

echo "" >>$conf_mk
echo "include $supdir/$plat.mk" >>$conf_mk
echo "include $supdir/dev.mk" >>$conf_mk
if test -z "$mxe_prefix"; then echo "include $supdir/mxe-empty.mk" >>$conf_mk
else echo "include $supdir/mxe.mk" >>$conf_mk; fi

# ---------------------------------------------------------------------------
# Documentation stuff.
# ---------------------------------------------------------------------------

if test -z $which_doxygen; then
    echo "include $supdir/doc-empty.mk" >>$conf_mk
else
    echo "include $supdir/doc.mk" >>$conf_mk
    tmp=$(mktemp)
    doxyinput="$topdir/README.md $srcdir/include $topdir/doc/doxygen"
    echo "s+PROJECT_NUMBER *=.*+PROJECT_NUMBER = $version+" >>$tmp
    echo "s+OUTPUT_DIRECTORY *=.*+OUTPUT_DIRECTORY = $doxydir+" >>$tmp
    echo "s+EXAMPLE_PATH *=.*+EXAMPLE_PATH = $topdir+" >>$tmp
    echo "s+INPUT *=.*+INPUT = $doxyinput+" >>$tmp
    echo "s+IMAGE_PATH *=.*+IMAGE_PATH = $shdir/pixmaps $shdir/icons/actions/12 $shdir/icons/actions/22 $shdir/icons/devices/22 $shdir/icons/places/22+" >>$tmp
    if [ 'YES' != "$disable_pdf" ] && [ -n "$which_latex" ]; then
        echo "s+GENERATE_LATEX *=.*+GENERATE_LATEX = YES+" >>$tmp
    fi
    sed -f "$tmp" "$topdir/doc/Doxyfile" >"$confdir/Doxyfile"
    rm -f $tmp
fi

# ---------------------------------------------------------------------------
# Generate cross-platform src/conf.cc file.
# ---------------------------------------------------------------------------

confcc="$confdir/conf.cc"
cp "$supdir/LICENSE.cct" $confcc
echo "#include <tau/sys.hh>" >>$confcc
echo "" >>$confcc
echo "namespace tau {" >>$confcc
echo "" >>$confcc
echo "const int Major_ = $Major_;" >>$confcc
echo "const int Minor_ = $Minor_;" >>$confcc
echo "const int Micro_ = $Micro_;" >>$confcc
echo "" >>$confcc
echo "} // namespace tau" >>$confcc
echo "" >>$confcc
echo "//END" >>$confcc

# ---------------------------------------------------------------------------
# Generate platform dependent src/${plat}/conf-${plat}.cc file.
# Sysinfo structure initialization.
# ---------------------------------------------------------------------------

mkdir -vp "$confdir/$plat"
confcc="$confdir/$plat/conf-$plat.cc"
cp "$supdir/LICENSE.cct" $confcc
echo "#include <tau/sysinfo.hh>"             >>$confcc
echo ""                                      >>$confcc
echo "namespace tau {"                       >>$confcc
echo ""                                      >>$confcc
echo "Sysinfo sysinfo_ = {"                  >>$confcc
echo "    .bytes    = 0,"                    >>$confcc
echo "    .Major    = $Major_,"              >>$confcc
echo "    .Minor    = $Minor_,"              >>$confcc
echo "    .Micro    = $Micro_,"              >>$confcc
echo "    .plat     = \"$plat\","            >>$confcc
echo "    .uname    = \"$plat\","            >>$confcc
echo "    .osmajor  = 0,"                    >>$confcc
echo "    .osminor  = 0,"                    >>$confcc
echo "    .distrib  = \"\","                 >>$confcc
echo "    .distrib_major = 0,"               >>$confcc
echo "    .distrib_minor = 0,"               >>$confcc
echo "    .distrib_codename = \"\","         >>$confcc
echo "    .distrib_description = \"\","      >>$confcc
echo "    .target    = \"$target\","         >>$confcc
echo "    .abits     = 0,"                   >>$confcc
echo "    .ibits     = 0,"                   >>$confcc
echo "    .lbits     = 0,"                   >>$confcc
echo "    .llbits    = 0,"                   >>$confcc
echo "    .mbits     = 0,"                   >>$confcc
echo "    .wcbits    = 0,"                   >>$confcc
echo "    .shared    = false,"               >>$confcc
echo "    .sopath    = \"\","                >>$confcc
echo "    .locale    = \"C\","               >>$confcc
echo "    .iocharset = \"\""                 >>$confcc
echo "};"                                    >>$confcc
echo ""                                      >>$confcc
echo "} // namespace tau"                    >>$confcc
echo ""                                      >>$confcc
echo "//END"                                 >>$confcc

# ---------------------------------------------------------------------------
# Generate platform dependent src/Windows/conf-Windows.cc file.
# Sysinfo structure initialization.
# ---------------------------------------------------------------------------

if test -n "$mxe_prefix"; then
    mkdir -vp "$confdir/Windows"
    confcc="$confdir/Windows/conf-Windows.cc"
    cp "$supdir/LICENSE.cct" $confcc
    echo "#include <tau/sysinfo.hh>"         >>$confcc
    echo ""                                  >>$confcc
    echo "namespace tau {"                   >>$confcc
    echo ""                                  >>$confcc
    echo "Sysinfo sysinfo_ = {"              >>$confcc
    echo "    .bytes     = 0,"               >>$confcc
    echo "    .Major     = $Major_,"         >>$confcc
    echo "    .Minor     = $Minor_,"         >>$confcc
    echo "    .Micro     = $Micro_,"         >>$confcc
    echo "    .plat      = \"Windows\","     >>$confcc
    echo "    .uname     = \"Windows\","     >>$confcc
    echo "    .osmajor   = 0,"               >>$confcc
    echo "    .osminor   = 0,"               >>$confcc
    echo "    .distrib   = \"\","            >>$confcc
    echo "    .distrib_major = 0,"           >>$confcc
    echo "    .distrib_minor = 0,"           >>$confcc
    echo "    .distrib_codename = \"\","     >>$confcc
    echo "    .distrib_description = \"\","  >>$confcc
    echo "    .target    = \"$mxe_target\"," >>$confcc
    echo "    .abits     = 0,"               >>$confcc
    echo "    .ibits     = 0,"               >>$confcc
    echo "    .lbits     = 0,"               >>$confcc
    echo "    .llbits    = 0,"               >>$confcc
    echo "    .mbits     = 0,"               >>$confcc
    echo "    .wcbits    = 0,"               >>$confcc
    echo "    .shared    = false,"           >>$confcc
    echo "    .sopath    = \"\","            >>$confcc
    echo "    .locale    = \"C\","           >>$confcc
    echo "    .iocharset = \"\""             >>$confcc
    echo "};"                                >>$confcc
    echo ""                                  >>$confcc
    echo "} // namespace tau"                >>$confcc
    echo ""                                  >>$confcc
    echo "//END"                             >>$confcc
fi

# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------

[ ! -e Makefile ] && $link "$topdir/Makefile" Makefile
exit 0
