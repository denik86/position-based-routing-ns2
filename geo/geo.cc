#include "geo.h"

// Binding between tcl and c++ objects
int hdr_geo::offset_;
static class GeoHeaderClass : public PacketHeaderClass {
public:
	GeoHeaderClass() : PacketHeaderClass("PacketHeader/GEO",
		sizeof(hdr_geo)) {
		bind_offset(&hdr_geo::offset_);
	}
} class_rtGeo_hdr;

static class GeoClass : public TclClass {
public:
	GeoClass() : TclClass("Agent/GEO") {}
	TclObject* create(int argc, const char*const* argv) {
		return (new Geo());
	}
} class_rtGeo;

Geo::Geo() : Agent(PT_GEO), beacon_timer_(this), addr_(-1), x_(0), y_(0), z_(0), ndpktr_(0)
{
	bind("algo", &algo_);		// algorithm id
	bind("geo_trace", &geo_trace_); // flag for geo_trace
	minPathLength_ = 0;
	beacon_timer_.sched((double)BEACON_PERIOD);
}

int Geo::command(int argc, const char*const* argv) {
	// argv[0] = istance node object
	if(argc == 2) {
		if(strcasecmp(argv[1], "start") == 0) {
			return TCL_OK;
		}
		else if(strcasecmp(argv[1], "set-location") == 0) {
			setLocation();
			return TCL_OK;
		}
	}
	else if(argc == 3) {
		// corresponding mux for packet bring to upper level
		if(strcmp(argv[1], "port-dmux") == 0) {
			dmux_ = (PortClassifier*)TclObject::lookup(argv[2]);
			if(dmux_ == 0) {
				fprintf(stderr, "%s: %s lookup of %s failed\n", __FILE__, argv[1], argv[2]);
				return TCL_ERROR;
			}
			return TCL_OK;
		}
		// trace target
		else if(strcmp(argv[1], "log-target") == 0 || strcmp(argv[1], "tracetarget") == 0) {
			logtarget_ = (Trace*)TclObject::lookup(argv[2]);
			if(logtarget_ == 0)
				return TCL_ERROR;
			return TCL_OK;
		}
		else if (strcmp(argv[1], "addr") == 0) {
			addr_ = Address::instance().str2addr(argv[2]);
			return TCL_OK;
		}
		else if(strcmp(argv[1], "mpl") == 0) {
			minPathLength_ = atof(argv[2]);
			return TCL_OK;
		}
		else if(strcmp(argv[1], "node") == 0) {
			node_ = (MobileNode*)TclObject::lookup(argv[2]);
			if (node_ == 0) {
				fprintf(stderr,  "%s: %s lookup of %s failed \n", __FILE__, argv[1], argv[2]);
				return TCL_ERROR;
			}
			return TCL_OK;
		}
	}
	return Agent::command(argc, argv);
}

void Geo::recv(Packet* p, Handler* h) {

	struct hdr_cmn* ch = HDR_CMN(p);
	struct hdr_ip* ih = HDR_IP(p);
	struct hdr_geo *hdr = HDR_GEO(p);

	// This node is source node
	if((ih->saddr() == addr_) && (ch->num_forwards() == 0)) {

		God* god = God::instance();
		god->ComputeRoute();
		god->stampPacket(p);

		if(ch->opt_num_forwards() > 16777210)  {
			trace("No path!");
			drop(p, DROP_RTR_NO_ROUTE);
			return;
		}
		ch->size() += hdr->size();
		//ch->ptype() = PT_GEO;
		hdr->type_ = GEOPKT_DATA;
		hdr->bornTime_ = CURRENT;
		
		// Initialize data packet
		hdr->src_ = addr_;
		hdr->srcX_ = x_;
		hdr->srcY_ = y_;
		hdr->srcZ_ = z_;
		
		hdr->dst_ = ih->daddr();
		MobileNode *dst = (MobileNode *) (Node::get_node_by_address(hdr->dst_));
		hdr->dstX_ = dst->X();
		hdr->dstY_ = dst->Y();
		hdr->dstZ_ = dst->Z();

		ih->ttl_ = DEFAULT_TTL;
		hdr->mpl_ = minPathLength_;
		ch->txtime_ = CURRENT;
	}

	// Loop on itself
	if((ih->saddr() == addr_) && (ch->num_forwards() == 1)) {
		drop(p, DROP_RTR_ROUTE_LOOP);
		return;
	}

	if(ch->ptype() == PT_GEO) {
		//struct hdr_geo_hello *rp = HDR_GEO_HELLO(p);
		switch(hdr->type_) {
	
			case GEOPKT_HELLO:
				if(ih->saddr() != addr_)
					beaconIn(p);
				Packet::free(p);
				break;
			}
	}


	// Data Packet
	else {
		if(ndpktr_ == 0) 
			gnn_ = new GeoNextNode(algo_, addr_, logtarget_, geo_trace_);
		ndpktr_++;
		ih->ttl_--;
		if(ih->ttl_ == 0) { 
			// time to live expired
			drop(p, DROP_RTR_TTL);
			return;
		}
		
		packets_.push_back(ch->uid()); // store id packet into node

		float delay = 0.0;
		data = p; 	
			
		// Delay them a little to help ARP. 
		// Otherwise ARP may drop packets. -SRD 5/23/99
		forward_data(delay);
		//delay += ARP_DELAY;	
	}
	return;
}

