#! /usr/bin/env python3 

# Convert a NMEA Log to the revised GENIVI positioning log format
# Based on my initial NMEA decoder: https://gist.github.com/huirad/d7a511f78ddba8537bd6
# Helmut.3.Schmidt@continental-corporation.com

###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: LogReplayer
# Author: Helmut Schmidt <Helmut.3.Schmidt@continental-corporation.com>
#
# Copyright (C) 2013, Continental Automotive GmbH
# 
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# # @licence end@
###########################################################################


#####################################
# Imports
#####################################

import sys
import re
import argparse
import math


###########################################################
# Configuration Parameters, can also be set by command line
###########################################################

#debug flag
c_debug = False

#receiver type
#Used to determine when a sequence of NMEA sentences is complete
#This can be set by command line
#Supported values: 
# "geko301"   for the Garmin geko301
# "z205"      for the Becker PND Z205 which contains a SiRFStarIII Chipset
c_receiver_type = "geko301"

#(rear) track width in m
#default value: 1.525m (VW Golf)
c_track = 1.525

#wheel (rolling) circumference
#default value: 1.92m (VW Golf)
c_wheel_circ = 1.92

#wheel ticks per revolution
#default value: 96
c_wheel_tick_rev = 96

#####################################
# Data structure definitions
#####################################

#empty dictionary for the content of a GPRMC sentence
empty_rmc = {
    "year"      : None,
    "month"     : None,
    "day"       : None,
    "hour"      : None,
    "minute"    : None,
    "second"    : None,
    "lat"       : None,
    "lon"       : None,
    "course"    : None,
    "speed"     : None
}    

#empty dictionary for the content of a GPGGA sentence
empty_gga = {
    "hour"      : None,
    "minute"    : None,
    "second"    : None,
    "lat"       : None,
    "lon"       : None,
    "quality"   : None,
    "sat_used"  : None,
    "hdop"      : None,
    "alt_msl"   : None,
    "geoid_sep" : None,
    "dgps_age"  : None
}

#empty dictionary for the content of a GPGSV sentence
empty_gsv = {
    "sent_tot"  : None,
    "sent_num"  : None,
    "sat_view"  : None,
    "sat1_prn"  : None,
    "sat1_ele"  : None,
    "sat1_azi"  : None,
    "sat1_snr"  : None,
    "sat2_prn"  : None,
    "sat2_ele"  : None,
    "sat2_azi"  : None,
    "sat2_snr"  : None,
    "sat3_prn"  : None,
    "sat3_ele"  : None,
    "sat3_azi"  : None,
    "sat3_snr"  : None,
    "sat4_prn"  : None,
    "sat4_ele"  : None,
    "sat4_azi"  : None,
    "sat4_snr"  : None    
}


#####################################
# Function definitions to read NMEA
#####################################

#Verify if NMEA checksum of a sentence is correct
def isChecksumCorrect(line):
  checksum = 0
  for char in line[1:-3]:
    checksum = checksum ^ ord(char)
  return (checksum == int(line[-2:],16))

  
#Read a GPRMC sentence to a dictionary
# Format: $GPRMC,<time as hhmmss>,<status A=active>,<lat in ddmm.mmm>,<N/S>,<lon in dddmm.mmm>,<E/W>,<speed in knots>,<course in degrees>,<date as ddmmyy>,<magnetic variation in degrees>,<E/W>*<checksum>
# Example: $GPRMC,165746,A,4901.5940,N,01203.9163,E,0.0,131.9,110410,1.6,E,A*1D
def readRMC (fields):
  ret = empty_rmc.copy()
  if ((fields[2] == "A") or (fields[2] == "a")):
    #time
    time_str = fields[1]
    if len(time_str)>=6:
      ret["hour"]   = int(time_str[0:2])
      ret["minute"] = int(time_str[2:4])
      ret["second"] = int(time_str[4:6])
    #latitude
    lat_str = fields[3]
    if len(lat_str)>=4:
      ret["lat"] = int(lat_str[0:2])+float(lat_str[2:])/60.0
    #latitude sign
    if ((fields[4] == "W") or (fields[4] == "w")):
      ret["lat"] = - ret["lat"]
    #longitude
    lon_str = fields[5]
    if len(lon_str)>=4:
      ret["lon"] = int(lon_str[0:3])+float(lon_str[3:])/60.0
    #longitude sign
    if ((fields[6] == "S") or (fields[6] == "s")):
      ret["lon"] = - ret["lon"]
    #speed in m/s
    speed_str = fields[7]
    if len(speed_str)>0:
      ret["speed"] = float(speed_str)*1.852/3.6
    #course in degrees
    course_str = fields[8]
    if len(course_str)>0:
      ret["course"] = float(course_str)
    #date
    date_str = fields[9]
    if len(date_str)==6:
      ret["year"]   = int(date_str[4:6])+2000
      ret["month"]  = int(date_str[2:4])
      ret["day"]    = int(date_str[0:2])
  return ret


