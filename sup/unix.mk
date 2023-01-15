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

export unix_pkg = tau-$(Major_).$(Minor_)
export unix_pc = $(pc_prefix)/$(unix_pkg).pc
export unix_aname = lib$(unix_pkg).a
export unix_adir = $(builddir)/lib
export unix_a = $(unix_adir)/$(unix_aname)
export unix_a_dest = $(lib_prefix)/$(unix_aname)

export unix_sobase = lib$(unix_pkg).so
export unix_soname = $(unix_sobase).$(Major_).$(Minor_).$(Micro_)
export unix_sodir = $(builddir)/lib
export unix_so = $(unix_sodir)/$(unix_soname)
export unix_so_dest = $(lib_prefix)/$(unix_soname)

unix_a_mk = 00-unix-a.mk
xcb_a_mk = 00-xcb-a.mk
unix_so_mk = 02-unix-so.mk
xcb_so_mk = 01-xcb-so.mk
unix_test_a_mk = 08-unix-test-a.mk
unix_test_so_mk = 08-unix-test-so.mk

.PHONY: en-xcb-a su-xcb-a rm-xcb-a en-xcb-so su-xcb-so rm-xcb-so

en-xcb-a:
	@$(ln) $(topdir)/sup/xcb-a.mk $(builddir)/$(xcb_a_mk)

su-xcb-a:
	@$(rm) $(builddir)/$(xcb_a_mk)

rm-xcb-a:
	@if [ -e $(builddir)/$(xcb_a_mk) ]; then $(MAKE) -C $(builddir) -f $(xcb_a_mk) rm; fi
	@$(rm) "$(builddir)/$(xcb_a_mk)"

en-xcb-so:
	@$(ln) "$(topdir)/sup/xcb-so.mk" "$(builddir)/$(xcb_so_mk)"

su-xcb-so:
	@$(rm) "$(builddir)/$(xcb_so_mk)"

rm-xcb-so:
	@if [ -e $(builddir)/$(xcb_so_mk) ]; then $(MAKE) -C $(builddir) -f $(xcb_so_mk) rm; fi
	@$(rm) "$(builddir)/$(xcb_so_mk)"

.PHONY: en-host-a su-host-a rm-host-a en-host-so su-host-so rm-host-so

en-host-a: $(builddir) en-xcb-a lj
	@$(ln) "$(supdir)/unix-a.mk" "$(builddir)/$(unix_a_mk)"

su-host-a: su-xcb-a su-host-test-a lj
	@$(rm) "$(builddir)/$(unix_a_mk)"

rm-host-a: rm-xcb-a rm-host-test-a lj
	@if [ -e "$(builddir)/$(unix_a_mk)" ]; then $(MAKE) -C $(builddir) -f $(unix_a_mk) rm; fi
	@$(rm) "$(builddir)/$(unix_a_mk)"

en-host-so: $(builddir) en-xcb-so lj
	@$(ln) "$(supdir)/unix-so.mk" "$(builddir)/$(unix_so_mk)"

su-host-so: su-xcb-so su-host-test-so lj
	@$(rm) "$(builddir)/$(unix_so_mk)"

rm-host-so: rm-xcb-so rm-host-test-so lj
	@if [ -e "$(builddir)/$(unix_so_mk)" ]; then $(MAKE) -C $(builddir) -f $(unix_so_mk) rm; fi
	@$(rm) "$(builddir)/$(unix_so_mk)"

.PHONY: en-host-test su-host-test rm-host-test
.PHONY: en-host-test-a su-host-test-a rm-host-test-a
.PHONY: en-host-test-so su-host-test-so rm-host-test-so

en-host-test-a: rm-host-test-so test-links en-host-a
	@$(ln) "$(supdir)/unix-test-a.mk" "$(builddir)/$(unix_test_a_mk)"

su-host-test-a: lj
	@$(rm) "$(builddir)/$(unix_test_a_mk)"

rm-host-test-a: lj
	@if [ -e "$(builddir)/$(unix_test_a_mk)" ]; then $(MAKE) -C $(builddir) -f $(unix_test_a_mk) rm; fi
	@$(rm) $(builddir)/$(unix_test_a_mk)

en-host-test-so: rm-host-test-a test-links en-host-so
	@$(ln) "$(supdir)/unix-test-so.mk" "$(builddir)/$(unix_test_so_mk)"

su-host-test-so: lj
	@$(rm) $(builddir)/$(unix_test_so_mk)

rm-host-test-so: lj
	@if [ -e "$(builddir)/$(unix_test_so_mk)" ]; then $(MAKE) -C $(builddir) -f $(unix_test_so_mk) rm; fi
	@$(rm) "$(builddir)/$(unix_test_so_mk)"

en-host-test:
	@if [ -e $(builddir)/$(unix_a_mk) ]; then $(MAKE) en-host-test-a; \
	else $(MAKE) en-host-test-so; fi

su-host-test: su-host-test-a su-host-test-so
rm-host-test: rm-host-test-a rm-host-test-so

.PHONY: en-host su-host rm-host
en-host: en-host-a en-host-so en-host-test
su-host: su-host-a su-host-so su-host-test
rm-host: rm-host-a rm-host-so rm-host-test

#END
