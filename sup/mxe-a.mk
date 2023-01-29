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

mxe_a_builddir = $(builddir)/mxe-a
srcdirs += $(srcdir)/a $(srcdir)/Windows $(srcdir)/Windows/mxe-a $(confdir)/Windows
VPATH = $(srcdirs)
sources = $(foreach dir, $(srcdirs), $(wildcard $(dir)/*.cc))
objects = $(addprefix $(mxe_a_builddir)/, $(addsuffix .o, $(basename $(notdir $(sources)))))
CXXFLAGS += -O2 -g
MXE_CXX = $(mxe_prefix)/bin/$(mxe_target)-g++
MXE_AR = $(mxe_prefix)/bin/$(mxe_target)-ar
MXE_STRIP = $(mxe_prefix)/bin/$(mxe_target)-strip

all: $(mxe_a_builddir) $(mxe_adir) $(mxe_a)

$(mxe_a): $(objects)
	$(MXE_AR) r $@ $(mxe_a_builddir)/*.o

clean:
	@$(rm) $(mxe_a_builddir)/* $(mxe_a)

rm: clean
	@$(rmr) $(mxe_a_builddir)

$(mxe_a_builddir)/%.o: %.cc
	$(MXE_CXX) $(CXXFLAGS) -c -MD -MF $(basename $@).dep -o $@ $<

include $(wildcard $(mxe_a_builddir)/*.dep)

.PHONY: install-a uninstall-a

install: $(lib_prefix)
	@$(cp) $(mxe_a) $(mxe_adest)
	@$(MXE_STRIP) --strip-unneeded $(mxe_adest)

uninstall:
	@$(rm) $(mxe_adest)

$(mxe_adir):
	@$(mkdir) $@

$(lib_prefix):
	@$(mkdir) $@

$(mxe_a_builddir):
	@$(mkdir) $@

#END
