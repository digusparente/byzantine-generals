#!/usr/bin/python

from mininet.topo import Topo
from mininet.net import Mininet
from random import sample
import sys


class SingleSwitchTopo(Topo):
    """Single switch connected to n hosts."""
    def build(self, n=4):
        switch = self.addSwitch('s1')
        # Python's range(N) generates 0..N-1
        for h in range(n):
            host = self.addHost('h%s' % (h + 1))
            self.addLink(host, switch)


def run_byz_generals(m_lieutenants, m_traitors, m_general_is_traitor, m_order):
    """Create and test a simple network"""

    m_lieutenants = m_lieutenants + 1  # add one host to be the general

    topology = SingleSwitchTopo(m_lieutenants)
    net = Mininet(topology)
    net.start()

    program_name = "./bin/soldier "

    pids = {}

    m_traitors = sample(range(2, m_lieutenants + 1), m_traitors)

    for identifier in reversed(range(1, m_lieutenants + 1)):
        hostname = "h" + str(identifier)
        host = net.get(hostname)

        if identifier == 1:
            print("General #{}".format(host))
            args = ("%s %s %s %s &" % (str(identifier), str(m_lieutenants), m_order, m_general_is_traitor))
        else:
            print("Lieutenant #{}".format(host))
            if identifier in m_traitors:
                args = ("%s %s %s %s &" % (str(identifier), str(m_lieutenants), "0", "1"))
            else:
                args = ("%s %s %s %s &" % (str(identifier), str(m_lieutenants), "0", "0"))

        command = program_name + args

        print("$ {}\n".format(command))

        host.cmd(command)

        pids[identifier - 1] = int(host.cmd("echo $!"))

    for identifier in range(1, m_lieutenants + 1):
        hostname = "h" + str(identifier)
        host = net.get(hostname)
        host.cmd("wait", pids[identifier - 1])

    net.stop()

if __name__ == '__main__':
    """Tell mininet to print useful information"""

    lieutenants = int(sys.argv[1])
    traitors = int(sys.argv[2])
    general_is_traitor = sys.argv[3]
    order = sys.argv[4]

    if not (lieutenants > (3 * traitors)):
        print("Number of lieutenants must be at least 3 times the number of traitors")
        sys.exit(0)

    run_byz_generals(lieutenants, traitors, general_is_traitor, order)
