###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: Positioning
#
# Author: Marco Residori
#
# Copyright (C) 2013, XS Embedded GmbH
# 
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# @licence end@
###########################################################################

#
# Description: this is a Yocto Recipe of the 4 proof of concept contained 
#              in the positioning repository
#
# Status: Draft (Work in Progress)
#

DESCRIPTION = "Positioning"
HOMEPAGE = "http://git.projects.genivi.org/?p=lbs/positioning.git;a=summary"

LICENSE = "MPLv2"
LICENSE_${PN}-gnss= "MPLv2"
LICENSE_${PN}-sns = "MPLv2"
LICENSE_${PN}-repl = "MPLv2"
LICENSE_${PN}-enhpos = "MPLv2"

SRC_URI = "git://git.projects.genivi.org/lbs/positioning.git;protocol=git"
SRCREV = "183d134e734e157c498f6cff367d104e4c5ad670"

LIC_FILES_CHKSUM = "file://LICENSE;md5=e73ca6874051c79a99d065bc57849af5"

S = "${WORKDIR}/git"

DEPENDS = "dbus"
DEPENDS += "dlt-daemon"
#DEPENDS += "gpsd"
#DEPENDS += "qt4"

inherit cmake pkgconfig 

PACKAGES =+ "${PN}-gnss ${PN}-gnss-test ${PN}-sns ${PN}-sns-test ${PN}-repl ${PN}-repl-test ${PN}-enhpos ${PN}-enhpos-test "

RDEPENDS_${PN}-gnss-test = "${PN}-gnss"
DEPENDS_${PN}-gnss-test = "${PN}-gnss"

RDEPENDS_${PN}-sns-test = "${PN}-sns"
DEPENDS_${PN}-sns-test = "${PN}-sns"

do_configure() {
 cd ${S}/gnss-service && cmake -DWITH_DLT=OFF -DWITH_GPSD=OFF -DWITH_REPLAYER=ON -DWITH_TESTS=ON . 
 cd ${S}/sensors-service && cmake -DWITH_DLT=OFF -DWITH_REPLAYER=ON -DWITH_IPHONE=OFF -DWITH_TESTS=ON . 
 cd ${S}/log-replayer && cmake -DWITH_DLT=OFF -DWITH_TESTS=ON . 
 #cd ${S}/enhanced-position-service && cmake -DWITH_DLT=OFF -DWITH_GPSD=OFF -DWITH_REPLAYER=ON -DWITH_TESTS=ON . 
}

do_compile() {
 cd ${S}/gnss-service && make
 cd ${S}/sensors-service && make
 cd ${S}/log-replayer && make
 #cd ${S}/enhanced-position-service && make
}

do_install() {
   install -d ${D}/${bindir}
   install -d ${D}/${libdir}
   install -d ${D}/${datadir}/${PN}
   install -m 755 ${S}/log-replayer/src/log-replayer ${D}/${bindir}
   install -m 755 ${S}/log-replayer/test/test-log-replayer ${D}/${bindir}
   install -m 644 ${S}/log-replayer/logs/*.log ${D}${datadir}/${PN}
   install -m 755 ${S}/gnss-service/src/*.so ${D}/${libdir}
   install -m 755 ${S}/gnss-service/test/gnss-service-client ${D}/${bindir}
   install -m 755 ${S}/gnss-service/test/compliance-test/gnss-service-compliance-test ${D}/${bindir}
   install -m 755 ${S}/sensors-service/src/*.so ${D}/${libdir}
   install -m 755 ${S}/sensors-service/test/sensors-service-client ${D}/${bindir}
   #install -m 755 ${S}/enhanced-position-service/src/position-daemon ${D}/${libdir}
   #install -m 755 ${S}/enhanced-position-service/test/test-client ${D}/${bindir}
   #install -m 755 ${S}/enhanced-position-service/test/compliance-test/enhanced-position-service-compliance-test ${D}/${bindir}
}

#select which of the files in ${bindir} and ${libdir} belong to which package
FILES_${PN}-gnss = "${libdir}/libgnss-service*.so "
FILES_${PN}-gnss-test = "${bindir}/gnss-service-client \
                         ${bindir}/gnss-service-compliance-test "    

FILES_${PN}-sns = "${libdir}/libsensors-service*.so "
FILES_${PN}-sns-test = "${bindir}/sensors-service-client "    

FILES_${PN}-repl = "${bindir}/log-replayer "
FILES_${PN}-repl-test = "${bindir}/test-log-replayer "
FILES_${PN}-repl-test = "${D}${datadir}/${PN} "

#FILES_${PN}-enhpos = "${bindir}/enhanced-position-service/src/position-daemon "
#FILES_${PN}-enhpos = "${bindir}/enhanced-position-service/test/test-client "
#FILES_${PN}-enhpos = "${bindir}/enhanced-position-service/test/compliance-test/enhanced-position-service-compliance-test "

BBCLASSEXTEND = "native"