#Read a GPGGA sentence to a dictionary
# Format: $GPGGA,<time as hhmmss>,<lat in ddmm.mmm>,<N/S>,<lon in dddmm.mmm>,<E/W>,<quality>,<sat_used>,<hdop>,<alt_msl>,<m>,<geoid_sep>,<m>,<dgps_age>*<checksum>
# Example: $GPGGA,165744,4901.5940,N,01203.9163,E,1,07,1.0,336.7,M,46.6,M,,*4D  
def readGGA (fields):
  ret = empty_gga.copy()
  if ((fields[6] == "1") or (fields[6] == "2")):
    #time
    time_str = fields[1]
    if len(time_str)==6:
      ret["hour"]   = int(time_str[0:2])
      ret["minute"] = int(time_str[2:4])
      ret["second"] = int(time_str[4:6])
    #latitude
    lat_str = fields[2]
    if len(lat_str)>=4:
      ret["lat"] = int(lat_str[0:2])+float(lat_str[2:])/60.0
    #latitude sign
    if ((fields[3] == "W") or (fields[3] == "w")):
      ret["lat"] = - ret["lat"]
    #longitude
    lon_str = fields[4]
    if len(lon_str)>=4:
      ret["lon"] = int(lon_str[0:3])+float(lon_str[3:])/60.0
    #longitude sign
    if ((fields[5] == "S") or (fields[5] == "s")):
      ret["lon"] = - ret["lon"]
    #sat_used
    sat_used_str = fields[7]
    if len(sat_used_str)>0:
      ret["sat_used"] = int(sat_used_str)
    #hdop
    hdop_str = fields[8]
    if len(hdop_str)>0:
      ret["hdop"] = float(hdop_str)
    #alt_msl
    alt_msl_str = fields[9]
    if len(alt_msl_str)>0:
      ret["alt_msl"] = float(alt_msl_str)
    #geoid_sep
    geoid_sep_str = fields[11]
    if len(geoid_sep_str)>0:
      ret["geoid_sep"] = float(geoid_sep_str)
  return ret

#Read a GPGSV sentence to a dictionary
# Format: $GPGSV,<total # of sentences>,<sentence #>,<# sat in view>,<SAT1 PRN>,<SAT1 Elevation>,<SAT1 azimuth>,<SAT1 SNR>,...<SAT4 PRN>,<SAT4 Elevation>,<SAT4 azimuth>,<SAT4 SNR>*<checksum>
# Example: $GPGSV,3,2,10,17,21,125,00,18,24,310,41,24,00,292,00,26,23,308,43*74
def readGSV (fields):
  ret = empty_gsv.copy()
  #sent_tot
  sent_tot_str = fields[1]
  if len(sent_tot_str)>0:
    ret["sent_tot"] = int(sent_tot_str)
  #sent_num
  sent_num_str = fields[2]
  if len(sent_num_str)>0:
    ret["sent_num"] = int(sent_num_str)
  #sent_tot
  sat_view_str = fields[3]
  if len(sat_view_str)>0:
    ret["sat_view"] = int(sat_view_str)

  #sat1_prn
  if len(fields) >= 8:
    sat1_prn_str = fields[4]
    if len(sat1_prn_str)>0:
      ret["sat1_prn"] = int(sat1_prn_str)
      #fo.write("sat1_prn={0:02d}|".format(ret["sat1_prn"]))
    #sat1_ele
    sat1_ele_str = fields[5]
    if len(sat1_ele_str)>0:
      ret["sat1_ele"] = int(sat1_ele_str)
      #fo.write("sat1_ele={0:.02f}|".format(ret["sat1_ele"]))
    #sat1_azi
    sat1_azi_str = fields[6]
    if len(sat1_azi_str)>0:
      ret["sat1_azi"] = int(sat1_azi_str)
      #fo.write("sat1_azi={0:.02f}|".format(ret["sat1_azi"]))
    #sat1_snr
    sat1_snr_str = fields[7]
    if len(sat1_snr_str)>0:
      ret["sat1_snr"] = int(sat1_snr_str)
      #fo.write("sat1_snr={0:02d}\n".format(ret["sat1_snr"]))

  if len(fields) >= 12:    
    #sat2_prn
    sat2_prn_str = fields[8]
    if len(sat2_prn_str)>0:
      ret["sat2_prn"] = int(sat2_prn_str)
    #sat2_ele
    sat2_ele_str = fields[9]
    if len(sat2_ele_str)>0:
      ret["sat2_ele"] = int(sat2_ele_str)
    #sat2_azi
    sat2_azi_str = fields[10]
    if len(sat2_azi_str)>0:
      ret["sat2_azi"] = int(sat2_azi_str)
    #sat2_snr
    sat2_snr_str = fields[11]
    if len(sat2_snr_str)>0:
      ret["sat2_snr"] = int(sat2_snr_str)

  if len(fields) >= 16:
    #sat3_prn
    sat3_prn_str = fields[12]
    if len(sat3_prn_str)>0:
      ret["sat3_prn"] = int(sat3_prn_str)
    #sat3_ele
    sat3_ele_str = fields[13]
    if len(sat3_ele_str)>0:
      ret["sat3_ele"] = int(sat3_ele_str)
    #sat3_azi
    sat3_azi_str = fields[14]
    if len(sat3_azi_str)>0:
      ret["sat3_azi"] = int(sat3_azi_str)
    #sat3_snr
    sat3_snr_str = fields[15]
    if len(sat3_snr_str)>0:
      ret["sat3_snr"] = int(sat3_snr_str)

  if len(fields) >= 20:
    #sat4_prn
    sat4_prn_str = fields[16]
    if len(sat4_prn_str)>0:
      ret["sat4_prn"] = int(sat4_prn_str)
    #sat4_ele
    sat4_ele_str = fields[17]
    if len(sat4_ele_str)>0:
      ret["sat4_ele"] = int(sat4_ele_str)
    #sat4_azi
    sat4_azi_str = fields[18]
    if len(sat4_azi_str)>0:
      ret["sat4_azi"] = int(sat4_azi_str)
    #sat4_snr
    sat4_snr_str = fields[19]
    if len(sat4_snr_str)>0:
      ret["sat4_snr"] = int(sat4_snr_str)

  return ret

