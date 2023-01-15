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

mxe_so_builddir = mxe-so
src_dirs = $(srcdir) $(srcdir)/Windows $(confdir)
sources = $(foreach dir, $(src_dirs), $(wildcard $(dir)/*.cc))
objects = $(addprefix $(mxe_so_builddir)/, $(sort $(addsuffix .o, $(basename $(notdir $(sources))))))
syslibs = $(addprefix $(mxe_prefix)/$(mxe_target)/lib/,$(mxe_syslibs))
CXXFLAGS += -O2 -Wall $(hh_impl_options)
MXE_CXX = $(mxe_prefix)/bin/$(mxe_target)-g++
MXE_STRIP = $(mxe_prefix)/bin/$(mxe_target)-strip
VPATH = $(src_dirs)

all: $(mxe_sodir) $(mxe_so_builddir) $(mxe_so)

$(mxe_so): $(objects)
	$(MXE_CXX) -shared -o $@ $(mxe_so_builddir)/*.o $(syslibs)

clean:
	@$(rm) $(mxe_so_builddir)/*.o $(mxe_so_builddir)/*.dep $(mxe_so)

rm: clean
	@$(rmr) $(mxe_so_builddir)

$(mxe_so_builddir)/%.o: %.cc
	$(MXE_CXX) $(CXXFLAGS) -c -MD -MF $(basename $@).dep -o $@ $<

include $(wildcard $(mxe_so_builddir)/*.dep)

install: $(lib_prefix)
	@$(cp) $(mxe_so) $(mxe_so_dest)
	@$(MXE_STRIP) --strip-unneeded $(mxe_so_dest)

uninstall:
	@$(rm) $(mxe_so_dest)

$(mxe_sodir):
	@$(mkdir) $@

$(lib_prefix):
	@$(mkdir) $@

$(mxe_so_builddir):
	@$(mkdir) $@

#END
