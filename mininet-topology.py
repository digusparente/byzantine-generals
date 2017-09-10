#!/usr/bin/python

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.log import setLogLevel

import time

class SingleSwitchTopo(Topo):
    """Single switch connected to n hosts."""
    def build(self, n=4):
        switch = self.addSwitch('s1')
        # Python's range(N) generates 0..N-1
        for h in range(n):
            host = self.addHost('h%s' % (h + 1))
            self.addLink(host, switch)

def runByzGenerals(number_generals, number_traitors):
    """Create and test a simple network"""
    number_generals = number_generals;

    topo = SingleSwitchTopo(number_generals)
    net = Mininet(topo)
    net.start()

    soldier_prog = "./bin/soldier "

    pids = {}

    for i in reversed(range(1, number_generals + 1)):
        hostname = 'h' + str(i)
        host = net.get(hostname)

        print "Host ID: ", host

        lieutenant_id = i

        prog_args = ("%s %s %s %s" % (str(lieutenant_id), str(number_generals), "0", "0"))
        prog_other = " &"
        command = soldier_prog + prog_args + prog_other

        print command

        host.cmd(command)

        pids[i-1] = int(host.cmd('echo $!'))

        time.sleep(1)

    for i in range(1, number_generals + 1):
        hostname = 'h' + str(i)
        host = net.get(hostname)
        host.cmd('wait', pids[i-1])

    net.stop()

if __name__ == '__main__':
    """Tell mininet to print useful information"""
    setLogLevel('info')
    runByzGenerals(8, 1)