##################################################################
# Function definitions to write GENIVI Positioning Log - GNSS part
##################################################################

#write GVGNSP - GNSS (simple) Position [take data from GGA sentence]
def writeGVGNSP (file, gga, rmc, timestamp):
  valid = 0
  file.write("{0:09d},0$GVGNSP,{0:09d},".format(timestamp))
  if gga["lat"] is not None:
    valid |= 0x01
    file.write("{0:.5f},".format(gga["lat"]))
  elif rmc["lat"] is not None:
    valid |= 0x01
    file.write("{0:.5f},".format(rmc["lat"]))
  else:
    file.write("0,")
  if gga["lon"] is not None:
    valid |= 0x02
    file.write("{0:.5f},".format(gga["lon"]))
  elif rmc["lon"] is not None:
    valid |= 0x02
    file.write("{0:.5f},".format(rmc["lon"]))
  else:
    file.write("0,")
  if gga["alt_msl"] is not None:
    valid |= 0x04
    file.write("{0:.1f},".format(gga["alt_msl"]))
  else:
    file.write("0,")
  file.write("0X{0:02X}".format(valid))
  file.write("\n")
  return

#write GVGNSC - GNSS (simple) Course [take data from RMC sentence]
def writeGVGNSC (file, gga, rmc, timestamp):
  valid = 0
  file.write("{0:09d},0$GVGNSC,{0:09d},".format(timestamp))
  if rmc["speed"] is not None:
    valid |= 0x01
    file.write("{0:.2f},".format(rmc["speed"]))
  else:
    file.write("0,")
  #climb is not available
  file.write("0,")
  if rmc["course"] is not None:
    valid |= 0x04
    file.write("{0:.2f},".format(rmc["course"]))
  else:
    file.write("0,")
  file.write("0X{0:02X}".format(valid))
  file.write("\n")

#write GVGNSAC - GNSS (extended) Accuracy [as first step take data from GGA sentence, later use GSA]
def writeGVGNSAC (file, gga, rmc, timestamp):
  valid = 0
  file.write("{0:09d},0$GVGNSAC,{0:09d},".format(timestamp))
  #pdop is not available
  file.write("0,")
  if gga["hdop"] is not None:
    valid |= 0x02
    file.write("{0:.1f},".format(gga["hdop"]))
  else:
    file.write("0,")
  #vdop is not available
  file.write("0,")
  if gga["sat_used"] is not None:
    valid |= 0x08
    file.write("{0:02d},".format(gga["sat_used"]))
  else:
    file.write("0,")
  #trackedSatellites is not available
  file.write("0,")
  #visibleSatellites is not available
  file.write("0,")
  #sigmaLatitude is not available
  file.write("0,")
  #sigmaLongitude is not available
  file.write("0,")
  #sigmaAltitude is not available
  file.write("0,")
  #fix status
  #HACK: set 3D fix if there is a valid position
  if rmc["lat"] is not None:
    valid |= 0x200
    file.write("2,") #yes, the enum value 2 stands for a 3D fix
  else:
    file.write("0,")
  #fix type
  #HACK: set single frequency if there is a valid position
  if rmc["lat"] is not None:
    valid |= 0x400
    file.write("0X{0:08X},".format(0x00000001))#GNSS_FIX_TYPE_SINGLE_FREQUENCY      = 0x00000001
  else:
    file.write("0,")
  file.write("0X{0:03X}".format(valid))
  file.write("\n")

