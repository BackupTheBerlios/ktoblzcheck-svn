# generated 2002/7/13 14:37:33 CEST by fabian@fabian.(none)
# using glademm V0.5_11f
AUTOMAKE_OPTIONS = 1.4


SUBDIRS = src 

EXTRA_DIST = AUTHORS TODO README configure 

rpm: ktoblzcheck.spec
	@echo "Please use one of the targets below:"
	@echo " rpm-suse81     - build RPMs for SuSE 8.1"
	@echo " rpm-suse82     - build RPMs for SuSE 8.2"
	@echo " rpm-redhat80   - build RPMs for Redhat 8.0"
	@echo " rpm-redhat90   - build RPMs for Redhat 9.0"
	@echo " rpm-mandrake90 - build RPMs for Mandrake 9.0"

rpm-suse81: ktoblzcheck.spec
	rpm -ba ktoblzcheck.spec
	mkdir -p distribution
	cp /usr/src/packages/RPMS/i386/ktoblzcheck-@rpm_version@-@rpm_release@.i386.rpm \
           `pwd`/distribution/ktoblzcheck-@rpm_version@-@rpm_release@.i586.suse81.rpm

rpm-suse82: ktoblzcheck.spec
	rpm -ba ktoblzcheck.spec
	mkdir -p distribution
	cp /usr/src/packages/RPMS/i386/ktoblzcheck-@rpm_version@-@rpm_release@.i386.rpm \
           `pwd`/distribution/ktoblzcheck-@rpm_version@-@rpm_release@.i586.suse82.rpm

rpm-redhat80: ktoblzcheck.spec
	rpmbuild -ba ktoblzcheck.spec
	mkdir -p distribution
	cp /usr/src/redhat/RPMS/i386/ktoblzcheck-@rpm_version@-@rpm_release@.i386.rpm \
           `pwd`/distribution/ktoblzcheck-@rpm_version@-@rpm_release@.i586.redhat80.rpm

rpm-redhat90: ktoblzcheck.spec
	rpmbuild -ba ktoblzcheck.spec
	mkdir -p distribution
	cp /usr/src/redhat/RPMS/i386/ktoblzcheck-@rpm_version@-@rpm_release@.i386.rpm \
           `pwd`/distribution/ktoblzcheck-@rpm_version@-@rpm_release@.i586.redhat90.rpm

rpm-mandrake90: ktoblzcheck.spec
	rpmbuild -ba ktoblzcheck.spec
	mkdir -p distribution
	cp /usr/src/RPM/RPMS/i586/ktoblzcheck-@rpm_version@-@rpm_release@.i586.rpm \
           `pwd`/distribution/ktoblzcheck-@rpm_version@-@rpm_release@.i586.mandrake90.rpm
