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

        defaultIP = '10.1.1.1/24'  # IP address for r1-eth1
        #router = self.addNode( 'r1', cls=LinuxRouter, ip=defaultIP )
        r1 = self.addHost( 'r1', ip='10.1.1.1/24')
        r2 = self.addHost( 'r2', ip='10.1.2.1/24')
        r3 = self.addHost( 'r3', ip='10.1.3.1/24')

        r1.cmd('fconfig r1-eth0 10.1.1.254/24')
        r1.cmd('ifconfig r1-eth2 20.1.1.254/24')

        s1, s2,s3= [ self.addSwitch( s ) for s in ( 's1', 's2','s3') ]

        self.addLink( s1, r1, intfName2='r1-eth1',
                      params2={ 'ip' : '10.1.1.1 /12'} )  # for clarity
        #self.addLink( s2, r2, intfName2='r2-eth1',
        #              params2={ 'ip' : '10.1.2.1/12' } )



        h1 = self.addHost( 'h1', ip='10.1.1.10/24',
                           defaultRoute='via 10.1.1.1' )
        h2 = self.addHost( 'h2', ip='10.1.2.10/24',
                           defaultRoute='via 10.1.2.1' )
        h3 = self.addHost( 'h3', ip='10.1.3.10/24',
                           defaultRoute='via 10.1.3.1' )


        self.addLink( h1, s1 )
        self.addLink( h2, s2 )
        self.addLink( h3, s3 )
        #for h, s in [ (h1, s1), (h2, s2)]:
        #   self.addLink( h, s )


        self.addLink( r1, r2, intfName2='r1-eth2',
                      params2={ 'ip' : '20.1.1.1 /12'} )  
        self.addLink( r2, r3, intfName2='r2-eth2',
                      params2={ 'ip' : '20.1.2.1 /12'} )  

        #for ro1, ro2 in [ (r1, r2), (r2, r3), (r3, r4), (r4, r5), (r5, r6), (r6, r7), (r7, r8), (r8, r9), (r9, r10)]:
        #    self.addLink( ro1, ro2 )
        print(6)

def run():
    "Test linux router"
    topo = NetworkTopo()
    net = Mininet( topo=topo,
                   waitConnected=True )  # controller is used by s1-s3
    net.start()
    info( '*** Routing Table on Router:\n' )
    info( net[ 'r1' ].cmd( 'route' ) )
    CLI( net )
    net.stop()


if __name__ == '__main__':
    setLogLevel( 'info' )
    run()