#GVGNSSAT - GNSS (extended) Satellite details [take data from GSV and GSA sentence]
#  TODO: add support for status bits including extract list of used satellites from GSA
#  This is a nice example for buffered data
def writeGVGNSSAT (file, gsv_arr, timestamp):
  countdown = 0
  if (gsv_arr[0]["sent_tot"] is not None) and (gsv_arr[0]["sent_num"] is not None) and (gsv_arr[0]["sat_view"] is not None) and \
     (gsv_arr[0]["sent_tot"] > 0)         and (gsv_arr[0]["sent_num"] > 0)         and (gsv_arr[0]["sat_view"] > 0):
    countdown = gsv_arr[0]["sat_view"]
    
    #repeat the filellowing sequence for each satellite - ugly :-(
    #gsv_arr[0], sat 1
    countdown = countdown-1
    if countdown >= 0:
      valid = 0x01
      file.write("{0:09d},{1:02d}$GVGNSSAT,{0:09d},1,".format(timestamp,countdown))
      if gsv_arr[0]["sat1_prn"] is not None:
        valid |= 0x02
        file.write("{0:02d},".format(gsv_arr[0]["sat1_prn"]))
      else:
        file.write("0,")
      if gsv_arr[0]["sat1_azi"] is not None:
        valid |= 0x04
        file.write("{0:03d},".format(gsv_arr[0]["sat1_azi"]))
      else:
        file.write("0,")
      if gsv_arr[0]["sat1_ele"] is not None:
        valid |= 0x08
        file.write("{0:02d},".format(gsv_arr[0]["sat1_ele"]))
      else:
        file.write("0,")
      if gsv_arr[0]["sat1_snr"] is not None:
        valid |= 0x10
        file.write("{0:02d},".format(gsv_arr[0]["sat1_snr"]))
      else:
        file.write("0,")
      #status bits not yet supported
      file.write("0X00,0X{0:02X}".format(valid))
      file.write("\n")
    #gsv_arr[0], sat 2
    countdown = countdown-1
    if countdown >= 0:
      valid = 0x01
      file.write("{0:09d},{1:02d}$GVGNSSAT,{0:09d},1,".format(timestamp,countdown))
      if gsv_arr[0]["sat2_prn"] is not None:
        valid |= 0x02
        file.write("{0:02d},".format(gsv_arr[0]["sat2_prn"]))
      else:
        file.write("0,")
      if gsv_arr[0]["sat2_azi"] is not None:
        valid |= 0x04
        file.write("{0:03d},".format(gsv_arr[0]["sat2_azi"]))
      else:
        file.write("0,")
      if gsv_arr[0]["sat2_ele"] is not None:
        valid |= 0x08
        file.write("{0:02d},".format(gsv_arr[0]["sat2_ele"]))
      else:
        file.write("0,")
      if gsv_arr[0]["sat2_snr"] is not None:
        valid |= 0x10
        file.write("{0:02d},".format(gsv_arr[0]["sat2_snr"]))
      else:
        file.write("0,")
      #status bits not yet supported
      file.write("0X00,0X{0:02X}".format(valid))
      file.write("\n")
    #gsv_arr[0], sat 3
    countdown = countdown-1
    if countdown >= 0:
      valid = 0x01
      file.write("{0:09d},{1:02d}$GVGNSSAT,{0:09d},1,".format(timestamp,countdown))
      if gsv_arr[0]["sat3_prn"] is not None:
        valid |= 0x02
        file.write("{0:02d},".format(gsv_arr[0]["sat3_prn"]))
      else:
        file.write("0,")
      if gsv_arr[0]["sat3_azi"] is not None:
        valid |= 0x04
        file.write("{0:03d},".format(gsv_arr[0]["sat3_azi"]))
      else:
        file.write("0,")
      if gsv_arr[0]["sat3_ele"] is not None:
        valid |= 0x08
        file.write("{0:02d},".format(gsv_arr[0]["sat3_ele"]))
      else:
        file.write("0,")
      if gsv_arr[0]["sat3_snr"] is not None:
        valid |= 0x10
        file.write("{0:02d},".format(gsv_arr[0]["sat3_snr"]))
      else:
        file.write("0,")
      #status bits not yet supported
      file.write("0X00,0X{0:02X}".format(valid))
      file.write("\n")
    #gsv_arr[0], sat 4
    countdown = countdown-1
    if countdown >= 0:
      valid = 0x01
      file.write("{0:09d},{1:02d}$GVGNSSAT,{0:09d},1,".format(timestamp,countdown))
      if gsv_arr[0]["sat4_prn"] is not None:
        valid |= 0x02
        file.write("{0:02d},".format(gsv_arr[0]["sat4_prn"]))
      else:
        file.write("0,")
      if gsv_arr[0]["sat4_azi"] is not None:
        valid |= 0x04
        file.write("{0:03d},".format(gsv_arr[0]["sat4_azi"]))
      else:
        file.write("0,")
      if gsv_arr[0]["sat4_ele"] is not None:
        valid |= 0x08
        file.write("{0:02d},".format(gsv_arr[0]["sat4_ele"]))
      else:
        file.write("0,")
      if gsv_arr[0]["sat4_snr"] is not None:
        valid |= 0x10
        file.write("{0:02d},".format(gsv_arr[0]["sat4_snr"]))
      else:
        file.write("0,")
      #status bits not yet supported
      file.write("0X00,0X{0:02X}".format(valid))
      file.write("\n")          
    #gsv_arr[1], sat 1
    countdown = countdown-1
    if countdown >= 0:
      valid = 0x01
      file.write("{0:09d},{1:02d}$GVGNSSAT,{0:09d},1,".format(timestamp,countdown))
      if gsv_arr[1]["sat1_prn"] is not None:
        valid |= 0x02
        file.write("{0:02d},".format(gsv_arr[1]["sat1_prn"]))
      else:
        file.write("0,")
      if gsv_arr[1]["sat1_azi"] is not None:
        valid |= 0x04
        file.write("{0:03d},".format(gsv_arr[1]["sat1_azi"]))
      else:
        file.write("0,")
      if gsv_arr[1]["sat1_ele"] is not None:
        valid |= 0x08
        file.write("{0:02d},".format(gsv_arr[1]["sat1_ele"]))
      else:
        file.write("0,")
      if gsv_arr[1]["sat1_snr"] is not None:
        valid |= 0x10
        file.write("{0:02d},".format(gsv_arr[1]["sat1_snr"]))
      else:
        file.write("0,")
      #status bits not yet supported
      file.write("0X00,0X{0:02X}".format(valid))
      file.write("\n")
    #gsv_arr[1], sat 2
    countdown = countdown-1
    if countdown >= 0:
      valid = 0x01
      file.write("{0:09d},{1:02d}$GVGNSSAT,{0:09d},1,".format(timestamp,countdown))
      if gsv_arr[1]["sat2_prn"] is not None:
        valid |= 0x02
        file.write("{0:02d},".format(gsv_arr[1]["sat2_prn"]))
      else:
        file.write("0,")
      if gsv_arr[1]["sat2_azi"] is not None:
        valid |= 0x04
        file.write("{0:03d},".format(gsv_arr[1]["sat2_azi"]))
      else:
        file.write("0,")
      if gsv_arr[1]["sat2_ele"] is not None:
        valid |= 0x08
        file.write("{0:02d},".format(gsv_arr[1]["sat2_ele"]))
      else:
        file.write("0,")
      if gsv_arr[1]["sat2_snr"] is not None:
        valid |= 0x10
        file.write("{0:02d},".format(gsv_arr[1]["sat2_snr"]))
      else:
        file.write("0,")
      #status bits not yet supported
      file.write("0X00,0X{0:02X}".format(valid))
      file.write("\n")
    #gsv_arr[1], sat 3
    countdown = countdown-1
    if countdown >= 0:
      valid = 0x01
      file.write("{0:09d},{1:02d}$GVGNSSAT,{0:09d},1,".format(timestamp,countdown))
      if gsv_arr[1]["sat3_prn"] is not None:
        valid |= 0x02
        file.write("{0:02d},".format(gsv_arr[1]["sat3_prn"]))
      else:
        file.write("0,")
      if gsv_arr[1]["sat3_azi"] is not None:
        valid |= 0x04
        file.write("{0:03d},".format(gsv_arr[1]["sat3_azi"]))
      else:
        file.write("0,")
      if gsv_arr[1]["sat3_ele"] is not None:
        valid |= 0x08
        file.write("{0:02d},".format(gsv_arr[1]["sat3_ele"]))
      else:
        file.write("0,")
      if gsv_arr[1]["sat3_snr"] is not None:
        valid |= 0x10
        file.write("{0:02d},".format(gsv_arr[1]["sat3_snr"]))
      else:
        file.write("0,")
      #status bits not yet supported
      file.write("0X00,0X{0:02X}".format(valid))
      file.write("\n")
    #gsv_arr[1], sat 4
    countdown = countdown-1
    if countdown >= 0:
      valid = 0x01
      file.write("{0:09d},{1:02d}$GVGNSSAT,{0:09d},1,".format(timestamp,countdown))
      if gsv_arr[1]["sat4_prn"] is not None:
        valid |= 0x02
        file.write("{0:02d},".format(gsv_arr[1]["sat4_prn"]))
      else:
        file.write("0,")
      if gsv_arr[1]["sat4_azi"] is not None:
        valid |= 0x04
        file.write("{0:03d},".format(gsv_arr[1]["sat4_azi"]))
      else:
        file.write("0,")
      if gsv_arr[1]["sat4_ele"] is not None:
        valid |= 0x08
        file.write("{0:02d},".format(gsv_arr[1]["sat4_ele"]))
      else:
        file.write("0,")
      if gsv_arr[1]["sat4_snr"] is not None:
        valid |= 0x10
        file.write("{0:02d},".format(gsv_arr[1]["sat4_snr"]))
      else:
        file.write("0,")
      #status bits not yet supported
      file.write("0X00,0X{0:02X}".format(valid))
      file.write("\n")   
    #gsv_arr[2], sat 1
    countdown = countdown-1
    if countdown >= 0:
      valid = 0x01
      file.write("{0:09d},{1:02d}$GVGNSSAT,{0:09d},1,".format(timestamp,countdown))
      if gsv_arr[2]["sat1_prn"] is not None:
        valid |= 0x02
        file.write("{0:02d},".format(gsv_arr[2]["sat1_prn"]))
      else:
        file.write("0,")
      if gsv_arr[2]["sat1_azi"] is not None:
        valid |= 0x04
        file.write("{0:03d},".format(gsv_arr[2]["sat1_azi"]))
      else:
        file.write("0,")
      if gsv_arr[2]["sat1_ele"] is not None:
        valid |= 0x08
        file.write("{0:02d},".format(gsv_arr[2]["sat1_ele"]))
      else:
        file.write("0,")
      if gsv_arr[2]["sat1_snr"] is not None:
        valid |= 0x10
        file.write("{0:02d},".format(gsv_arr[2]["sat1_snr"]))
      else:
        file.write("0,")
      #status bits not yet supported
      file.write("0X00,0X{0:02X}".format(valid))
      file.write("\n")
    #gsv_arr[2], sat 2
    countdown = countdown-1
    if countdown >= 0:
      valid = 0x01
      file.write("{0:09d},{1:02d}$GVGNSSAT,{0:09d},1,".format(timestamp,countdown))
      if gsv_arr[2]["sat2_prn"] is not None:
        valid |= 0x02
        file.write("{0:02d},".format(gsv_arr[2]["sat2_prn"]))
      else:
        file.write("0,")
      if gsv_arr[2]["sat2_azi"] is not None:
        valid |= 0x04
        file.write("{0:03d},".format(gsv_arr[2]["sat2_azi"]))
      else:
        file.write("0,")
      if gsv_arr[2]["sat2_ele"] is not None:
        valid |= 0x08
        file.write("{0:02d},".format(gsv_arr[2]["sat2_ele"]))
      else:
        file.write("0,")
      if gsv_arr[2]["sat2_snr"] is not None:
        valid |= 0x10
        file.write("{0:02d},".format(gsv_arr[2]["sat2_snr"]))
      else:
        file.write("0,")
      #status bits not yet supported
      file.write("0X00,0X{0:02X}".format(valid))
      file.write("\n")
    #gsv_arr[2], sat 3
    countdown = countdown-1
    if countdown >= 0:
      valid = 0x01
      file.write("{0:09d},{1:02d}$GVGNSSAT,{0:09d},1,".format(timestamp,countdown))
      if gsv_arr[2]["sat3_prn"] is not None:
        valid |= 0x02
        file.write("{0:02d},".format(gsv_arr[2]["sat3_prn"]))
      else:
        file.write("0,")
      if gsv_arr[2]["sat3_azi"] is not None:
        valid |= 0x04
        file.write("{0:03d},".format(gsv_arr[2]["sat3_azi"]))
      else:
        file.write("0,")
      if gsv_arr[2]["sat3_ele"] is not None:
        valid |= 0x08
        file.write("{0:02d},".format(gsv_arr[2]["sat3_ele"]))
      else:
        file.write("0,")
      if gsv_arr[2]["sat3_snr"] is not None:
        valid |= 0x10
        file.write("{0:02d},".format(gsv_arr[2]["sat3_snr"]))
      else:
        file.write("0,")
      #status bits not yet supported
      file.write("0X00,0X{0:02X}".format(valid))
      file.write("\n")
    #gsv_arr[2], sat 4
    countdown = countdown-1
    if countdown >= 0:
      valid = 0x01
      file.write("{0:09d},{1:02d}$GVGNSSAT,{0:09d},1,".format(timestamp,countdown))
      if gsv_arr[2]["sat4_prn"] is not None:
        valid |= 0x02
        file.write("{0:02d},".format(gsv_arr[2]["sat4_prn"]))
      else:
        file.write("0,")
      if gsv_arr[2]["sat4_azi"] is not None:
        valid |= 0x04
        file.write("{0:03d},".format(gsv_arr[2]["sat4_azi"]))
      else:
        file.write("0,")
      if gsv_arr[2]["sat4_ele"] is not None:
        valid |= 0x08
        file.write("{0:02d},".format(gsv_arr[2]["sat4_ele"]))
      else:
        file.write("0,")
      if gsv_arr[2]["sat4_snr"] is not None:
        valid |= 0x10
        file.write("{0:02d},".format(gsv_arr[2]["sat4_snr"]))
      else:
        file.write("0,")
      #status bits not yet supported
      file.write("0X00,0X{0:02X}".format(valid))
      file.write("\n")   

