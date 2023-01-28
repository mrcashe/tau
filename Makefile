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

all: all-build
install: all install-build
uninstall: uninstall-build
clean: clean-build

include conf/conf.mk

export srcdirs = $(srcdir) $(confdir)
export hh_src = $(srcdir)/include
CXXFLAGS += -I$(hh_src) -I$(srcdir)

BUILD_TARGETS = all-build install-build uninstall-build clean-build
.PHONY: $(BUILD_TARGETS) rm xz dist test-links icheck

$(BUILD_TARGETS): $(builddir)
	@target=`echo $@ | sed s/-build//`;\
	$(MAKE) -C $$builddir $$target || exit 1

conf/conf.mk:
	@./configure || exit 1

rm:
	@rm -vrf $(builddir) $(bindir) $(confdir)

$(builddir):
	@mkdir -vp $@
	@$(ln) "$(supdir)/build.mk" $@/Makefile
	@[ -z $(wildcard "$@/*.mk") ] && $(MAKE) $(conf_targets)

test-links: $(builddir)
	@if [ ! -d $(builddir)/test ]; then \
	    mkdir -vp "$(builddir)/test"; \
	    for f in $(wildcard $(srcdir)/test/*.cc); do \
		cc=`basename $$f`; \
		$(ln) "$(srcdir)/test/$$cc" "$(builddir)/test/$$cc"; \
	    done; \
	fi

xz:
	@cd ../; \
	tar -cJf tau-$(shell date +%Y%m%d).tar.xz \
	    tau/share tau/doc tau/src tau/sup \
	    tau/AUTHORS* tau/configure tau/COPYING* tau/Makefile* \
	    tau/README* tau/VERSION

icheck:
	@bash -c $(supdir)/icheck.bash

dist: en-doc doc
	@cwd=$(PWD); \
	cd ../; \
	tar -cJf tau-$(Major_).$(Minor_).$(Micro_).tar.xz \
	    tau/share tau/doc tau/src tau/sup \
	    tau/AUTHORS* tau/configure tau/COPYING* tau/Makefile* \
	    tau/README* tau/VERSION; \
	cd $(builddir)/doxygen; \
	pdf=''; \
	if [ -d $(builddir)/doxygen/latex ]; then pdf='*.pdf'; fi; \
	doc="tau-$(Major_).$(Minor_).$(Micro_)-doc.tar.xz"; \
	tar -cJf $$doc html $$pdf; \
	mv -f $$doc $$cwd/..;\
	cd $$cwd

.PHONY: en-host su-host rm-host en-a en-so en-test su-a su-so su-test rm-a rm-so rm-test

en-host: en-host-a en-host-so en-host-test
su-host: su-host-a su-host-so su-host-test
rm-host: rm-host-a rm-host-so rm-host-test

en-a: 	 en-host-a en-mxe-a
en-so:	 en-host-so en-mxe-so
en-test: en-host-test en-mxe-test
en-test-a: en-host-test-a en-mxe-test-a
en-test-so: en-host-test-so en-mxe-test-so

su-a:	 su-host-a su-mxe-a
su-so:	 su-host-so su-mxe-so
su-test: su-host-test su-mxe-test

rm-a:	 rm-host-a rm-mxe-a
rm-so:	 rm-host-so rm-mxe-so
rm-test: rm-host-test rm-mxe-test

#END
