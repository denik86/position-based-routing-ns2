

# ======================================================================
# Define options
# ======================================================================

set val(chan)       Channel/WirelessChannel
set val(prop)       Propagation/FreeSpace    ;#FreeSpace !!!!!
set val(netif)      Phy/WirelessPhy
set val(mac)        Mac/802_11
set val(ifq)        Queue/DropTail/PriQueue
set val(ll)         LL
set val(ant)        Antenna/OmniAntenna
set val(x)              1500   ;# X dimension of the topography
set val(y)              1500   ;# Y dimension of the topography
set val(z)              1500   ;# Z dimension of the topography !!!!!
set val(ifqlen)         50            ;# max packet in ifq
set val(seed)           0.0
set val(adhocRouting)   DSR
set val(nn)             3             ;# how many nodes are simulated
set val(stop)           51.0           ;# simulation time

# =====================================================================
# Main Program
# ======================================================================

#
# Initialize Global Variables
#

# create simulator instance

set ns_		[new Simulator]

# setup topography object

set topo	[new Topography]

# create trace object for ns

$ns_ use-newtrace

set tracef_3D	[open wireless3D_v2.tr w]
$ns_ trace-all $tracef_3D 

# define topology
$topo load_cube $val(x) $val(y) $val(z) 1

#
# Create God
#
set god_ [create-god $val(nn)]

#
# define how node should be created
#

#global node setting

$ns_ node-config -adhocRouting $val(adhocRouting) \
                 -llType $val(ll) \
                 -macType $val(mac) \
                 -ifqType $val(ifq) \
                 -ifqLen $val(ifqlen) \
                 -antType $val(ant) \
                 -propType $val(prop) \
                 -phyType $val(netif) \
                 -channelType $val(chan) \
		 -topoInstance $topo \
		 -agentTrace ON \
                 -routerTrace OFF \
                 -macTrace OFF 

#
#  Create the specified number of nodes [$val(nn)] a
#   

for {set i 0} {$i < $val(nn) } {incr i} {
	set node_($i) [$ns_ node]	
	$node_($i) random-motion 0		;# disable random motion
}


# 
# Define node movement model
#
$node_(0) set X_ 100.0
$node_(0) set Y_ 200.0
$node_(0) set Z_ 300.0
$node_(1) set X_ 400.0
$node_(1) set Y_ 500.0
$node_(1) set Z_ 600.0
$node_(2) set X_ 700.0
$node_(2) set Y_ 800.0
$node_(2) set Z_ 900.0

$ns_ at 20.0 "$node_(0) setdest3d 200.0 200.0 300.0 10.0"
$ns_ at 30.0 "$node_(1) setdest3d 500.0 500.0 500.0 10.0"
$ns_ at 40.0 "$node_(2) setdest3d 600.0 600.0 600.0 10.0"

# 
# Define traffic model
#
# CBR from n0  to n2
#
set udp0 [new Agent/UDP]
$ns_ attach-agent $node_(0) $udp0
set null2 [new Agent/Null]
$ns_ attach-agent $node_(2) $null2
set cbr0 [new Application/Traffic/CBR]
$cbr0 set packetSize_ 512
$cbr0 set interval_ 5.0
$cbr0 attach-agent $udp0
$ns_ connect $udp0 $null2

#
# Tell nodes when the simulation ends
#
for {set i 0} {$i < $val(nn) } {incr i} {
    $ns_ at $val(stop).0 "$node_($i) reset";
}
$ns_ at  $val(stop).002 "puts \"NS EXITING...\" ; $ns_ halt"

#
# Start simulation
#
puts "Starting Simulation..."
$ns_ at 10 "$cbr0 start"
$ns_ at 50 "$cbr0 stop"
$ns_ run




