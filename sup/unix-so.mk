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

src_dirs = $(cplat_srcdir) $(posix_srcdir) $(unix_srcdir) $(confdir)
VPATH = $(src_dirs)
sources = $(foreach dir, $(src_dirs), $(wildcard $(dir)/*.cc))
objects = $(addprefix $(unix_so_builddir)/, $(sort $(addsuffix .o, $(basename $(notdir $(sources))))))
CXXFLAGS += -O2 $(unix_CXXFLAGS)

all: $(unix_sodir) $(unix_so_builddir) $(unix_so)

$(unix_so): $(objects)
	$(CXX) -shared -o $@ $(unix_so_builddir)/*.o $(xcb_so_builddir)/*.o

install: $(lib_prefix)
	@if [ -f $(unix_so) ]; then \
	    cp -f -vp $(unix_so) $(unix_so_dest); \
	    $(unix_STRIP) --strip-unneeded $(unix_so_dest); \
	    (cd $(lib_prefix) && ln -vsf $(unix_soname) $(unix_sobase)); \
	fi

uninstall:
	@rm -vf $(lib_prefix)/$(unix_sobase)*

$(lib_prefix):
	@mkdir -vp $@

$(unix_so_builddir):
	@mkdir -vp $@

$(unix_sodir):
	@mkdir -vp $@

$(unix_so_builddir)/%.o: %.cc
	$(CXX) -o $@ $< -fPIC $(CXXFLAGS) -c -MD -MF $(basename $@).dep

include $(wildcard $(unix_so_builddir)/*.dep)

clean:
	@rm -vf $(unix_so_builddir)/*.o $(unix_so_builddir)/*.dep $(unix_so)

rm: clean
	@rm -vrf $(unix_so_builddir)

#END
