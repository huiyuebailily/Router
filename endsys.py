#!/usr/bin/env python

"""
simplerouter.py: Example network with Linux IP router


The example topology creates a router and three IP subnets:

    - 192.168.1.0/24 (r0-eth1, IP: 192.168.1.1)
    - 172.16.0.0/12 (r0-eth2, IP: 172.16.0.1)


Each subnet consists of a single host connected to
a single switch:

    r1-eth1 - s1-eth1 - h1-eth0 (IP: 192.168.1.100)
    r1-eth2 - s2-eth1 - h2-eth0 (IP: 172.16.0.100)


The example relies on default routing entries that are
automatically created for each router interface, as well
as 'defaultRoute' parameters for the host interfaces.

Additional routes may be added to the router or hosts by
executing 'ip route' or 'route' commands on the router or hosts.
"""


from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import Node
from mininet.log import setLogLevel, info
from mininet.cli import CLI


class LinuxRouter( Node ):
    "A Node with IP forwarding enabled."

    # pylint: disable=arguments-differ
    def config( self, **params ):
        super( LinuxRouter, self).config( **params )
        # Enable forwarding on the router
        self.cmd( 'sysctl net.ipv4.ip_forward=1' )

    def terminate( self ):
        self.cmd( 'sysctl net.ipv4.ip_forward=0' )
        super( LinuxRouter, self ).terminate()


class NetworkTopo( Topo ):
    "A LinuxRouter connecting three IP subnets"

    # pylint: disable=arguments-differ
    def build( self, **_opts ):

        s1= self.addSwitch('s1' ) 


        h1 = self.addHost( 'h1', ip='10.1.1.100/24' )
        h2 = self.addHost( 'h2', ip='10.1.1.101/24')

        self.addLink( h1, s1 )
        self.addLink( h2, s1 )


def run():
    "Test linux router"
    topo = NetworkTopo()
    net = Mininet( topo=topo,
                   waitConnected=True )  # controller is used by s1-s3
    net.start()
    info( '*** Routing Table on Router:\n' )
    #info( net[ 'r1' ].cmd( 'route' ) )
    info( net[ 'h1' ].cmd( 'route add 255.255.255.255 dev h1-eth0' ) )
    info( net[ 'h2' ].cmd( 'route add 255.255.255.255 dev h2-eth0' ) )
    CLI( net )
    net.stop()


if __name__ == '__main__':
    setLogLevel( 'info' )
    run()
