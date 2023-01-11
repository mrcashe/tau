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

all: all-recursive
install: all install-recursive install-doc
uninstall: uninstall-recursive uninstall-doc
clean: clean-recursive clean-doc

include conf/conf.mk

export hh_src = $(srcdir)/include
export hh_option = -I$(hh_src)
export hh_impl_options = $(hh_option) -I$(topdir)/src -I$(confdir)/src

SUBDIRS = $(builddir)
RECURSIVE_TARGETS = all-recursive install-recursive uninstall-recursive clean-recursive

.PHONY: $(RECURSIVE_TARGETS) configure rm dist

$(RECURSIVE_TARGETS):
	@target=`echo $@ | sed s/-recursive//`;\
	list='$(SUBDIRS)'; \
	for subdir in $$list; do $(MAKE) -C $$subdir $$target || exit 1; done

conf/conf.mk:
	@./configure || exit 1

configure:
	@./configure || exit 1

rm:
	@rm -vrf $(builddir) $(bindir) $(topdir)/conf

xz:
	@cd ../; \
	tar -cJf tau-$(shell date +%Y%m%d).tar.xz \
	    tau/share tau/doc tau/src tau/sup \
	    `find tau -maxdepth 1 ! -type d`

.PHONY: en-host su-host rm-host
en-host: en-host-a en-host-so en-host-test
su-host: su-host-a su-host-so su-host-test
rm-host: rm-host-a rm-host-so rm-host-test

.PHONY: en-a en-so en-test su-a su-so su-test rm-a rm-so rm-test
en-a: 	 en-host-a en-mxe-a
en-so:	 en-host-so
en-test: en-host-test en-mxe-test

su-a:	 su-host-a su-mxe-a
su-so:	 su-host-so
su-test: su-host-test su-mxe-test

rm-a:	 rm-host-a rm-mxe-a
rm-so:	 rm-host-so
rm-test: rm-host-test rm-mxe-test

#END
