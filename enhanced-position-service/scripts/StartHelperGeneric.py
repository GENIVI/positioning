# vim: set noexpandtab sw=4 ts=4:
# author: thomas.bader@bmw.de

import os
import subprocess
import time
import sys
import signal

# some issues with terminations.
# time_cmd = ["/usr/bin/time", "-f", "cpu %P | user %U | system %S | %C"]


'''
Exmaple:

	# create server by helper
	file = open('/tmp/server.out', 'w')
	server = StartHelperGeneric(["server"], "server", file)
	client = StartHelperGeneric(["client"], "client-1")

	# start
	server.start()
	client.start()

	time.sleep(1)

	# stop them
	client.stop()
	server.stop()
	file.close()
'''

#--------------------------
# generic
#--------------------------
class StartHelperGeneric:
	def __init__(self, cmd, name="unkown", out=sys.stdout):
		self.cmd = cmd
		self.name = name
		self.out = out
		self.env = os.environ
		print "%s: created" % self.name

	def setEnv(self, name, value):
		self.env[name] = value
		print self.env[name]

	def start(self):
		print "%s: starting ..." % self.name
		self.p = subprocess.Popen(self.cmd, stdin=subprocess.PIPE, stdout=self.out, stderr=sys.stderr, shell=True, env=self.env)

	def isTerminated(self):
		if self.p.poll() != None:
			return True

	def stop(self):
		# already terminated
		if self.isTerminated():
			print "%s: own termination triggered successfully!" % self.name
			return self.p.returncode

		# signal: SIGTERM
		self.p.send_signal(signal.SIGTERM)
		time.sleep(0.3)
		if self.isTerminated():
			print "%s: SIGTERM triggered successfully!" % self.name
			return self.p.returncode

		# signal: SIGINT (Ctr-C)
		self.p.send_signal(signal.SIGINT)
		time.sleep(0.3)
		if self.isTerminated():
			print "%s: Ctrl-C triggered successfully!" % self.name
			return self.p.returncode

		# terminate
		self.p.terminate()
		time.sleep(0.3)
		if self.isTerminated():
			print "%s: 'terminate' triggered successfully!" % self.name
			return self.p.returncode

		# kill
		self.p.kill()
		time.sleep(0.3)
		if self.isTerminated():
			print "%s: killed force successfully" % self.name
			return self.p.returncode

		print "%s: failed to terminate" % self.name
		return 1

	def stop_sigint(self):
		# signal: SIGINT (Ctr-C)
		self.p.send_signal(signal.SIGINT)
		time.sleep(0.3)
		if self.isTerminated():
			print "%s: Ctrl-C triggered successfully!" % self.name
			return self.p.returncode


'''
# demo how to customize your start/stop methods

#-----------------------------------------------------------------------------
# server
#-----------------------------------------------------------------------------
class StartHelperServer(StartHelperGeneric):
	def start(self):
		print "start 'server': %s" % self.name
		StartHelperGeneric.start(self)

	def stop(self):
		print "stop 'server': %s" % self.name
		# try by own methods
		if self.p.poll() == None:
			self.p.stdin.write("1")
			self.p.stdin.write("2")
			time.sleep(0.3)	 # 300 ms
		# use termination
		StartHelperGeneric.stop(self)

#-----------------------------------------------------------------------------
# client
#-----------------------------------------------------------------------------
class StartHelperClient(StartHelperGeneric):
	def start(self):
		print "start 'client': %s" % self.name
		StartHelperGeneric.start(self)

	def stop(self):
		print "stop 'client': %s" % self.name
		# try by own methods
		if self.p.poll() == None:
			self.p.stdin.write("1")
			time.sleep(2.0)	 # 2000 ms
		# use termination
		StartHelperGeneric.stop(self)
*/
'''