/********************** FORWARDING DATA SECTION **************************************************/
void Geo::forward_data(float delay) {
	struct hdr_cmn* ch = HDR_CMN(data);
	struct hdr_ip* ih = HDR_IP(data);

	if(ch->direction() == hdr_cmn::UP && ((u_int32_t)ih->daddr() == IP_BROADCAST || ih->daddr() == addr_)) {
		dmux_->recv(data, 0); 
		return;
	}
	else {
		/* algorithm fowarding */
		trace("[%d] Next node processing for %d\n", addr_, ch->uid());
		char cand[255];
		cand[0]=0;
		nbr_.print(cand);
		trace("%s",cand);
		nsaddr_t target = gnn_->nextNode(node_, data, nbr_);
		God* god = God::instance();
		while(god->hops(target, addr_) > 1 && target > 0) {
			trace("MAC: neighbor %d not present!", target);
			nbr_.delByAddr(target);
			target = gnn_->nextNode(node_, data, nbr_);
		}
		ch->direction() = hdr_cmn::DOWN;
		ch->addr_type() = NS_AF_INET;
		ch->last_hop_ = addr_;
		//ch->num_forwards_++;


		if(target == FAIL) {
			trace("[%d] FAILED! >>>>", addr_);	
			Packet::free(data);
			return;
		}
		ch->next_hop_ = target;
		trace("final target = %d (%d)", target, god->hops(addr_, target));
		Scheduler::instance().schedule(target_, data, delay);
		
	}
}

// set node's location
void Geo::setLocation() {
	
	x_ = (float)node_->X();
	y_ = (float)node_->Y();
	z_ = (float)node_->Z();
}

void GeoBeaconTimer::expire(Event *e) { agent_->beacon_callback(); }

// Elaborate received beacon
void Geo::beaconIn(Packet *p) {
	struct hdr_ip *ih = HDR_IP(p);
	struct hdr_geo_hello *rp = HDR_GEO_HELLO(p);
	nbr_.addNbr(ih->saddr(), rp->x_, rp->y_, rp->z_, BEACON_ALIVE);
}

void Geo::beacon_callback(void)
{
  Scheduler &s = Scheduler::instance(); // take the time istant
  Packet *p = makeBeacon(); //create the beacon packet

  // schedule the transmission of this beacon
  if (p) {
    assert (!HDR_CMN(p)->xmit_failure_); // simply error test
    s.schedule(target_, p, 0);
  }

  // schedule the next beacon generation event
  beacon_timer_.resched((double)BEACON_PERIOD + ((0.1)*Random::uniform()));
}

// construct and return an alive beacon packet
Packet *Geo::makeBeacon(void)
{
  	Packet *p = allocpkt();
  	struct hdr_ip *ih = HDR_IP(p);
  	struct hdr_cmn *ch = HDR_CMN(p);
  	struct hdr_geo_hello *hb = HDR_GEO_HELLO(p);

	// set up headers
	ch->next_hop_ = IP_BROADCAST;
	ch->prev_hop_ = addr_;
	ch->addr_type_ = AF_INET;
	ch->ptype_ = PT_GEO;
	
	ih->saddr() = addr_;
	ih->daddr() = IP_BROADCAST; //<< Address::instance().nodeshift();
	ih->dport() = GEO_PORT;

	hb->type = GEOPKT_HELLO;
	setLocation();
	hb->x_ = x_;
	hb->y_ = y_;
	hb->z_ = z_;
	//trace("beacon (%f, %f, %f)\n", x_, y_, z_);

	// simulated size: hdr_geo, IP header
	ch->size() = hb->size() + IP_HDR_LEN;
	return p;
}

void
Geo::trace(char* fmt, ...)
{
	if(geo_trace_) {
		va_list ap;
		  
		if (!logtarget_) return;

		va_start(ap, fmt);
		vsprintf(logtarget_->pt_->buffer(), fmt, ap);
		logtarget_->pt_->dump();
		va_end(ap);
	}
}