# generated 2002/7/13 14:37:33 CEST by fabian@fabian.(none)
# using glademm V0.5_11f
#AUTOMAKE_OPTIONS = 1.4


SUBDIRS = src doc

EXTRA_DIST = AUTHORS TODO README \
  ktoblzcheck.spec.in \
  ktoblzcheck.spec

rpm:    $(PACKAGE).spec dist
	rpmbuild="rpm" && \
	if [ `rpm --version | awk '{ print $$3 }'` > /dev/null ]; then rpmbuild="rpmbuild"; fi && \
	$$rpmbuild -ta $(PACKAGE)-$(VERSION).tar.gz