##################################################################
# Function definitions to write GENIVI Positioning Log - SNS part
##################################################################

#write GVSNSVSP: Vehicle Speed
def writeGVSNSVSP (file, rmc, timestamp):
  valid = 0
  file.write("{0:09d},0$GVSNSVSP,{0:09d},".format(timestamp))
  if rmc["speed"] is not None:
    valid |= 0x01
    file.write("{0:.2f},".format(rmc["speed"]))
  else:
    file.write("0,")
  file.write("0X{0:02X}".format(valid))
  file.write("\n")

#write GVSNSGYR: Gyroscope Data
def writeGVSNSGYR (file, rmc, prev_rmc, timestamp, prev_timestamp):
  valid = 0
  file.write("{0:09d},0$GVSNSGYR,{0:09d},".format(timestamp))
  #yaw rate is delta_heading / delta_time
  if (rmc["course"] is not None) and (prev_rmc["course"] is not None) and (timestamp > prev_timestamp):
    valid |= 0x01
    delta_heading = - (rmc["course"] - prev_rmc["course"]) #course is clockwise, yaw rate is counter clock wise
    # normalize to -180 ..180
    delta_heading = delta_heading % 360.0 #normalizes to 0..359.9
    if delta_heading > 180.0:
      delta_heading = delta_heading - 360.0   
    delta_time = (timestamp - prev_timestamp) / 1000.0
    file.write("{0:.2f},".format(delta_heading/delta_time))
  else:
    file.write("0,")
  #pitchRate is not available
  file.write("0,")
  #rollRate is not available
  file.write("0,")
  #temperature is not available
  file.write("0,")    
  file.write("0X{0:02X}".format(valid))
  file.write("\n")

