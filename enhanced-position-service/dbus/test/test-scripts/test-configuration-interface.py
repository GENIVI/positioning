#!/usr/bin/python

"""
**************************************************************************
* @licence app begin@
* SPDX-License-Identifier: MPL-2.0
*
* \copyright Copyright (C) 2015, Mentor Graphics
*
* \file test-configuration-interface.py
*
* \brief This simple test shows how the configuration interface of the 
*        enhanced-position-service can be easily tested using a python script
*
* \author Marco Residori <marco_residori@mentor.com>
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

#test constants 
UPDATE_INTERVAL = 1500
SATELLITE_SYSTEM = 2

print '\n--------------------------'
print 'Configuration Interface Test'
print '--------------------------\n'

if __name__ == '__main__':
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True) 

#connect to session bus
bus = dbus.SessionBus()

#signal receiver
def catchall_positioning_signals_handler(changedProperty, propertyValue):

    print 'PropertyChanged'
    
    if changedProperty == 'SatelliteSystem':
        print 'SatelliteSystem:' + str(propertyValue)
        if propertyValue != SATELLITE_SYSTEM:
             print '\nTest Failed:' + str(propertyValue) + '!=' + str(SATELLITE_SYSTEM)  + '\n'
             loop.quit()


    if changedProperty == 'UpdateInterval':
        print 'UpdateInterval:' + str(propertyValue)
        if propertyValue != UPDATE_INTERVAL:
             print '\nTest Failed:' + str(propertyValue) + '!=' + str(UPDATE_INTERVAL)  + '\n'
             loop.quit()


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

#test GetVersion()
version = enhanced_position_interface.GetVersion()
print('Version: {0}.{1}.{2} [{3}]'.format(version[0], version[1], version[2], version[3]))
print('')

#test GetSupportedProperties()
supported_properties = enhanced_position_interface.GetSupportedProperties()
print('GetSupportedProperties()')
for key in supported_properties:
    print('{0}:'.format(key))
    for item in supported_properties[key]:
        print('  {0}'.format(item))
print('')

#test GetProperties()
properties = enhanced_position_interface.GetProperties()
print('GetProperties()')
print('UpdateInterval: {0}'.format(properties.get('UpdateInterval', 'N/A')))
print('SatelliteSystem: {0}'.format(properties['SatelliteSystem']))
print('')

#test SetProperty()
enhanced_position_interface.SetProperty(dbus.String('SatelliteSystem'),dbus.UInt32(SATELLITE_SYSTEM, variant_level=1))
enhanced_position_interface.SetProperty('UpdateInterval', dbus.Int32(UPDATE_INTERVAL, variant_level=1))

#main loop 
gobject.timeout_add(3000, timeout)
loop = gobject.MainLoop()
loop.run()


