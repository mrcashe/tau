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

mxe_aname = libtau-$(Major_).$(Minor_)-mxe.a
mxe_adir = $(bindir)
mxe_a = $(mxe_adir)/$(mxe_aname)
mxe_test_a_builddir = mxe-test-a
all_sources = $(basename $(notdir $(wildcard $(srcdir)/test/*.cc)))
all_binaries = $(addprefix $(bindir)/, $(addsuffix .exe, $(all_sources)))
sources = $(basename $(notdir $(wildcard $(builddir)/test/*.cc)))
binaries = $(addprefix $(bindir)/, $(addsuffix .exe, $(sources)))
syslibs = $(addprefix $(mxe_prefix)/$(mxe_target)/lib/,$(mxe_syslibs))
CXXFLAGS += -O0 -I$(hh_src) -mwindows
MXE_CXX = $(mxe_prefix)/bin/$(mxe_target)-g++
MXE_STRIP = $(mxe_prefix)/bin/$(mxe_target)-strip
VPATH = $(srcdir)/test

all: $(mxe_test_a_builddir) $(bindir) $(binaries)

# Install binaries with long name including target platform sepcification.
install: $(bin_prefix) $(binaries)
	@for f in $(sources); do \
	    fname=$(mxe_target)-$$f.exe; \
	    $(cp) $(bindir)/$$f.exe $(bin_prefix)/$$fname; \
	    $(MXE_STRIP) --strip-unneeded $$bin_prefix/$$fname; \
	done

uninstall:
	@for f in $(all_sources); do \
	    $(rm) -vf $(bin_prefix)/$(mxe_target)-$$f.exe; \
	done


$(bindir)/%.exe: %.cc $(mxe_a)
	$(MXE_CXX) $(CXXFLAGS) -MD -MF $(mxe_test_a_builddir)/$(basename $(notdir $@)).dep -o $@ $< $(mxe_a) $(syslibs)

$(bindir):
	@$(mkdir) $@

$(bin_prefix):
	@$(mkdir) $@
	
$(mxe_test_a_builddir):
	@$(mkdir) $@

clean:
	@$(rm) $(all_binaries) $(mxe_test_a_builddir)/*.dep

rm: clean
	@$(rmr) $(mxe_test_a_builddir)

include $(wildcard $(mxe_test_a_builddir)/*.dep)

#END
