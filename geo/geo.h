// Position-based routing protocols for ns-2 wireless extensions
// Author: Daniele Ronzani, University of Padua.


#include <vector>
#include <agent.h>
#include <packet.h>
#include <trace.h>
#include <timer-handler.h>
#include <random.h>
//#include <mobilenode.h>
#include <classifier-port.h>
#include <cmu-trace.h>
#include <time.h> 
#include <stdarg.h>
#include <mac.h>

#include "geo_next_node.h"

#define ARP_DELAY 0.01

#define BEACON_PERIOD 1	// default value of sending beacon period
#define BEACON_ALIVE 2.5 		// timeout for expiring received neighbor beacon
#define GEO_PORT 0xff		// port number for GEO protocol messages
#define DEFAULT_TTL 100

using namespace std;

class Geo;

/* Tmer for beacon callback */
class GeoBeaconTimer : public TimerHandler {

public:
	GeoBeaconTimer(Geo *agent) : TimerHandler() {agent_ = agent;}
protected:
	Geo *agent_;
	virtual void expire(Event * e);
};

/* Agent */
class Geo : public Agent {
	/* Friends */
	friend class GeoDataTimer;
	friend class GeoHelloTimer;
	friend class GeoReqTimer;

protected:
	PortClassifier* dmux_; /* for passing packets up to agents */
	GeoBeaconTimer beacon_timer_;
	Trace* logtarget_; 			// log trace
	int geo_trace_;
	void forward_data(float); 	// packet forward
	nsaddr_t getNextNode(); 	// get the next node
	Packet* makeBeacon(void); 	// generate a beacon

public:
	Geo();									// class constructor
	void beacon_callback();
	int command(int, const char*const*);	// command sent by tcl script
	void recv(Packet*, Handler*);			// when receives a packet 
	void beaconIn(Packet*);					// beacon arriving
	void trace(char* fmt, ...);				// prototipe trace print
	void setLocation();

private:
	// Algorithms parameters
	MobileNode *node_; // mobile node pointer
	nsaddr_t addr_; // node address
	GeoNextNode *gnn_; // next node pointer
	int algo_;

	// Node's information
	double x_;
	double y_;
	double z_;
	GNeighbors nbr_;	// list of neighbors
	int ndpktr_;		// number of received data packets
	Packet *data; 		// current data packet

	int minPathLength_;
	std::vector<int> packets_; //store packet id
};
