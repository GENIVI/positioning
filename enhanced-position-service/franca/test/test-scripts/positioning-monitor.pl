#! /usr/bin/perl -w
###########################################################################
# Component Name: Navit POC
# Author: Martin Schaller <martin.schaller@it-schaller.de>
#
# Copyright (C) 2012, GENIVI Alliance, Inc.
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as 
# published by the Free Software Foundation.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
###########################################################################
$| = 1;
open(MON,"dbus-monitor|");
while (<MON>) {
	if (/^method call.*;\ interface=org\.genivi\.EnhancedPositionService/) {
		$serial=$_;
		$serial=~s/.*serial=//;
		$serial=~s/\ path=.*//;
		$active=1;
		print $_;
		$serials{$serial}=1;
	} elsif (/^method return/) {
		$serial=$_;
		$serial=~s/.*reply_serial=//;
		if ($serials{$serial}) {
			delete($serials{$serial});
			$active=1;
			print $_;
		}
	} elsif (/^error /) {
		$serial=$_;
		$serial=~s/.*reply_serial=//;
		if ($serials{$serial}) {
			delete($serials{$serial});
			$active=1;
			print $_;
		}
	} elsif (/^signal.*;\ interface=org\.genivi\.EnhancedPositionService/) {
		$active=1;
		print $_;
	} elsif (/^ +/) {
		if ($active) {
			print $_;
		}
	} else {
		$active=0;
	}
}
close(MON);
