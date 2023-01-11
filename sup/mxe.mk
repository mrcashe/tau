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

mxe_a_sid = 10
mxe_test_sid = 18

.PHONY: en-mxe-a su-mxe-a rm-mxe-a

en-mxe-a:
	@ln -vsf $(topdir)/sup/mxe-a.mk $(builddir)/$(mxe_a_sid)-$(mxe_target)-mxe-a.mk

su-mxe-a:
	@rm -vf $(builddir)/$(mxe_a_sid)-$(mxe_target)-mxe-a.mk

rm-mxe-a: rm-mxe-test
	@if [ -e $(builddir)/$(mxe_a_sid)-$(mxe_target)-mxe-a.mk ]; then $(MAKE) MAKEFILE=$(mxe_a_sid)-$(mxe_target)-mxe-a.mk -C $(builddir) -f $(mxe_a_sid)-$(mxe_target)-mxe-a.mk rm; fi; \
	rm -vf $(builddir)/$(mxe_a_sid)-$(mxe_target)-mxe-a.mk

.PHONY: en-mxe-test su-mxe-test rm-mxe-test

en-mxe-test: test-links en-mxe-a
	@ln -vsf $(topdir)/sup/mxe-test.mk $(builddir)/$(mxe_test_sid)-$(mxe_target)-mxe-test.mk

su-mxe-test:
	@rm -vf $(builddir)/$(mxe_test_sid)-$(mxe_target)-mxe-test.mk

rm-mxe-test:
	@if [ -e $(builddir)/$(mxe_test_sid)-$(mxe_target)-mxe-test.mk ]; then $(MAKE) MAKEFILE=$(mxe_test_sid)-$(mxe_target)-mxe-test.mk -C $(builddir) -f $(mxe_test_sid)-$(mxe_target)-mxe-test.mk rm; fi; \
	rm -vf $(builddir)/$(mxe_test_sid)-$(mxe_target)-mxe-test.mk

.PHONY:  su-mxe en-mxe rm-mxe
en-mxe:  en-mxe-a en-mxe-test
su-mxe:  su-mxe-a su-mxe-test
rm-mxe:  rm-mxe-a rm-mxe-test
