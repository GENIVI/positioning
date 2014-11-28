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
*        can be tested using a python script with a TK based GUI
*        which reacts on DBus Signals
*        It is based on test-enhanced-position-service.py from Marco Residori
*        and the multithreaded Tk approach described in
*        http://bytes.com/topic/python/answers/448559-oddities-tkinter
*
* \author Helmut Schmidt <Helmut.3.Schmidt@continental-corporation.com>
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

"""
**************************************************************************
*
* Current restrictions
* Stopping the GUI does not always work as expected
* - when the Exit button is clicked, it sometimes hangs for a while
* - it's not possible to stop the GUI by closing the window
*
* Alternative implementation
* - Poll on the queue instead of waiting for event
*   as described in http://effbot.org/zone/tkinter-threads.htm
*   But apparently this does not improve the issue conerning stopping
*
**************************************************************************
"""

#for dbus access
import dbus
import gobject
import dbus.mainloop.glib

#for the TK based GUI (dbus mainloop runs in background thread)
import threading
import Queue
from Tkinter import *

#constants as defined in the Positioning API
LATITUDE  = 0x00000001
LONGITUDE = 0x00000002
ALTITUDE  = 0x00000004
CLIMB     = 0x00000020
SPEED     = 0x00000010
HEADING   = 0x00000008

if __name__ == '__main__':
#According http://dbus.freedesktop.org/doc/dbus-python/doc/tutorial.html#setting-up-an-event-loop
# the main loop must be setup before connecting to the bus.
#Probably this line could be moved further down
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True) 


#The following functions depend on global variables which are defined later
#That's ugly, but it works in python
#Probably we can avoid this with using classes in a redesign

#DBus signal receiver
#Sends a custom event to the Tk main loop and puts the data in a queue
def catchall_positioning_signals_handler(changedValues):
    #print ('PositionUpdate')
    #print changedValues
    ## Each time the signal is catched, put the parameter in the queue...
    commQueue.put(changedValues)
    ## ... and generate a custom event on the main window
    try:
        tk_root.event_generate('<<SignalPositionUpdate>>', when='tail')
    ## If it failed, the window has been destoyed: over
    except:
        print ("Cannot send Tk Event, terminating ....");
        stopEvent.set()

# Receive the custom event in the GUI thread:
# Retrieve changed data over dbus and display them  
def signalPositionUpdate(event):
    #print("signalPositionUpdate Event received by GUI thread")
    changedValues = commQueue.get()
    #print changedValues
    position = enhanced_position_interface.GetPositionInfo(changedValues)
    timestamp = position[0]
    #print 'TIMESTAMP:' +str(timestamp)
    data = position[1]
    for key in data:
        if key == LATITUDE:
            labelLat.config(text='LATITUDE:' + str(data[dbus.UInt16(key)]))
        if key == LONGITUDE:
            labelLon.config(text='LONGITUDE:' + str(data[dbus.UInt16(key)]))
        if key == ALTITUDE:
            labelAlt.config(text='ALTITUDE:' + str(data[dbus.UInt16(key)]))
        if key == CLIMB:
            labelClimb.config(text='CLIMB:' + str(data[dbus.UInt16(key)]))
        if key == SPEED:
            labelSpeed.config(text='SPEED:' + str(data[dbus.UInt16(key)]))
        if key == HEADING:
            labelHeading.config(text='HEADING:' + str(data[dbus.UInt16(key)]))
    #It's important to call update_idletasks() to update the GUI
    tk_root.update_idletasks()

#Tasks which have to be done periodically within the dbus thread
def dbus_timeout_periodic():
    #it seems that the dbus thread blocks the Tk GUI thread if we don't call update_idletasks()
    tk_root.update_idletasks()
    #Check whether the termination event is set and terminate dbus thread and Tk GUI
    if stopEvent.is_set():
        print ('stopEvent.is_set()')
        dbus_loop.quit()
        tk_root.quit() # as well call root.destroy() ???
    else:
        gobject.timeout_add(100, dbus_timeout_periodic)

##############Here comes the real main()#################
print ('Enhanced Positioning Test GUI')
print ('== Always use the Exit button to terminate (closing the window may not work) ==')

###Event and Queue for thread synchronization/communication
stopEvent = threading.Event()
commQueue = Queue.Queue()

###Create Tk main window
tk_root = Tk()
tk_root.title("Enhanced Positioning Test GUI")
# Setup the GUI
labelLat = Label(tk_root, width=40, text='LATITUDE: UKNOWN')
labelLat.pack()
labelLon = Label(tk_root, width=40, text='LONGITUDE: UKNOWN')
labelLon.pack()
labelAlt = Label(tk_root, width=40, text='ALTITUDE: UKNOWN')
labelAlt.pack()
labelClimb = Label(tk_root, width=40, text='CLIMB: UKNOWN')
labelClimb .pack()
labelSpeed = Label(tk_root, width=40, text='SPEED: UKNOWN')
labelSpeed.pack()
labelHeading = Label(tk_root, width=40, text='HEADING: UKNOWN')
labelHeading.pack()
bExit = Button(tk_root, text="Exit", command=stopEvent.set)
bExit.pack()
#bind the event to the callback function
tk_root.bind('<<SignalPositionUpdate>>', signalPositionUpdate)

### Setup the DBus
#connect to session bus
bus = dbus.SessionBus()
#some global variables for the dbus used inside the signal handler
enhanced_position = bus.get_object('org.genivi.positioning.EnhancedPosition','/org/genivi/positioning/EnhancedPosition')
enhanced_position_interface = dbus.Interface(enhanced_position, dbus_interface='org.genivi.positioning.EnhancedPosition')
#register the signal handler
bus.add_signal_receiver(catchall_positioning_signals_handler, \
                        dbus_interface = "org.genivi.positioning.EnhancedPosition", \
                        signal_name = "PositionUpdate")
#create the dbus loop (must be global so we can terminate it)
gobject.timeout_add(100, dbus_timeout_periodic)
dbus_loop = gobject.MainLoop()


###Finally: start the dbus thread and then the Tk main loop
dbus_thread=threading.Thread(target=dbus_loop.run)
dbus_thread.start()
tk_root.mainloop()
