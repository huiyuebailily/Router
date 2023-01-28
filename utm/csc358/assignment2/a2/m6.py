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

        defaultIP = '10.1.1.1/0'  # IP address for r1-eth1

        h1 = self.addHost( 'h1', ip='10.1.1.100/24',
                           defaultRoute='via 10.1.1.2' )
        h2 = self.addHost( 'h2', ip='10.1.2.100/24',
                           defaultRoute='via 10.1.2.2' )
        h3 = self.addHost( 'h3', ip='10.1.3.100/24',
                           defaultRoute='via 10.1.3.1' )
        h4 = self.addHost( 'h4', ip='10.1.4.100/24',
                           defaultRoute='via 10.1.4.1' )
        h5 = self.addHost( 'h5', ip='10.1.5.100/24',
                           defaultRoute='via 10.1.5.1' )
        h6 = self.addHost( 'h6', ip='10.1.6.100/24',
                           defaultRoute='via 10.1.6.1' )

        r1 = self.addHost( 'r1')
        r2 = self.addHost( 'r2')
        r3 = self.addHost( 'r3')
        r4 = self.addHost( 'r4')
        r5 = self.addHost( 'r5')
        r6 = self.addHost( 'r6')

        s1= self.addSwitch( 's1')
        s2= self.addSwitch( 's2')
        s3= self.addSwitch( 's3')
        s4= self.addSwitch( 's4')
        s5= self.addSwitch( 's5')
        s6= self.addSwitch( 's6')

        ss1= self.addSwitch( 'ss1')
        ss2= self.addSwitch( 'ss2')
        ss3= self.addSwitch( 'ss3')
        ss4= self.addSwitch( 'ss4')
        ss5= self.addSwitch( 'ss5')

        self.addLink( h1,s1 )
        self.addLink( h2,s2 )
        self.addLink( h3,s3 )
        self.addLink( h4,s4 )
        self.addLink( h5,s5 )
        self.addLink( h6,s6 )


        self.addLink( r1,s1)
        self.addLink( r2, s2)
        self.addLink( r3, s3)
        self.addLink( r4, s4)
        self.addLink( r5, s5)
        self.addLink( r6, s6)



        self.addLink( r1,ss1)
        self.addLink( r2,ss1)

        self.addLink( r2,ss2)
        self.addLink( r3,ss2)

        self.addLink( r3,ss3)
        self.addLink( r4,ss3)

        self.addLink( r4,ss4)
        self.addLink( r5,ss4)

        self.addLink( r5,ss5)
        self.addLink( r6,ss5)

def run():
    "Test linux router"
    topo = NetworkTopo()
    net = Mininet( topo=topo,
                   waitConnected=True )  # controller is used by s1-s3
    net.start()
    info( '*** Routing Table on Router:\n' )
    #info( net[ 'r1' ].cmd( 'route add 255.255.255.255 dev r1-eth0' ) )
    info( net[ 'r1' ].cmd( 'ifconfig r1-eth0 10.1.1.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r1' ].cmd( 'ifconfig r1-eth1 20.1.1.2 netmask 255.255.255.0  ' ) )

    info( net[ 'r2' ].cmd( 'ifconfig r2-eth0 10.1.2.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r2' ].cmd( 'ifconfig r2-eth1 20.1.1.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r2' ].cmd( 'ifconfig r2-eth2 20.1.2.2 netmask 255.255.255.0  ' ) )

    info( net[ 'r3' ].cmd( 'ifconfig r3-eth0 10.1.3.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r3' ].cmd( 'ifconfig r3-eth1 20.1.2.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r3' ].cmd( 'ifconfig r3-eth2 20.1.3.2 netmask 255.255.255.0  ' ) )

    info( net[ 'r4' ].cmd( 'ifconfig r4-eth0 10.1.4.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r4' ].cmd( 'ifconfig r4-eth1 20.1.3.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r4' ].cmd( 'ifconfig r4-eth2 20.1.4.2 netmask 255.255.255.0  ' ) )

    info( net[ 'r5' ].cmd( 'ifconfig r5-eth0 10.1.5.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r5' ].cmd( 'ifconfig r5-eth1 20.1.4.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r5' ].cmd( 'ifconfig r5-eth2 20.1.5.2 netmask 255.255.255.0  ' ) )


    info( net[ 'r6' ].cmd( 'ifconfig r6-eth0 10.1.6.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r6' ].cmd( 'ifconfig r6-eth1 20.1.5.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r6' ].cmd( 'ifconfig r6-eth2 20.1.6.2 netmask 255.255.255.0  ' ) )




    #info( net[ 'r1' ].cmd( 'route' ) )
    CLI( net )
    net.stop()


if __name__ == '__main__':
    setLogLevel( 'info' )
    run()
