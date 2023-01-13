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
mxe_so_sid = 12
mxe_test_a_sid = 18
mxe_test_so_sid = 20

.PHONY: en-mxe-a su-mxe-a rm-mxe-a en-mxe-so su-mxe-so rm-mxe-so

en-mxe-a: $(builddir)
	@ln -vsf $(supdir)/mxe-a.mk $(builddir)/$(mxe_a_sid)-$(mxe_target)-mxe-a.mk

su-mxe-a: 
	@rm -vf $(builddir)/$(mxe_a_sid)-$(mxe_target)-mxe-a.mk

rm-mxe-a: rm-mxe-test
	@if [ -e $(builddir)/$(mxe_a_sid)-$(mxe_target)-mxe-a.mk ]; then $(MAKE) MAKEFILE=$(mxe_a_sid)-$(mxe_target)-mxe-a.mk -C $(builddir) -f $(mxe_a_sid)-$(mxe_target)-mxe-a.mk rm; fi; \
	rm -vf $(builddir)/$(mxe_a_sid)-$(mxe_target)-mxe-a.mk

en-mxe-so: $(builddir)
	@ln -vsf $(supdir)/mxe-so.mk $(builddir)/$(mxe_so_sid)-$(mxe_target)-mxe-so.mk

su-mxe-so: 
	@rm -vf $(builddir)/$(mxe_so_sid)-$(mxe_target)-mxe-so.mk

rm-mxe-so:
	@if [ -e $(builddir)/$(mxe_so_sid)-$(mxe_target)-mxe-so.mk ]; then $(MAKE) MAKEFILE=$(mxe_so_sid)-$(mxe_target)-mxe-so.mk -C $(builddir) -f $(mxe_a_sid)-$(mxe_target)-mxe-a.mk rm; fi; \
	rm -vf $(builddir)/$(mxe_so_sid)-$(mxe_target)-mxe-so.mk

.PHONY: en-mxe-test su-mxe-test rm-mxe-test
.PHONY: en-mxe-test-a su-mxe-test-a rm-mxe-test-a
.PHONY: en-mxe-test-so su-mxe-test-so rm-mxe-test-so


en-mxe-test-a: $(builddir) rm-mxe-test-so test-links en-mxe-a
	@ln -vsf $(supdir)/mxe-test-a.mk $(builddir)/$(mxe_test_a_sid)-$(mxe_target)-mxe-test-a.mk

su-mxe-test-a:
	@rm -vf $(builddir)/$(mxe_test_a_sid)-$(mxe_target)-mxe-test-a.mk

rm-mxe-test-a:
	@if [ -e $(builddir)/$(mxe_test_a_sid)-$(mxe_target)-mxe-test-a.mk ]; then $(MAKE) MAKEFILE=$(mxe_test_a_sid)-$(mxe_target)-mxe-test.mk -C $(builddir) -f $(mxe_test_sid)-$(mxe_target)-mxe-test-a.mk rm; fi; \
	rm -vf $(builddir)/$(mxe_test_a_sid)-$(mxe_target)-mxe-test-a.mk

en-mxe-test-so: $(builddir) rm-mxe-test-a test-links en-mxe-so
	@ln -vsf $(supdir)/mxe-test-so.mk $(builddir)/$(mxe_test_so_sid)-$(mxe_target)-mxe-test-so.mk
	
su-mxe-test-so:
	@rm -vf $(builddir)/$(mxe_test_so_sid)-$(mxe_target)-mxe-test-so.mk
	
rm-mxe-test-so:
	@if [ -e $(builddir)/$(mxe_test_so_sid)-$(mxe_target)-mxe-test-so.mk ]; then $(MAKE) MAKEFILE=$(mxe_test_so_sid)-$(mxe_target)-mxe-test.mk -C $(builddir) -f $(mxe_test_sid)-$(mxe_target)-mxe-test-so.mk rm; fi; \
	rm -vf $(builddir)/$(mxe_test_so_sid)-$(mxe_target)-mxe-test-so.mk

en-mxe-test:
	@if [ -e $(builddir)/$(mxe_a_sid)-$(mxe_target)-mxe-a.mk ]; then $(MAKE) en-mxe-test-a; \
	else $(MAKE) en-mxe-test-so; fi
		
su-mxe-test: su-mxe-test-a su-mxe-test-so
rm-mxe-test: rm-mxe-test-a rm-mxe-test-so
	
.PHONY:  su-mxe en-mxe rm-mxe
en-mxe:  en-mxe-a en-mxe-so en-mxe-test-so
su-mxe:  su-mxe-a su-mxe-so su-mxe-test
rm-mxe:  rm-mxe-a rm-mxe-so rm-mxe-test
