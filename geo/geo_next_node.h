#ifndef GEO_NEXT_NODE_H_
#define GEO_NEXT_NODE_H_

#include <mobilenode.h>
#include <time.h> 
#include <random.h>
#include "geo_node.h"
#include "geo_utility.h"
#include <god.h>

// Forwarding algorithms id
#define DISTANCE		1
#define COMPASS			2
#define MOST_FORWARD	3
//define new_algorithm	4

#define FAIL -1

class GEO_data;

// Node's memory space for routing data (e.g., history)
class GEO_data {
public:
	
	//Protocol variables

	//Protocol functions

	 GEO_data();
};

class GeoNextNode {

public:
	GeoNextNode();
  	~GeoNextNode();
	GeoNextNode(int, nsaddr_t, Trace*, int);

	nsaddr_t nextNode(MobileNode*, Packet*, GNeighbors);
	nsaddr_t greedy(int, bool, GNeighbors);

private:
	int algo_;						// forward algorithm
	nsaddr_t addr_;					// node's address
	float x_, y_, z_;				// node's coordinates
	Trace* logtargetnext_; 			// log trace
	int geo_trace_;	
	GEO_data geo_data_;

	GNeighbors nbr_;				// neighbors object

	struct hdr_geo* hdr;			// GEO header
	struct hdr_cmn* ch;				// COMMON header	

	void trace(char* fmt, ...);		// prototipe trace print
};

#endif