#write GVSNSWHE: Wheel tick data Data (only rear right and left wheels)
def writeGVSNSWHE (file, rmc, prev_rmc, timestamp, prev_timestamp):
  #this is the only sentence without valid bits
  file.write("{0:09d},0$GVSNSWHE,{0:09d},".format(timestamp))
  #yaw rate is delta_heading / delta_time
  if (rmc["speed"] is not None) and (rmc["course"] is not None) and (prev_rmc["course"] is not None) and (timestamp > prev_timestamp):
    delta_heading = - (rmc["course"] - prev_rmc["course"]) #course is clockwise, yaw rate is counter clock wise
    # normalize to -180 ..180
    delta_heading = delta_heading % 360.0 #normalizes to 0..359.9
    if delta_heading > 180.0:
      delta_heading = delta_heading - 360.0   
    delta_time = (timestamp - prev_timestamp) / 1000.0
    #wheel DR formulae:
    #heading change = (ticks right - ticks left) * distance per pulse / track 
    #speed = (ticks right - ticks left)/2 *distance per pulse / delta time
    delta_heading_rad = delta_heading * math.pi / 180 #heading change is needed in radians
    DPP = c_wheel_circ / c_wheel_tick_rev #distance per pulse
    avg_ticks = rmc["speed"] * delta_time / DPP
    delta_ticks = delta_heading_rad * c_track / DPP #non-integer
    ticks_left  = avg_ticks - (delta_ticks / 2.0) #non-integer
    ticks_right = avg_ticks + (delta_ticks / 2.0) #non-integer
    ticks_left_int  = int(ticks_left)
    ticks_right_int = int(ticks_right)
    #TODO: store the non-integer fractions for the next update
    file.write("{0:03d},{1:03d},0,0,0,0,0,0,0X0001,0X03".format(ticks_left_int, ticks_right_int))
  else:
    file.write("0,")
  #front left wheel is not available
  file.write("0,0,")
  #front right wheel is not available
  file.write("0,0")
  file.write("\n")
  #debug
  if c_debug and (rmc["speed"] is not None) and (rmc["course"] is not None) and (prev_rmc["course"] is not None) and (timestamp > prev_timestamp):
    file.write("#writeGVSNSWHE: delta_time:{0:f} delta_heading:{1:f} delta_heading_rad:{2:f} DPP:{3:f} avg_ticks:{4:f} delta_ticks:{5:f}\n".format(delta_time, delta_heading, delta_heading_rad, DPP, avg_ticks, delta_ticks))



