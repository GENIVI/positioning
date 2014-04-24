#!/usr/bin/python
#
# call from your build directory (e.g. build, Debug, ...)
#
# vim: set noexpandtab sw=4 ts=4:
#
#-----------------------------------------------------------------------
# If you have no active dbus session you have to call with dbus-launch
#
#   $ dbus-launch --exit-with-session ../run-test.py
#
#-----------------------------------------------------------------------


import time
import sys
from StartHelperGeneric import StartHelperGeneric

#-----------------------------------------------------------------------------
# main
#-----------------------------------------------------------------------------
def main():
	duration = 3.0
	print "starting test script: test %i seconds...\n" % duration

	# starten server by helper
	server = StartHelperGeneric(["enhanced-position-service/src/position-daemon"], "server")
	server.setEnv("REPLAYER_LOG_FILE",
		      "../replayer/logs/testlog.gvsl")
	client = StartHelperGeneric(["enhanced-position-service/test/test-enhpos"], "test-client")

	server.start()

	time.sleep(2)
	client.start()

	time.sleep(duration)

	# expect them to be terminated by SIGINT --> -2
	status = 0
	rc = 0
	rc = client.stop()
	if rc != -2:
		status += 1

	rc = server.stop()
	if rc != -2:
		status += 1

	print "exit code: %d" % status
	sys.exit(status)

if __name__ == "__main__":
    main()

