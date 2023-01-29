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

all:

install:
	@if [ -d $(doxydir) ]; then \
	    $(mkdir) $(doc_prefix); \
	    $(cpr) $(doxydir)/html $(doc_prefix)/html; \
	    $(cp) $(doxydir)/*.pdf $(doc_prefix); \
	fi

uninstall:
	@$(rmr) "$(doc_prefix)/html/" "$(doc_prefix)/*.pdf"

clean:
	@$(rmr) "$(doxydir)"

.PHONY: doc

doc:
	@if test -n $(doxygen); then \
	    (cd $(topdir) && $(doxygen) "$(confdir)/Doxyfile"); \
	    if [ -d $(builddir)/doxygen/latex ]; then \
		gmake -C $(builddir)/doxygen/latex pdf; \
		[ ! -e $(builddir)/doxygen/latex/refman.pdf ] && gmake -C $(builddir)/doxygen/latex; \
		mv -f $(builddir)/doxygen/latex/refman.pdf $(builddir)/doxygen/tau-$(Major_).$(Minor_).$(Micro_).pdf; \
	    fi;\
	fi
