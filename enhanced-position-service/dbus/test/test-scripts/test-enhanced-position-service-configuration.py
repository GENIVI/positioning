#!/usr/bin/python

"""
**************************************************************************
* @licence app begin@
* SPDX-License-Identifier: MPL-2.0
*
* \copyright Copyright (C) 2014, XS Embedded GmbH
*
* \file test-enhanced-position-service.py
*
* \brief This simple test shows how the enhanced-position-service 
*        can be easily tested using a python script
*
* \author Marco Residori <marco.residori@xse.de>
*
* \version 1.0
*
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
* List of changes:
* <date>, <name>, <description of change>
*
* @licence end@
**************************************************************************
"""

import dbus
import gobject
import dbus.mainloop.glib

#constants as defined in the Positioning API
LATITUDE  = 0x00000001
LONGITUDE = 0x00000002
ALTITUDE  = 0x00000004
CLIMB     = 0x00000020
SPEED     = 0x00000010
HEADING   = 0x00000008

print '\n--------------------------'
print 'Positioning Test'
print '--------------------------\n'

if __name__ == '__main__':
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True) 

#connect to session bus
bus = dbus.SessionBus()

#signal receiver
def catchall_positioning_signals_handler(changedValues):
    print 'PropertyChanged'
    satSystem = enhanced_position_interface.GetProperty(dbus.String("SatelliteSystem"))

    print 'SatelliteSystem:' +str(satSystem)


#add signal receiver
bus.add_signal_receiver(catchall_positioning_signals_handler, \
                        dbus_interface = "org.genivi.positioning.Configuration", \
                        signal_name = "PropertyChanged")

#timeout
def timeout():
    print '\nTest Finished\n'
    loop.quit()

#get object
enhanced_position = bus.get_object('org.genivi.positioning.EnhancedPosition','/org/genivi/positioning/Configuration')

#get interface
enhanced_position_interface = dbus.Interface(enhanced_position, dbus_interface='org.genivi.positioning.Configuration')

#enhanced_position_interface.SetProperty(dbus.String('SatelliteSystem'),dbus.UInt32(2, variant_level=1))
enhanced_position_interface.SetProperty(dbus.String('UpdateInterval'),dbus.UInt32(2, variant_level=1))

#satSystem = enhanced_position_interface.GetProperty(dbus.String('SatelliteSystem'))
#print 'SatelliteSystem:' + str(satSystem)

#main loop 

gobject.timeout_add(10000, timeout)
loop = gobject.MainLoop()
loop.run()


