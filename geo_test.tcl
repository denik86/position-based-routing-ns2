# nam
set nam true

# Define options
set val(chan)	Channel/WirelessChannel		;# channel type
set val(prop)	Propagation/FreeSpace		;# radio-propagation model
set val(ant)	Antenna/OmniAntenna			;# antenna type
set val(ll)		LL							;# link layer type
set val(ifq)	Queue/DropTail/PriQueue		;# interface queue type
set val(ifqlen)	500						;# max packet in ifq
set val(netif)	Phy/WirelessPhy				;# network interface type
set val(mac)	Mac/802_11					;# MAC protocol
set val(rp)		GEO						;# ad-hoc routing protocol
set val(stop)	5				;# the time when the simulation stop

# default values
set val(x) 1000
set val(y) 1000
set val(z) 1000
set val(nn) 12
set val(out) "results" ;# the output file name

### set the transmission range (DEFAULT 250)
set val(r) 250

# GEO parameters
# Select the forwarding algorithm
# Distance 		1
# Compass 		2
# Most Forward 	3
set val(algo_) 1

# create ns instancefor 
set ns_ [new Simulator]

#$ns_ use-scheduler Heap (a to append, w to rewrite)
set trace_3D [open $val(out).tr w]
$ns_ trace-all $trace_3D

if {$nam == "true"} {
	set namtrace [open $val(out).nam w]
	$ns_ namtrace-all-wireless $namtrace $val(x) $val(y)
}

proc setThresh {d} {
	set M_PI 3.14159265359
	set Pt 0.28183815           	;#transmit power
	set Gt 1.0             			;#transmit antenna gain
 	set Gr 1.0               		;#receive antenna
	set freq 914.0e6         		;#frequency
	set lambda [expr 3.0e8/$freq]	;#
	set sysLoss 1.0       			;# system loss

	set M [expr $lambda / (4 * $M_PI * $d)]
	set rx [expr $Pt * $Gt * $Gr * ($M * $M) / $sysLoss]
	return $rx
}

# calculate the transmission power, based on transmission distance range
$val(netif) set CSThresh_ [setThresh $val(r)]
$val(netif) set RXThresh_ [setThresh $val(r)]

# set up topography object
set topo       [new Topography]
$topo load_cube $val(x) $val(y) $val(z) 1
set god_ [create-god $val(nn)]

# configure the nodes
$ns_ node-config -adhocRouting $val(rp) \
		 -llType $val(ll) \
		 -macType $val(mac) \
		 -ifqType $val(ifq) \
		 -ifqLen $val(ifqlen) \
		 -antType $val(ant) \
		 -propType $val(prop) \
		 -phyType $val(netif) \
		 -topoInstance $topo \
		 -agentTrace ON \
		 -routerTrace OFF\
		 -macTrace OFF\
		 -movementTrace OFF \
		 -channel [new $val(chan)]

# setting GEO protcol parameters
Agent/GEO set algo $val(algo_)
Agent/GEO set geo_trace 0


set node_(0) [$ns_ node]
$node_(0) set X_ 100
$node_(0) set Y_ 150
$node_(0) set Z_ 1

set node_(1) [$ns_ node]
$node_(1) set X_ 900
$node_(1) set Y_ 900
$node_(1) set Z_ 1

set node_(2) [$ns_ node]
$node_(2) set X_ 150
$node_(2) set Y_ 300
$node_(2) set Z_ 1

set node_(3) [$ns_ node]
$node_(3) set X_ 350
$node_(3) set Y_ 350
$node_(3) set Z_ 1

set node_(4) [$ns_ node]
$node_(4) set X_ 450
$node_(4) set Y_ 500
$node_(4) set Z_ 1

set node_(5) [$ns_ node]
$node_(5) set X_ 550
$node_(5) set Y_ 450 
$node_(5) set Z_ 1

set node_(6) [$ns_ node]
$node_(6) set X_ 500
$node_(6) set Y_ 650
$node_(6) set Z_ 1

set node_(7) [$ns_ node]
$node_(7) set X_ 650
$node_(7) set Y_ 800
$node_(7) set Z_ 1

set node_(8) [$ns_ node]
$node_(8) set X_ 800
$node_(8) set Y_ 750
$node_(8) set Z_ 1

set node_(9) [$ns_ node]
$node_(9) set X_ 700
$node_(9) set Y_ 550
$node_(9) set Z_ 1

set node_(10) [$ns_ node]
$node_(10) set X_ 750
$node_(10) set Y_ 500
$node_(10) set Z_ 1

set node_(11) [$ns_ node]
$node_(11) set X_ 850
$node_(11) set Y_ 600
$node_(11) set Z_ 1


for {set i 0} {$i < $val(nn)} {incr i} {
	#size node in nam (cirlces)
	$ns_ initial_node_pos $node_($i) 30
}


set udp_(1) [new Agent/UDP]
$ns_ attach-agent $node_(0) $udp_(1)
set null_(1) [new Agent/Null]
$ns_ attach-agent $node_(1) $null_(1)
set cbr_(1) [new Application/Traffic/CBR]
$cbr_(1) set packetsize_ 64
$cbr_(1) set interval_ 0.1
$cbr_(1) set random_ 0

$cbr_(1) attach-agent $udp_(1)
$ns_ connect $udp_(1) $null_(1)
$ns_ at 2.0 "$cbr_(1) start"
$ns_ at 3.0 "$cbr_(1) stop"



# end simulation
$ns_ at [expr $val(stop) + 1.101] "stop"
$ns_ at [expr $val(stop) + 0.102] "puts \"NS EXITING...\" ; $ns_ halt"

for {set i 0} {$i < $val(nn) } {incr i} {
	$ns_ at [expr $val(stop) + 0.101] "$node_($i) reset"
}

proc stop {} {
	global trace_3D namtrace nam
	close $trace_3D
	if {$nam == "true"} {close $namtrace}	
}

puts "Starting Simulation..."
puts "Routing protocol: $val(rp)"
puts "Algorithm: $val(algo_)"
$ns_ run
