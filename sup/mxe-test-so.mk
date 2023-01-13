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
mxe_target = $(subst -mxe-test-so.mk,,$(target))
mxe_soname = libtau-$(Major_).$(Minor_)-$(mxe_target)-mxe.dll
mxe_sodir = $(builddir)/lib
mxe_so = $(mxe_sodir)/$(mxe_soname)
mxe_test_so_builddir = $(mxe_target)-mxe-test-so
MXE_CXX = $(mxe_prefix)/bin/$(mxe_target)-g++
MXE_STRIP = $(mxe_prefix)/bin/$(mxe_target)-strip

all_sources = $(basename $(notdir $(wildcard $(srcdir)/test/*.cc)))
all_binaries = $(addprefix $(bindir)/$(mxe_target)-, $(addsuffix .exe, $(all_sources)))
sources = $(basename $(notdir $(wildcard $(builddir)/test/*.cc)))
binaries = $(addprefix $(bindir)/$(mxe_target)-, $(addsuffix .exe, $(sources)))
CXXFLAGS += -O2 $(hh_option)

all: $(mxe_test_so_builddir) $(bindir) $(binaries)

install: $(bin_prefix) $(binaries)
	@for f in $(sources); do \
	    fname=$(mxe_target)-$$f.exe; \
	    cp -vfp $(bindir)/$$fname $(bin_prefix)/; \
	    $(MXE_STRIP) --strip-unneeded $$bin_prefix/$$fname; \
	done

uninstall:
	@for f in $(all_sources); do \
	    rm -vf $(bin_prefix)/$(mxe_target)-$$f.exe; \
	done

VPATH = $(srcdir)/test

$(bindir)/$(mxe_target)-%.exe: %.cc $(mxe_so)
	$(MXE_CXX) $(CXXFLAGS) -MD -MF $(mxe_test_so_builddir)/$(basename $(notdir $@)).dep -o $@ $< $(mxe_so) && $(link) $(mxe_so) $(bindir)/$(mxe_soname)

$(bindir):
	@mkdir -vp $@

$(bin_prefix):
	@mkdir -vp $@
	
$(mxe_test_so_builddir):
	@mkdir -vp $@

clean:
	@rm -vf $(all_binaries) $(mxe_test_so_builddir)/*.dep

rm: clean
	@rm -vrf $(mxe_test_so_builddir)

include $(wildcard $(mxe_test_so_builddir)/*.dep)

#END
