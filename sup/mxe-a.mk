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

target = $(shell echo $(MAKEFILE) | sed s/[0-9][0-9]-//)
mxe_target = $(subst -mxe-a.mk,,$(target))
mxe_aname = libtau-$(Major_).$(Minor_)-$(mxe_target)-mxe.a
mxe_adir = $(builddir)/lib
mxe_a = $(mxe_adir)/$(mxe_aname)
mxe_a_builddir = $(mxe_target)-mxe-a
mxe_libroot = $(mxe_prefix)/$(mxe_target)/lib
mxe_a_dest = $(lib_prefix)/$(mxe_aname)
mxe_pc = $(pc_prefix)/tau-$(Major_).$(Minor_)-$(mxe_target)-mxe.pc

MXE_CXX = $(mxe_prefix)/bin/$(mxe_target)-g++
MXE_AR = $(mxe_prefix)/bin/$(mxe_target)-ar
MXE_STRIP = $(mxe_prefix)/bin/$(mxe_target)-strip

cplat_srcdir = $(srcdir)
win_srcdir = $(srcdir)/Windows
src_dirs = $(cplat_srcdir) $(win_srcdir) $(confdir)
VPATH = $(src_dirs)
sources = $(foreach dir, $(src_dirs), $(wildcard $(dir)/*.cc))
objects = $(addprefix $(mxe_a_builddir)/, $(sort $(addsuffix .o, $(basename $(notdir $(sources))))))
CXXFLAGS += -O2 -Wall $(hh_impl_options)

all: $(mxe_adir) $(mxe_a_builddir) $(objects)
install: install-a install-pc
uninstall: uninstall-pc uninstall-a

clean:
	@rm -vf $(mxe_a_builddir)/*.o $(mxe_a_builddir)/*.dep $(mxe_a)

rm: clean
	@rm -vrf $(mxe_a_builddir)

$(mxe_a_builddir)/%.o: %.cc
	$(MXE_CXX) $(CXXFLAGS) -c -MD -MF $(basename $@).dep -o $@ $< && $(MXE_AR) rs $(mxe_a) $@

include $(wildcard $(mxe_a_builddir)/*.dep)

.PHONY: install-a install-pc uninstall-a uninstall-pc

install-a: $(lib_prefix)
	@cp -vfp $(mxe_a) $(mxe_a_dest)
	@$(MXE_STRIP) --strip-unneeded $(mxe_a_dest)

install-pc: $(pc_prefix)
	@if [ -f $(mxe_a) ]; then \
	    echo "prefix=$(PREFIX)" >$(mxe_pc); \
	    echo "exec_prefix=$(PREFIX)" >>$(mxe_pc); \
	    echo "lib_prefix=$(PREFIX)/lib">>$(mxe_pc); \
	    echo "includedir=$(hh_prefix)">>$(mxe_pc); \
	    echo "a=$(mxe_a_dest)">>$(mxe_pc); \
	    echo "mxe_prefix=$(mxe_prefix)">>$(mxe_pc); \
	    echo "mxe_target=$(mxe_target)">>$(mxe_pc); \
	    echo "mxe_libroot=$(mxe_libroot)">>$(mxe_pc); \
	    echo "mxe_syslibs=$(mxe_syslibs)">>$(mxe_pc); \
	    echo "Name: libtau">>$(mxe_pc); \
	    echo "Description: C++ GUI toolkit for MinGW cross compile">>$(mxe_pc); \
	    echo "Version: $(Major_).$(Minor_).$(Micro_)">>$(mxe_pc); \
	    echo -n "Libs: -L$$">>$(mxe_pc); \
	    echo "{lib_prefix}">>$(mxe_pc); \
	    echo -n "Cflags: -I$$">>$(mxe_pc); \
	    echo "{includedir}">>$(mxe_pc); \
	fi

uninstall-a:
	@rm -vf $(mxe_a_dest)

uninstall-pc:
	@rm -vf $(mxe_pc)

$(mxe_adir):
	@mkdir -vp $@

$(lib_prefix):
	@mkdir -vp $@

$(mxe_a_builddir):
	@mkdir -vp $@

$(pc_prefix):
	@mkdir -vp $@

#END
