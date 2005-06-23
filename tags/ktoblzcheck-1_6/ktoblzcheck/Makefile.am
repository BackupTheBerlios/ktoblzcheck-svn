# generated 2002/7/13 14:37:33 CEST by fabian@fabian.(none)
# using glademm V0.5_11f
#AUTOMAKE_OPTIONS = 1.4


SUBDIRS = src doc macros

ACLOCAL_AMFLAGS = -I macros

EXTRA_WIN_DIST =  README.WIN32 INSTALL.WIN32 AUTHORS README COPYING

EXTRA_DIST = $(EXTRA_WIN_DIST) \
  ktoblzcheck.spec.in \
  ktoblzcheck.spec \
  ktoblzcheck.vcproj \
  ktoblzcheck.iss.in

rpm:    $(PACKAGE).spec dist
	rpmbuild="rpm" && \
	if [ `rpm --version | awk '{ print $$3 }'` > /dev/null ]; then rpmbuild="rpmbuild"; fi && \
	$$rpmbuild -ta $(PACKAGE)-$(VERSION).tar.gz

dll: 
	make -C src/lib dll

srcdoc: Doxyfile 
	make -C doc mainpage.doxygen
	doxygen

# Rule for using the Inno Setup compiler. The path to iscc.exe is set
# manually here for now.
ISCC = "C:\Programme\Inno Setup 4\iscc.exe"
ISCC_TMPDIR = win32-tmp
setup: $(PACKAGE).iss
	mkdir -p $(ISCC_TMPDIR)
	make prefix="`pwd`/$(ISCC_TMPDIR)" BANKDATA_PATH="`pwd`/$(ISCC_TMPDIR)/data" install \
	 && $(ISCC) $(PACKAGE).iss \
	 && rm -rf $(ISCC_TMPDIR)