#####################################
# Global variables
#####################################

#trigger flag for writing the genivi log
#should be set to true when the NMEA sentence completing the cycle is sent
#For the garmin geko301, it is the GPRTE sentence
#For most SiRF GPS receivers this is the GPRMC sentence
#For most u-blox receivers, GPRMC however is sent at the start of the cycle
#So you have to adapt the moment when this flag is set to your actual NMEA log / GPS receiver
g_write_genivi_log = False


#Buffers for decoded NMEA messages
g_rmc = empty_rmc.copy()
g_prev_rmc = empty_rmc.copy()
g_gga = empty_gga.copy()
g_prev_gga = empty_gga.copy()
g_gsv = empty_gsv.copy()
g_gsv_arr = [empty_gsv.copy() for i in range(3)]

#GENIVI timestamp - generated from NMEA UTC time
g_timestamp = 0
g_prev_timestamp = 0
  
  
#####################################
# Main program
#####################################
  
  

# 
# Process command line arguments
#

parser = argparse.ArgumentParser()
parser.add_argument("nmea_log", help="name of the NMEA file which is read as input")
parser.add_argument("genivi_log", help="name of the GENIVI positioning log file which is written as output")
parser.add_argument("--receiver_type", default=c_receiver_type, help="GPS receiver type. Supported values: geko301 for Garmin Geko301 and z205 for Becker PND Z205")
parser.add_argument("--track", default=c_track, type=float, help="(Rear) track width of the vehicle in m")
parser.add_argument("--wheel_circ", default=c_wheel_circ, type=float, help="Wheel rolling radius in m")
parser.add_argument("--wheel_tick_rev", default=c_wheel_tick_rev, type=int, help="Number of wheel ticks per revolution")
parser.add_argument("--SNS", action='count', help="'derive' SNS data from NMEA")
parser.add_argument("--debug", action='count', help="print additinal debug info to output file")
args = parser.parse_args()
  
