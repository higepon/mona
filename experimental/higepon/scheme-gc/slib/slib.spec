Summary:      platform independent library for scheme
Name:         slib
Version:      3a4
Release:      1
Group:        Development/Languages
BuildArch:    noarch
Packager:     Aubrey Jaffer <agj@alum.mit.edu>

License:      distributable, see individual files for copyright
Vendor:       Aubrey Jaffer <agj @ alum.mit.edu>
Provides:     slib

Source:       ftp://swiss.csail.mit.edu/pub/scm/slib%{version}.zip
URL:          http://swiss.csail.mit.edu/~jaffer/SLIB.html
BuildRoot:    %{_tmppath}/%{name}-%{version}-root

%description
"SLIB" is a portable library for the programming language Scheme.
It provides a platform independent framework for using "packages" of
Scheme procedures and syntax.  As distributed, SLIB contains useful
packages for all Scheme implementations.  Its catalog can be
transparently extended to accomodate packages specific to a site,
implementation, user, or directory.

%prep
%setup -q -n %{name}
for i in *; do
  cp -f ${i} ${i}.orig
  sed -s "s,/usr/local/lib,%{_datadir},g" < ${i} > ${i}.orig
  sed -s "s,/usr/lib,%{_datadir},g" < ${i}.orig > ${i}
  sed -s "s,/usr/local,/usr,g" < ${i}.orig > ${i}
  rm -f ${i}.orig
done

%build
gzip -9nf slib.info

%install
mkdir -p ${RPM_BUILD_ROOT}%{_datadir}/slib
mkdir -p ${RPM_BUILD_ROOT}%{_bindir}
cp *.scm *.init *.xyz *.txt grapheps.ps Makefile ${RPM_BUILD_ROOT}%{_datadir}/slib
mkdir -p ${RPM_BUILD_ROOT}%{_infodir}
install -m644 slib.info.gz ${RPM_BUILD_ROOT}%{_infodir}
make	prefix=${RPM_BUILD_ROOT}%{prefix}/ \
	mandir=${RPM_BUILD_ROOT}%{_mandir}/ \
	infodir=${RPM_BUILD_ROOT}%{_infodir}/ \
	pinstall

echo '#! /bin/sh'			 > ${RPM_BUILD_ROOT}%{_bindir}/slib
echo SCHEME_LIBRARY_PATH=%{_datadir}/slib/ >> ${RPM_BUILD_ROOT}%{_bindir}/slib
echo export SCHEME_LIBRARY_PATH		>> ${RPM_BUILD_ROOT}%{_bindir}/slib
echo VERSION=%{version}			>> ${RPM_BUILD_ROOT}%{_bindir}/slib
echo "S48_VICINITY=\"%{_datadir}/scheme48\";export S48_VICINITY" >> ${RPM_BUILD_ROOT}%{_bindir}/slib
cat slib.sh				>> ${RPM_BUILD_ROOT}%{_bindir}/slib
chmod +x ${RPM_BUILD_ROOT}%{_bindir}/slib
%clean
rm -rf ${RPM_BUILD_ROOT}

%post
# /sbin/install-info ${RPM_BUILD_ROOT}%{_infodir}/slib.info.gz %{_infodir}/dir

# This symlink is made as in the spec file of Robert J. Meier.
if [ -L /usr/share/guile/slib ]; then
  rm /usr/share/guile/slib
  ln -s %{_datadir}/slib /usr/share/guile/slib
fi

# Rebuild catalogs for as many implementations as possible.
export PATH=$PATH:/usr/local/bin
echo PATH=${PATH}
cd %{_datadir}/slib/
make catalogs

# %postun
# if [ $1 = 0 ]; then
#   /sbin/install-info --delete %{_infodir}/slib.info.gz %{_infodir}/dir
# fi

%preun
cd %{_datadir}/slib/
rm -f srcdir.mk slib.image

%files
%defattr(-, root, root)
%{_bindir}/slib
%dir %{_datadir}/slib
%{_datadir}/slib/*.scm
%{_datadir}/slib/*.init
%{_datadir}/slib/cie1931.xyz
%{_datadir}/slib/cie1964.xyz
%{_datadir}/slib/nbs-iscc.txt
%{_datadir}/slib/saturate.txt
%{_datadir}/slib/resenecolours.txt
%{_datadir}/slib/grapheps.ps
%{_infodir}/slib.info.gz
# The Makefile is of limited utility without all the texinfo files.
%{_datadir}/slib/Makefile
%{_mandir}/man1/slib.1.gz
%doc ANNOUNCE README COPYING FAQ ChangeLog

%changelog
* Sun Sep 25 2005 Aubrey Jaffer <agj@alum.mit.edu>
- Updated from RedHat version from Jindrich Novy.

* Fri Jun 22 2005 Aubrey Jaffer  <agj@alum.mit.edu>
- slib.spec (install): Make slib executable.

* Sat Jun 18 2004 Aubrey Jaffer <agj@alum.mit.edu>
- Fixed for RPMbuild version 4.3.1
- Make slib executable.

* Thu Nov 03 2002 Aubrey Jaffer  <agj@alum.mit.edu>
- slib.spec (%post): Improved catalog-building scripts.
- Make clrnamdb.scm.

* Wed Mar 14 2001 Radey Shouman <shouman@ne.mediaone.net>
- Adapted from the spec file of R. J. Meier.

* Mon Jul 12 2000 Dr. Robert J. Meier <robert.meier@computer.org> 0.9.4-1suse
- Packaged for SuSE 6.3

* Sun May 30 2000 Aubrey Jaffer <agj @ alum.mit.edu>
- Updated content
