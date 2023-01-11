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
mxe_target = $(subst -mxe-test.mk,,$(target))
mxe_aname = libtau-$(Major_).$(Minor_)-$(mxe_target)-mxe.a
mxe_adir = $(builddir)/lib
mxe_a = $(mxe_adir)/$(mxe_aname)
mxe_test_builddir = $(mxe_target)-mxe-test
mxe_libroot = $(mxe_prefix)/$(mxe_target)/lib
MXE_CXX = $(mxe_prefix)/bin/$(mxe_target)-g++

all_sources = $(basename $(notdir $(wildcard $(srcdir)/test/*.cc)))
all_binaries = $(addprefix $(bindir)/, $(addsuffix .exe, $(all_sources)))
sources = $(basename $(notdir $(wildcard $(builddir)/test/*.cc)))
binaries = $(addprefix $(bindir)/, $(addsuffix .exe, $(sources)))
CXXFLAGS += -O0 $(hh_option)
mxe_sys_static = $(addprefix $(mxe_libroot)/,$(mxe_syslibs))

all: $(mxe_test_builddir) $(bindir) $(binaries)
install:
uninstall:

VPATH = $(srcdir)/test

$(bindir)/%.exe: %.cc $(mxe_a)
	$(MXE_CXX) $(CXXFLAGS) -MD -MF $(mxe_test_builddir)/$(basename $(notdir $@)).dep -o $@ $< $(mxe_a) $(mxe_sys_static)

$(bindir):
	@mkdir -vp $@

$(mxe_test_builddir):
	@mkdir -vp $@

clean:
	rm -vf $(all_binaries) $(mxe_test_builddir)/*.dep

rm: clean
	rm -vrf $(mxe_test_builddir)

include $(wildcard $(mxe_test_builddir)/*.dep)

#END