if len(sys.argv)>2:
  infile = args.nmea_log
  outfile = args.genivi_log
  c_receiver_type = args.receiver_type
  c_SNS = args.SNS  
  c_track = args.track
  c_wheel_circ = args.wheel_circ
  c_wheel_tick_rev = args.wheel_tick_rev
  c_debug = args.debug
else:
  parser.print_help()
  sys.exit("")
    
#print (infile)
#print (outfile)

try:
  fi = open (infile, "r") #"r" is default, so it cout be left away
except:
  sys.exit("Error: Cannot open "+infile+" for reading")

try:
  fo = open (outfile, "w")
except:
  sys.exit("Error: Cannot open "+outfile+" for writing")

fo.write("#Log file generated from the nmea2gvl.py converter\n")
fo.write("#Original NMEA file: "+infile+"\n")
fo.write("#Receiver type: "+c_receiver_type+"\n")
if (c_SNS):
  fo.write("#Rear track width: {0:.3f}m\n".format(c_track))
  fo.write("#Wheel rolling cirumference: {0:.3f}m\n".format(c_wheel_circ))
  fo.write("#Wheel ticks per revolution: {0:d}\n".format(c_wheel_tick_rev))
fo.write("#By courtesy of Continental Automotive\n")
fo.write("0,0$GVGNSVER,2,0,0\n")
fo.write("0,0$GVSNSVER,2,0,0\n")


#
# iterate over NMEA input file
#



for line in fi:
  line = line.rstrip() #remove \r\n at end of string
  if not (isChecksumCorrect(line)):
    print("Wrong checksum in "+line)
  else:
    #split by "," and "*", so a simple 'line.split(",")' wont't work
    #fields = line.split(",")
    fields = re.split('[,*]', line)
    if (len(fields) > 12) and (fields[0] == "$GPRMC"):
      g_rmc = readRMC (fields)
      fo.write("#"+line+"\n")
      #g_write_genivi_log = True
    if (len(fields) > 14) and (fields[0] == "$GPGGA"):        
      g_gga = readGGA (fields)
      fo.write("#"+line+"\n")
    if (len(fields) > 3) and (fields[0] == "$GPGSV"):        
      g_gsv = readGSV (fields)
      fo.write("#"+line+"\n")
      if (g_gsv["sent_num"] is not None) and (g_gsv["sent_num"]<=3):
        sent_idx=g_gsv["sent_num"]-1
        g_gsv_arr[g_gsv["sent_num"]-1] = g_gsv.copy()
    #Check when NMEA sequence is complete
    if fields[0] == "$GPRTE" and c_receiver_type == "geko301":
      #sequence is complete for Garmin geko301, so determine g_timestamp and set g_write_genivi_log to True
      if (g_rmc["hour"] is not None) and (g_rmc["minute"] is not None) and (g_rmc["second"] is not None):
        g_timestamp = 1000*(g_rmc["hour"]*3600+g_rmc["minute"]*60+g_rmc["second"])
      elif g_timestamp > 0:
        g_timestamp = g_timestamp + 2000 #if no UTC time, simply add 2 seconds (geko update interval)
      if g_timestamp > 0:
        g_write_genivi_log = True
    if fields[0] == "$GPRMC" and c_receiver_type == "z205":
      #sequence is complete for Becker Z205, so determine g_timestamp and set g_write_genivi_log to True
      if (g_rmc["hour"] is not None) and (g_rmc["minute"] is not None) and (g_rmc["second"] is not None):
        g_timestamp = 1000*(g_rmc["hour"]*3600+g_rmc["minute"]*60+g_rmc["second"])
      elif g_timestamp > 0:
        g_timestamp = g_timestamp + 1000 #if no UTC time, simply add 2 seconds (Z205 update interval)
      if g_timestamp > 0:
        g_write_genivi_log = True


    if g_write_genivi_log:
      g_write_genivi_log = False
      writeGVGNSP  (fo, g_gga, g_rmc, g_timestamp)
      writeGVGNSC  (fo, g_gga, g_rmc, g_timestamp)
      writeGVGNSAC (fo, g_gga, g_rmc, g_timestamp)
      writeGVGNSSAT(fo, g_gsv_arr, g_timestamp)
      #write SNS data if requested
      if c_SNS:
        fo.write("#SNS 'derived' from GPS\n")
        writeGVSNSVSP (fo, g_rmc, g_timestamp)
        writeGVSNSGYR (fo, g_rmc, g_prev_rmc, g_timestamp, g_prev_timestamp)
        writeGVSNSWHE (fo, g_rmc, g_prev_rmc, g_timestamp, g_prev_timestamp)

      #clear buffers, but not g_timestamp
      g_prev_rmc = g_rmc.copy()
      g_prev_gga = g_gga.copy()
      g_prev_timestamp = g_timestamp
      g_rmc = empty_rmc.copy()
      g_gga = empty_gga.copy()
      g_gsv = empty_gsv.copy()
      g_gsv_arr = [empty_gsv.copy() for i in range(3)]


#end of the loop

#
# clean up: close files
#

fi.close()
fo.close()

        #fo.write("{0:02d}-{1:02d}-{2:02d}\n".format(rmc["year"],rmc["month"],rmc["day"]))