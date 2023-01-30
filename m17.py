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
        h7 = self.addHost( 'h7', ip='10.1.7.100/24',
                           defaultRoute='via 10.1.7.1' )
        h8 = self.addHost( 'h8', ip='10.1.8.100/24',
                           defaultRoute='via 10.1.8.1' )
        h9 = self.addHost( 'h9', ip='10.1.9.100/24',
                           defaultRoute='via 10.1.9.1' )
        h10 = self.addHost( 'h10', ip='10.1.10.100/24',
                           defaultRoute='via 10.1.10.1' )
        h11 = self.addHost( 'h11', ip='10.1.11.100/24',
                           defaultRoute='via 10.1.11.1' )
        h12 = self.addHost( 'h12', ip='10.1.12.100/24',
                           defaultRoute='via 10.1.12.1' )
        h13 = self.addHost( 'h13', ip='10.1.13.100/24',
                           defaultRoute='via 10.1.13.1' )
        h14 = self.addHost( 'h14', ip='10.1.14.100/24',
                           defaultRoute='via 10.1.14.1' )
        h15 = self.addHost( 'h15', ip='10.1.15.100/24',
                           defaultRoute='via 10.1.15.1' )
        h16 = self.addHost( 'h16', ip='10.1.16.100/24',
                           defaultRoute='via 10.1.16.1' )
        h17 = self.addHost( 'h17', ip='10.1.17.100/24',
                           defaultRoute='via 10.1.17.1' )

        r1 = self.addHost( 'r1')
        r2 = self.addHost( 'r2')
        r3 = self.addHost( 'r3')
        r4 = self.addHost( 'r4')
        r5 = self.addHost( 'r5')
        r6 = self.addHost( 'r6')
        r7 = self.addHost( 'r7')
        r8 = self.addHost( 'r8')
        r9 = self.addHost( 'r9')
        r10 = self.addHost( 'r10')
        r11 = self.addHost( 'r11')
        r12 = self.addHost( 'r12')
        r13 = self.addHost( 'r13')
        r14 = self.addHost( 'r14')
        r15 = self.addHost( 'r15')
        r16 = self.addHost( 'r16')
        r17 = self.addHost( 'r17')

        s1= self.addSwitch( 's1')
        s2= self.addSwitch( 's2')
        s3= self.addSwitch( 's3')
        s4= self.addSwitch( 's4')
        s5= self.addSwitch( 's5')
        s6= self.addSwitch( 's6')
        s7= self.addSwitch( 's7')
        s8= self.addSwitch( 's8')
        s9= self.addSwitch( 's9')
        s10= self.addSwitch( 's10')
        s11= self.addSwitch( 's11')
        s12= self.addSwitch( 's12')
        s13= self.addSwitch( 's13')
        s14= self.addSwitch( 's14')
        s15= self.addSwitch( 's15')
        s16= self.addSwitch( 's16')
        s17= self.addSwitch( 's17')

        ss1= self.addSwitch( 'ss1')
        ss2= self.addSwitch( 'ss2')
        ss3= self.addSwitch( 'ss3')
        ss4= self.addSwitch( 'ss4')
        ss5= self.addSwitch( 'ss5')
        ss6= self.addSwitch( 'ss6')
        ss7= self.addSwitch( 'ss7')
        ss8= self.addSwitch( 'ss8')
        ss9= self.addSwitch( 'ss9')
        ss10= self.addSwitch( 'ss10')
        ss11= self.addSwitch( 'ss11')
        ss12= self.addSwitch( 'ss12')
        ss13= self.addSwitch( 'ss13')
        ss14= self.addSwitch( 'ss14')
        ss15= self.addSwitch( 'ss15')
        ss16= self.addSwitch( 'ss16')
        ss17= self.addSwitch( 'ss17')

        self.addLink( h1,s1 )
        self.addLink( h2,s2 )
        self.addLink( h3,s3 )
        self.addLink( h4,s4 )
        self.addLink( h5,s5 )
        self.addLink( h6,s6 )
        self.addLink( h7,s7 )
        self.addLink( h8,s8 )
        self.addLink( h9,s9 )
        self.addLink( h10,s10 )
        self.addLink( h11,s11 )
        self.addLink( h12,s12 )
        self.addLink( h13,s13 )
        self.addLink( h14,s14 )
        self.addLink( h15,s15 )
        self.addLink( h16,s16 )
        self.addLink( h17,s17 )


        self.addLink( r1,s1)
        self.addLink( r2, s2)
        self.addLink( r3, s3)
        self.addLink( r4, s4)
        self.addLink( r5, s5)
        self.addLink( r6, s6)
        self.addLink( r7, s7)
        self.addLink( r8, s8)
        self.addLink( r9, s9)
        self.addLink( r10, s10)
        self.addLink( r11, s11)
        self.addLink( r12, s12)
        self.addLink( r13, s13)
        self.addLink( r14, s14)
        self.addLink( r15, s15)
        self.addLink( r16, s16)
        self.addLink( r17, s17)


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

        self.addLink( r6,ss6)
        self.addLink( r7,ss6)

        self.addLink( r7,ss7)
        self.addLink( r8,ss7)

        self.addLink( r8,ss8)
        self.addLink( r9,ss8)


        self.addLink( r9,ss9)
        self.addLink( r10,ss9)


        self.addLink( r10,ss10)
        self.addLink( r11,ss10)

        self.addLink( r11,ss11)
        self.addLink( r12,ss11)

        self.addLink( r12,ss12)
        self.addLink( r13,ss12)

        self.addLink( r13,ss13)
        self.addLink( r14,ss13)

        self.addLink( r14,ss14)
        self.addLink( r15,ss14)

        self.addLink( r15,ss15)
        self.addLink( r16,ss15)

        self.addLink( r16,ss16)
        self.addLink( r17,ss16)


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


    info( net[ 'r7' ].cmd( 'ifconfig r7-eth0 10.1.7.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r7' ].cmd( 'ifconfig r7-eth1 20.1.6.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r7' ].cmd( 'ifconfig r7-eth2 20.1.7.2 netmask 255.255.255.0  ' ) )


    info( net[ 'r8' ].cmd( 'ifconfig r8-eth0 10.1.8.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r8' ].cmd( 'ifconfig r8-eth1 20.1.7.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r8' ].cmd( 'ifconfig r8-eth2 20.1.8.2 netmask 255.255.255.0  ' ) )

    info( net[ 'r9' ].cmd( 'ifconfig r9-eth0 10.1.9.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r9' ].cmd( 'ifconfig r9-eth1 20.1.8.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r9' ].cmd( 'ifconfig r9-eth2 20.1.9.2 netmask 255.255.255.0  ' ) )


    info( net[ 'r10' ].cmd( 'ifconfig r10-eth0 10.1.10.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r10' ].cmd( 'ifconfig r10-eth1 20.1.9.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r10' ].cmd( 'ifconfig r10-eth2 20.1.10.2 netmask 255.255.255.0  ' ) )


    info( net[ 'r11' ].cmd( 'ifconfig r11-eth0 10.1.11.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r11' ].cmd( 'ifconfig r11-eth1 20.1.10.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r11' ].cmd( 'ifconfig r11-eth2 20.1.11.2 netmask 255.255.255.0  ' ) )


    info( net[ 'r12' ].cmd( 'ifconfig r12-eth0 10.1.12.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r12' ].cmd( 'ifconfig r12-eth1 20.1.11.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r12' ].cmd( 'ifconfig r12-eth2 20.1.12.2 netmask 255.255.255.0  ' ) )

    info( net[ 'r13' ].cmd( 'ifconfig r13-eth0 10.1.13.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r13' ].cmd( 'ifconfig r13-eth1 20.1.12.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r13' ].cmd( 'ifconfig r13-eth2 20.1.13.2 netmask 255.255.255.0  ' ) )

    info( net[ 'r14' ].cmd( 'ifconfig r14-eth0 10.1.14.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r14' ].cmd( 'ifconfig r14-eth1 20.1.13.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r14' ].cmd( 'ifconfig r14-eth2 20.1.14.2 netmask 255.255.255.0  ' ) )

    info( net[ 'r15' ].cmd( 'ifconfig r15-eth0 10.1.15.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r15' ].cmd( 'ifconfig r15-eth1 20.1.14.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r15' ].cmd( 'ifconfig r15-eth2 20.1.15.2 netmask 255.255.255.0  ' ) )


    info( net[ 'r16' ].cmd( 'ifconfig r16-eth0 10.1.16.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r16' ].cmd( 'ifconfig r16-eth1 20.1.15.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r16' ].cmd( 'ifconfig r16-eth2 20.1.16.2 netmask 255.255.255.0  ' ) )


    info( net[ 'r17' ].cmd( 'ifconfig r17-eth0 10.1.17.2 netmask 255.255.255.0  ' ) )
    info( net[ 'r17' ].cmd( 'ifconfig r17-eth1 20.1.16.3 netmask 255.255.255.0  ' ) )
    info( net[ 'r17' ].cmd( 'ifconfig r17-eth2 20.1.17.2 netmask 255.255.255.0  ' ) )


    #info( net[ 'r1' ].cmd( 'route' ) )
    CLI( net )
    net.stop()


if __name__ == '__main__':
    setLogLevel( 'info' )
    run()
