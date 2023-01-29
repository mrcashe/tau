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

mxe_sofile = libtau-$(Major_).$(Minor_)-$(mxe_target)-mxe.dll
mxe_sodir = $(bindir)
mxe_so = $(mxe_sodir)/$(mxe_sofile)
mxe_test_so_builddir = mxe-test-so
all_sources = $(basename $(notdir $(wildcard $(srcdir)/test/*.cc)))
all_binaries = $(addprefix $(bindir)/, $(addsuffix .exe, $(all_sources)))
sources = $(basename $(notdir $(wildcard $(builddir)/test/*.cc)))
binaries = $(addprefix $(bindir)/, $(addsuffix .exe, $(sources)))
CXXFLAGS += -O2 -I$(hh_src) -mwindows
MXE_CXX = $(mxe_prefix)/bin/$(mxe_target)-g++
MXE_STRIP = $(mxe_prefix)/bin/$(mxe_target)-strip
VPATH = $(srcdir)/test

all: $(mxe_test_so_builddir) $(bindir) $(binaries)

# Install binaries with long name including target platform sepcification.
install: $(bin_prefix) $(binaries)
	@for f in $(sources); do \
	    fname=$$f-$(mxe_target)-mxe.exe; \
	    $(cp) "$(bindir)/$$f.exe" $(bin_prefix)/$$fname; \
	    $(MXE_STRIP) --strip-unneeded $$bin_prefix/$$fname; \
	done

uninstall:
	@for f in $(all_sources); do \
	    $(rmr) $(bin_prefix)/$$f-$(mxe_target)-mxe.exe; \
	done


$(bindir)/%.exe: %.cc
	$(MXE_CXX) $(CXXFLAGS) -MD -MF $(mxe_test_so_builddir)/$(basename $(notdir $@)).dep -o $@ $< $(mxe_so)

$(bindir):
	@$(mkdir) -vp $@

$(bin_prefix):
	@$(mkdir) -vp $@
	
$(mxe_test_so_builddir):
	@$(mkdir) $@

clean:
	@$(rm) $(all_binaries) $(mxe_test_so_builddir)/*.dep

rm: clean
	@$(rmr) $(mxe_test_so_builddir)

include $(wildcard $(mxe_test_so_builddir)/*.dep)

#END
