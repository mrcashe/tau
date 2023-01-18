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

all_sources = $(basename $(notdir $(wildcard $(srcdir)/test/*.cc)))
all_binaries = $(addprefix $(bindir)/, $(all_sources))
sources = $(basename $(notdir $(wildcard $(builddir)/test/*.cc)))
binaries = $(addprefix $(bindir)/, $(sources))
unix_sys_shared += $(shell pkg-config --libs $(pkg_required))
CXXFLAGS += -O2 -g -Wall -fPIC -pthread $(hh_option) $(unix_sys_headers)
VPATH = $(srcdir)/test

all: $(unix_test_so_builddir) $(bindir) $(binaries)

install: $(bin_prefix) $(binaries)
	@for f in $(sources); do \
	    if [ -h $$bin_prefix/$$f ]; then \
		echo "** unix-test-so.mk: skipping install of $$bin_prefix/$$f: destination is a symlink"; \
	    else \
		echo "++ unix-test-so.mk: recompiling '$$f' against just installed shared library..."; \
		$(CXX) $(CXXFLAGS) -o $$bin_prefix/$$f $$srcdir/test/$$f.cc -L $(lib_prefix) -ltau-$(Major_).$(Minor_) $(unix_sys_shared); \
		if [ $$? -ne 0 ]; then \
		    echo "** unix-test-so.mk: compile failed, exitting  with status 1" 1>&2; \
		    exit 1; \
		fi; \
		strip --strip-unneeded $$bin_prefix/$$f; \
	    fi; \
	done

uninstall:
	@for f in $(sources); do \
	    if [ -h $$bin_prefix/$$f ]; then \
		echo "** unix-test-so.mk: skipping removal of $$bin_prefix/$$f: file is a symlink"; \
	    else \
		rm -vf $$bin_prefix/$$f; \
	    fi; \
	done

$(bindir)/%: %.cc
	$(CXX) $(CXXFLAGS) -o $@ $< -MD -MF $(unix_test_so_builddir)/$(notdir $@).dep $(unix_so) $(unix_sys_shared)

$(bindir):
	@mkdir -vp $@

$(bin_prefix):
	@mkdir -vp $@
	
$(unix_test_so_builddir):
	@mkdir -vp $@

clean:
	@rm -vf $(all_binaries) $(unix_test_so_builddir)/*.dep

rm: clean
	@rm -vrf $(unix_test_so_builddir)

include $(wildcard $(unix_test_so_builddir)/*.dep)

#END
