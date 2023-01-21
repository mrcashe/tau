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

all:
install: install-hh install-host install-mxe
uninstall: clean-hh uninstall-host uninstall-mxe
clean:

.PHONY: install-hh clean-hh install-host uninstall-host install-mxe uninstall-mxe

install-hh: clean-hh
	@$(mkdir) $(hh_prefix)/tau
	@$(cp) $(hh_src)/tau/*.hh $(hh_prefix)/tau/
	@$(cp) $(hh_src)/*.hh $(hh_prefix)/

clean-hh:
	@if [ -d $(hh_prefix) ]; then \
	    cwd=`pwd`; \
	    hhs='$(notdir $(wildcard $(hh_src)/tau/*.hh))'; \
	    if [ -d "$(hh_prefix)/tau" ]; then \
		cd $(hh_prefix)/tau; \
		for file in $$hhs; do $(rm) $$file; done; \
	    fi; \
	    hhs='$(notdir $(wildcard $(hh_src)/*.hh))'; \
	    cd $(hh_prefix); \
	    for file in $$hhs; do $(rm) $$file; done; \
	    cd $$cwd; \
	fi

install-host:
	@if [ -e $(unix_adest) -o -e $(unix_sopath) ]; then \
	    $(mkdir) $(pc_prefix); \
	    echo "++ dev.mk: writing $(unix_pc)..."; \
	    echo "prefix=$(PREFIX)" >$(unix_pc); \
	    echo "exec_prefix=$(PREFIX)" >>$(unix_pc); \
	    echo "lib_prefix=$(PREFIX)/lib">>$(unix_pc); \
	    echo "includedir=$(hh_prefix)">>$(unix_pc); \
	    [ -e $(unix_adest) ] && echo "a=$(unix_adest)">>$(unix_pc); \
	    echo "Name: tau">>$(unix_pc); \
	    echo "Description: C++ GUI toolkit">>$(unix_pc); \
	    echo "Version: $(Major_).$(Minor_).$(Micro_)">>$(unix_pc); \
	    echo "Requires: $(pkg_required)">>$(unix_pc); \
	    echo -n "Libs: -L$$">>$(unix_pc); \
	    echo -n "{lib_prefix} -lpthread $(unix_sys_shared)">>$(unix_pc); \
	    [ -e $(unix_sopath) ] && echo -n " -ltau-$(Major_).$(Minor_)">>$(unix_pc); \
	    echo "">>$(unix_pc); \
	    echo -n "Cflags: -I$$">>$(unix_pc); \
	    echo "{includedir}">>$(unix_pc); \
	fi

install-mxe:
	@if [ -e $(mxe_a_dest) -o -e $(mxe_so_dest) ]; then \
	    $(mkdir) $(pc_prefix); \
	    echo "++ dev.mk: writing $(mxe_pc)..."; \
	    echo "prefix=$(PREFIX)" >$(mxe_pc); \
	    echo "exec_prefix=$(PREFIX)" >>$(mxe_pc); \
	    echo "lib_prefix=$(PREFIX)/lib">>$(mxe_pc); \
	    echo "includedir=$(hh_prefix)">>$(mxe_pc); \
	    [ -e $(mxe_a_dest) ] && echo "a=$(mxe_a_dest)">>$(mxe_pc); \
	    [ -e $(mxe_so_dest) ] && echo "dll=$(mxe_so_dest)">>$(mxe_pc); \
	    echo "mxe_prefix=$(mxe_prefix)">>$(mxe_pc); \
	    echo "mxe_target=$(mxe_target)">>$(mxe_pc); \
	    echo "mxe_libroot=$(mxe_prefix)/$(mxe_target)/lib">>$(mxe_pc); \
	    echo "mxe_syslibs=$(mxe_syslibs)">>$(mxe_pc); \
	    echo "Name: libtau">>$(mxe_pc); \
	    echo "Description: C++ GUI toolkit using M cross environment (MXE)">>$(mxe_pc); \
	    echo "Version: $(Major_).$(Minor_).$(Micro_)">>$(mxe_pc); \
	    echo -n "Libs: -L$$">>$(mxe_pc); \
	    echo "{lib_prefix}">>$(mxe_pc); \
	    echo -n "Cflags: -I$$">>$(mxe_pc); \
	    echo "{includedir}">>$(mxe_pc); \
	fi

uninstall-host:
	@$(rm) $(unix_pc)

uninstall-mxe:
	@$(rm) $(mxe_pc)
