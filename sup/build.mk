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

Makefiles = $(sort $(wildcard *.mk))

all: all-recursive
clean: clean-recursive
install: install-recursive install-hh install-pc install-share
uninstall: uninstall-recursive clean-hh uninstall-pc uninstall-share

.PHONY: $(RECURSIVE_TARGETS) install-hh clean-hh install-share clean-share  install-pc uninstall-pc

RECURSIVE_TARGETS = all-recursive install-recursive uninstall-recursive clean-recursive

$(RECURSIVE_TARGETS):
	@target=`echo $@ | sed s/-recursive//`; \
	list='$(Makefiles)'; \
	for makefile in $$list; do $(MAKE) MAKEFILE=$$makefile -f $$makefile $$target || exit 1; done

install-hh: clean-hh
	@mkdir -vp $(hh_prefix)/tau
	@cp -vp $(hh_src)/tau/*.hh $(hh_prefix)/tau
	@cp -vp $(hh_src)/*.hh $(hh_prefix)

clean-hh:
	@if [ -d $(hh_prefix) ]; then \
	    cwd=`pwd`; \
	    hhs='$(notdir $(wildcard $(hh_src)/tau/*.hh))'; \
	    cd $(hh_prefix)/tau; \
	    for file in $$hhs; do rm -vf $$file; done; \
	    hhs='$(notdir $(wildcard $(hh_src)/*.hh))'; \
	    cd $(hh_prefix); \
	    for file in $$hhs; do rm -vf $$file; done; \
	    cd $$cwd; \
	fi; \
	if [ -z `ls $(hh_prefix)/tau 2>/dev/null` ]; then rm -vrf $(hh_prefix)/tau; fi; \
	if [ -z `ls $(hh_prefix) 2>/dev/null` ]; then rm -vrf $(hh_prefix); fi

install-share:
	@mkdir -vp $(share_prefix)
	@cp -vRpf $(topdir)/share/* $(share_prefix)

uninstall-share:
	@rm -vrf $(share_prefix)

install-pc: $(pc_prefix)
	@if [ -e $(unix_a_dest) -o -e $(unix_so_dest) ]; then \
	    echo "prefix=$(PREFIX)" >$(unix_pc); \
	    echo "exec_prefix=$(PREFIX)" >>$(unix_pc); \
	    echo "lib_prefix=$(PREFIX)/lib">>$(unix_pc); \
	    echo "includedir=$(hh_prefix)">>$(unix_pc); \
	    [ -e $(unix_a_dest) ] && echo "a=$(unix_a_dest)">>$(unix_pc); \
	    echo "Name: libtau">>$(unix_pc); \
	    echo "Description: C++ GUI toolkit">>$(unix_pc); \
	    echo "Version: $(Major_).$(Minor_).$(Micro_)">>$(unix_pc); \
	    echo "Requires: $(pkg_required)">>$(unix_pc); \
	    echo -n "Libs: -L$$">>$(unix_pc); \
	    echo -n "{lib_prefix} -lpthread $(unix_sys_shared)">>$(unix_pc); \
	    [ -e $(unix_so_dest) ] && echo -n " -l$(unix_pkg)">>$(unix_pc); \
	    echo "">>$(unix_pc); \
	    echo -n "Cflags: -I$$">>$(unix_pc); \
	    echo "{includedir}">>$(unix_pc); \
	fi

uninstall-pc:
	@rm -vf $(unix_pc)

$(pc_prefix):
	@mkdir -vp $@

#END
