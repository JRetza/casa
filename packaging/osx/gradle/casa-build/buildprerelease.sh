export PATH=/opt/casa/01/libexec/qt4/bin:/Users/rpmbuild/gradle-2.3/bin/:/Users/rpmbuild/bin:/opt/casa/01/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/opt/X11/bin:/usr/texbin:/opt/local/bin
gradle -b /Users/rpmbuild/gradle/casa-build/build.gradle -PbuildType=prerelease buildAndPublish > /Users/rpmbuild/gradle/workdir/buildlogs/build.log 2>&1
mv /Users/rpmbuild/gradle/workdir/buildlogs/build.log /Users/rpmbuild/gradle/workdir/buildlogs/casa-build-prerelease-4.7.0-`row=$(tail -1 /Users/rpmbuild/gradle/casa-build/release-4_7_svn_revision_history.txt);rowArr=($row); echo ${rowArr[1]}`.log
scp /Users/rpmbuild/gradle/workdir/dmgs/prerelease/* rpmbuild@cbt-osx1011-tst:/Users/rpmbuild/gradle/workdir/dmgs/prerelease/
mv /Users/rpmbuild/gradle/workdir/dmgs/prerelease/* /Users/rpmbuild/gradle/workdir/dmgs/prerelease.bak/
