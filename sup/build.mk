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

Makefiles = $(sort $(wildcard *.mk))

all: all-recursive
clean: clean-recursive
install: install-recursive install-share install-doc
uninstall: uninstall-recursive uninstall-share uninstall-doc

.PHONY: $(RECURSIVE_TARGETS) install-share clean-share install-doc uninstall-doc
RECURSIVE_TARGETS = all-recursive install-recursive uninstall-recursive clean-recursive

$(RECURSIVE_TARGETS):
	@target=`echo $@ | sed s/-recursive//`; \
	for mk in $(Makefiles); do $(MAKE) -f $$mk $$target || exit 1; done

install-share:
	@$(mkdir) $(share_prefix)
	@$(cpr) $(topdir)/share/* $(share_prefix)

uninstall-share:
	@$(rmr) $(share_prefix)

install-doc:
	@$(mkdir) $(doc_prefix)
	@$(cp) $(topdir)/AUTHORS $(doc_prefix)/
	@$(cp) $(topdir)/COPYING $(doc_prefix)/
	@$(cp) $(topdir)/README.md $(doc_prefix)/
	@$(cp) $(topdir)/VERSION $(doc_prefix)/

uninstall-doc:
	@$(rmr) $(doc_prefix)

#END
