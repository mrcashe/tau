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

srcdirs += $(posix_srcdir) $(xcb_srcdir) $(unix_srcdir) $(confdir)/$(plat) $(srcdir)/so $(posix_srcdir)/so
VPATH = $(srcdirs)
sources = $(foreach dir, $(srcdirs), $(wildcard $(dir)/*.cc))
objects = $(addprefix $(unix_so_builddir)/, $(addsuffix .o, $(sort $(basename $(notdir $(sources))))))
CXXFLAGS += -O2 -g

all: $(unix_sodir) $(unix_so_builddir) $(unix_so)

$(unix_so): $(objects)
	$(CXX) -o $@ -g -shared -fPIC $(unix_so_builddir)/*.o && chmod -x $@

install: $(lib_prefix)
	@if [ -f $(unix_so) ]; then \
	    echo "++ unix-so.mk: rebuilding shared library with -soname option..."; \
	    $(CXX) -o $(unix_sopath) -shared -fPIC -Wl,-soname,$(unix_soname) $(unix_so_builddir)/*.o; \
	    chmod -x $(unix_sopath); \
	    strip --strip-unneeded $(unix_sopath); \
	    (cd $(lib_prefix) && ln -vsf $(unix_sofile) $(unix_soname)); \
	    (cd $(lib_prefix) && ln -vsf $(unix_soname) $(unix_solink)); \
	    if [ $$USER = 'root' ]; then ldconfig; fi; \
	fi

uninstall:
	@$(rm) $(lib_prefix)/$(unix_solink) $(lib_prefix)/$(unix_soname) $(lib_prefix)/$(unix_sofile)

$(lib_prefix):
	@$(mkdir) $@

$(unix_so_builddir):
	@$(mkdir) $@

$(unix_sodir):
	@$(mkdir) $@

$(unix_so_builddir)/%.o: %.cc
	$(CXX) -o $@ $< -fPIC $(CXXFLAGS) -c -MD -MF $(basename $@).dep

include $(wildcard $(unix_so_builddir)/*.dep)

clean:
	@$(rm) $(unix_so_builddir)/*.o $(unix_so_builddir)/*.dep $(unix_so)

rm: clean
	@$(rmr) $(unix_so_builddir)

#END
