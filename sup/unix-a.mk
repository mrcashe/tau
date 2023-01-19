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

srcdirs += $(posix_srcdir) $(unix_srcdir) $(confdir)/$(plat)
VPATH = $(srcdirs)
sources = $(foreach dir, $(srcdirs), $(wildcard $(dir)/*.cc))
objects = $(addprefix $(unix_a_builddir)/, $(sort $(addsuffix .o, $(basename $(notdir $(sources))))))
CXXFLAGS += -g -O0 $(unix_CXXFLAGS)
AR = $(unix_AR)

all: $(unix_adir) $(unix_a_builddir) $(objects)

install: $(lib_prefix)
	@if [ -f $(unix_a) ]; then \
	    $(cp) $(unix_a) $(unix_a_dest); \
	    strip --strip-unneeded $(unix_a_dest); \
	fi

uninstall:
	@$(rm) $(unix_a_dest)

$(unix_a_builddir)/%.o: %.cc
	$(CXX) -o $@ $< $(CXXFLAGS) -c -MD -MF $(basename $@).dep && $(AR) rs $(unix_a) $@

$(unix_adir):
	@$(mkdir) $@

$(unix_a_builddir):
	@$(mkdir) $@

$(lib_prefix):
	@$(mkdir) $@

clean:
	@$(rm) $(unix_a_builddir)/*.o $(unix_a_builddir)/*.dep $(unix_a)

rm: clean
	@$(rmr) -vrf $(unix_a_builddir)

include $(wildcard $(unix_a_builddir)/*.dep)

#END
