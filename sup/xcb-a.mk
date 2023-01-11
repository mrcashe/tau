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

srcroot := $(topdir)/src
xcb_srcdir := $(srcroot)/xcb
src_dirs := $(xcb_srcdir)
VPATH = $(src_dirs)

sources := $(sort $(basename $(notdir $(wildcard $(xcb_srcdir)/*.cc))))

objects := $(addprefix $(xcb_a_builddir)/, $(addsuffix .o, $(sources)))
deps := $(addprefix $(xcb_a_builddir)/, $(addsuffix .dep, $(sources)))
CXXFLAGS += $(unix_CXXFLAGS)
AR = $(unix_AR)

all: $(xcb_a_builddir) $(objects)

install:
uninstall:

$(xcb_a_builddir)/%.o: %.cc
	$(CXX) -o $@ $< $(CXXFLAGS) -c -MD -MF $(basename $@).dep && $(AR) rs $(unix_a) $@

$(xcb_a_builddir):
	@mkdir -vp $@

include $(wildcard $(xcb_a_builddir)/*.dep)

clean:
	@rm -vf $(objects) $(deps)

rm: clean
	@rm -vrf $(xcb_a_builddir)

#END
