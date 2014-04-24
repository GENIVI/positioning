#!/usr/bin/python
# -*- coding: utf-8 -*-

# author: thomas.bader@bmw.de
# This client communicates with postion-daemon (genivi)
# DBUS Client using PySide integration

import sys, signal, time
from traceback import print_exc

# import python dbus module
import dbus
# import python dbus GLib mainloop support
import dbus.mainloop.glib
# import QtCore
from PySide.QtCore import *

# handler: state changed
def state_changed(eventText):
    print "state changed ["+ eventText + "]"
    sys.stdout.flush()

#
# MAIN
#
if __name__ == '__main__':

    # use glib main loop
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    # communicate on session bus
    bus = dbus.SessionBus()

    try:
        # get server object
        server_object = bus.get_object("org.genivi.position.EnhancedPosition",
                                       "/")
        # connect to interface
        iface = dbus.Interface(server_object, "org.genivi.position.EnhancedPosition")
    except dbus.DBusException:
        print_exc()
        sys.exit(1)

    # Start the application
    app = QCoreApplication([])
    # qtside specific termination handler! redirect to _DFL
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    # version ok?
    version = []
    result = 0
    version, date = iface.GetVersion()
    print "GetVersion: %d.%d.%d (%s)" % (version[0], version[1], version[2] , date)

    # connect state signal
    iface.connect_to_signal("StateChanged", state_changed)

    # method call: CreateSession
    iface.CreateSession('simple-client.py')
    print 'created session'

    # mehtod call: CloseSession
    iface.CloseSession()
    print 'closed session'

    # enter main loop
    print 'enter application loop ... termninate with ctrl-c'
    app.exec_()
