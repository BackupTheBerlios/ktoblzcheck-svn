# generated 2002/7/13 14:37:33 CEST by fabian@fabian.(none)
# using glademm V0.5_11f
#AUTOMAKE_OPTIONS = 1.4


SUBDIRS = src doc macros

ACLOCAL_AMFLAGS = -I macros

EXTRA_WIN_DIST =  README.WIN32 INSTALL.WIN32 AUTHORS README COPYING

EXTRA_DIST = $(EXTRA_WIN_DIST) \
  ktoblzcheck.spec.in \
  ktoblzcheck.spec \
  ktoblzcheck.vcproj

rpm:    $(PACKAGE).spec dist
	rpmbuild="rpm" && \
	if [ `rpm --version | awk '{ print $$3 }'` > /dev/null ]; then rpmbuild="rpmbuild"; fi && \
	$$rpmbuild -ta $(PACKAGE)-$(VERSION).tar.gz

dll: 
	make -C src/lib dll

win_buildroot=distribution
win_develdir=$(PACKAGE)-$(VERSION)-win32-devel
win_bindir=$(PACKAGE)-$(VERSION)-win32-bin

win32: dist
	$(am__remove_distdir)
	GZIP=$(GZIP_ENV) gunzip -c $(distdir).tar.gz | $(AMTAR) xf -
	chmod -R a-w $(distdir); chmod a+w $(distdir)
	mkdir $(distdir)/=build
	mkdir $(distdir)/=inst
	mkdir -p $(win_buildroot)
	mkdir -p $(win_buildroot)/$(win_develdir)
	mkdir -p $(win_buildroot)/$(win_bindir)
	chmod a-w $(distdir)
	dc_install_base=`$(am__cd) $(distdir)/=inst && pwd` \
	  && dc_destdir="$${TMPDIR-/tmp}/am-dc-$$$$/" \
	  && cd $(distdir)/=build \
	  && ../configure --srcdir=.. --prefix="$$dc_install_base" \
	    $(DISTCHECK_CONFIGURE_FLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) check \
	  && $(MAKE) $(AM_MAKEFLAGS) install \
	  && cp src/lib/$(PACKAGE)32.dll ../../$(win_buildroot)/$(win_bindir) \
	  && cp src/lib/$(PACKAGE)32.lib ../../$(win_buildroot)/$(win_develdir) \
	  && cd ../../$(win_buildroot) \
	  && cp -r "$$dc_install_base"/include $(win_develdir) \
	  && cp -r "$$dc_install_base"/share $(win_bindir) \
	  && cp -r "$$dc_install_base"/bin $(win_bindir) \
	  && cd .. \
	  && cp $(EXTRA_WIN_DIST) $(win_buildroot)/$(win_develdir) \
	  && cp $(EXTRA_WIN_DIST) $(win_buildroot)/$(win_bindir) \
	  && cd $(win_buildroot) \
	  && strip $(win_bindir)/*.dll \
	  && zip -9r $(win_develdir).zip $(win_develdir) \
	  && zip -9r $(win_bindir).zip $(win_bindir) \
	  && rm -Rf $(win_develdir) $(win_bindir)
	$(am__remove_distdir)
