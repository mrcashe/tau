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

export mxe_aname = libtau-$(Major_).$(Minor_)-mxe.a
export mxe_adir = $(bindir)
export mxe_a = $(mxe_adir)/$(mxe_aname)
export mxe_a_dest = $(lib_prefix)/$(mxe_target)-$(mxe_aname)

export mxe_soname = libtau-$(Major_).$(Minor_)-$(mxe_target)-mxe.dll
export mxe_sodir = $(bindir)
export mxe_so = $(mxe_sodir)/$(mxe_soname)
export mxe_so_dest = $(bin_prefix)/$(mxe_soname)

export mxe_pc = $(pc_prefix)/tau-$(Major_).$(Minor_)-$(mxe_target)-mxe.pc

mxe_a_mk = "10-mxe-a.mk"
mxe_so_mk = "12-mxe-so.mk"
mxe_test_a_mk = "18-mxe-test-a.mk"
mxe_test_so_mk = "20-mxe-test-so.mk"

.PHONY: en-mxe-a su-mxe-a rm-mxe-a en-mxe-so su-mxe-so rm-mxe-so

en-mxe-a: $(builddir) lj
	@$(ln) $(supdir)/mxe-a.mk $(builddir)/$(mxe_a_mk)

su-mxe-a: lj
	@$(rm) $(builddir)/$(mxe_a_mk)

rm-mxe-a: rm-mxe-test lj
	@if [ -e "$(builddir)/$(mxe_a_mk)" ]; then $(MAKE) -C $(builddir) -f $(mxe_a_mk) rm; fi
	@$(rm) $(builddir)/$(mxe_a_mk)

en-mxe-so: $(builddir) lj
	@$(ln) $(supdir)/mxe-so.mk $(builddir)/$(mxe_so_mk)

su-mxe-so: lj
	@$(rm) $(builddir)/$(mxe_so_mk)

rm-mxe-so: lj
	@if [ -e $(builddir)/$(mxe_a_mk) ]; then $(MAKE) -C $(builddir) -f $(mxe_a_mk) rm; fi
	@$(rm) $(builddir)/$(mxe_so_mk)

.PHONY: en-mxe-test su-mxe-test rm-mxe-test
.PHONY: en-mxe-test-a su-mxe-test-a rm-mxe-test-a
.PHONY: en-mxe-test-so su-mxe-test-so rm-mxe-test-so

en-mxe-test-a: rm-mxe-test-so test-links en-mxe-a
	@$(ln) $(supdir)/mxe-test-a.mk $(builddir)/$(mxe_test_a_mk)

su-mxe-test-a: lj
	@$(rm) $(builddir)/$(mxe_test_a_mk)

rm-mxe-test-a: lj
	@if [ -e $(builddir)/$(mxe_test_a_mk) ]; then $(MAKE) -C $(builddir) -f $(mxe_test_a_mk) rm; fi
	@$(rm) $(builddir)/$(mxe_test_a_mk)

en-mxe-test-so: rm-mxe-test-a test-links en-mxe-so
	@$(ln) $(supdir)/mxe-test-so.mk $(builddir)/$(mxe_test_so_mk)
	
su-mxe-test-so: lj
	@$(rm) $(builddir)/$(mxe_test_so_mk)
	
rm-mxe-test-so: lj
	@if [ -e "$(builddir)/$(mxe_test_so_mk)" ]; then $(MAKE) -C $(builddir) -f $(mxe_test_so_mk) rm; fi
	@$(rm) $(builddir)/$(mxe_test_so_mk)

en-mxe-test:
	@if [ -e "$(builddir)/$(mxe_a_mk)" ]; then $(MAKE) en-mxe-test-a; \
	else $(MAKE) en-mxe-test-so; fi
		
su-mxe-test: su-mxe-test-a su-mxe-test-so
rm-mxe-test: rm-mxe-test-a rm-mxe-test-so
	
.PHONY:  su-mxe en-mxe rm-mxe
en-mxe:  en-mxe-a en-mxe-so en-mxe-test-so
su-mxe:  su-mxe-a su-mxe-so su-mxe-test
rm-mxe:  rm-mxe-a rm-mxe-so rm-mxe-test
