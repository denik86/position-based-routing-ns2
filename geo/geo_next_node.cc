#include "geo_next_node.h"
#include "geo_utility.h"

GeoNextNode::GeoNextNode(int algo, nsaddr_t addr, Trace * logtarget, int geo_trace) : x_(0), y_(0), z_(0) {

	algo_ = algo;
	addr_ = addr;

	logtargetnext_ = logtarget;
	geo_trace_ = geo_trace;

	geo_data_ = GEO_data();
}

// Returns the address of the next node
// -1 if the algorithm number is not valid or if the algorithm fails.
nsaddr_t 
GeoNextNode::nextNode(MobileNode* mn, Packet* data, GNeighbors nbr) {

	// take node's current position
	x_ = (float)mn->X();
	y_ = (float)mn->Y();
	z_ = (float)mn->Z();

	// take data packet's headers
	hdr = HDR_GEO(data);
	ch = HDR_CMN(data);

	// take neighbors object
	nbr_ = nbr;
	
	// Check if the neighborood is empty
	if(nbr_.empty()) {
		trace("[%d] *** ERROR: neighborhood is empty!!! ***\n", addr_);
		return FAIL;
	}

	// Check if any neighbor is the destination node
	for(int i = 0; i < nbr_.size(); i++)
	{	
		if(nbr_.get(i)->addr_ == hdr->dst_) {
			trace("destination found!");
			return nbr_.get(i)->addr_;
		}
	}

	switch (algo_)
	{
		case (DISTANCE): // DISTANCE algorithm
		return greedy(DISTANCE, false, nbr_);

		case (COMPASS): // COMPASS algorithm
		return greedy(COMPASS, false, nbr_);

		case (MOST_FORWARD): // MOST FORWARD algorithm
		return greedy(MOST_FORWARD, false, nbr_);

		default:
		fprintf(stderr, "*** ERROR: number of protocol (%d) ****", algo_);
		exit(1);
	}
}

// Returns the address of the node that makes a progress toward the destination
// "type" is the progress algorithm, 
// "back" indicates whether permit (true) or not (false) a return to the previous node.
nsaddr_t 
GeoNextNode::greedy(int type, bool back, GNeighbors nbr) {

	nsaddr_t target = -1;
	if(nbr.empty())
		return target;
	//trace("dest coord: %f, %f, %f\n", hdr->dstX_, hdr->dstY_, hdr->dstZ_);
	float min;
	if(type == DISTANCE) {
		if(back) min = dist(hdr->dstX_, hdr->dstY_, hdr->dstZ_, nbr.front()->x_, nbr.front()->y_, nbr.front()->z_);
		else min = dist(hdr->dstX_, hdr->dstY_, hdr->dstZ_, x_, y_, z_);
	}
	else if(type == COMPASS) min = angle(x_, y_, z_, hdr->dstX_, hdr->dstY_, hdr->dstZ_, nbr.front()->x_, nbr.front()->y_, nbr.front()->z_);
	else if(type == MOST_FORWARD) min = projDist(x_, y_, z_, hdr->dstX_, hdr->dstY_, hdr->dstZ_, nbr.front()->x_, nbr.front()->y_, nbr.front()->z_);
	else { fprintf(stderr, "*** ERROR: number of progress algorithm!! ***"); exit(1);}
	if(back || type==COMPASS|| type==MOST_FORWARD) target = nbr.front()->addr_;
	float curr;
	for(int i = 0; i < nbr.size(); i++) {
		//trace("current coord (%d): %f, %f, %f\n", nbr.get(i)->addr_, nbr.get(i)->x_, nbr.get(i)->y_, nbr.get(i)->z_);
		if(type == DISTANCE) curr = dist(hdr->dstX_, hdr->dstY_, hdr->dstZ_, nbr.get(i)->x_, nbr.get(i)->y_, nbr.get(i)->z_);
		else if(type == COMPASS) curr = angle(x_, y_, z_, hdr->dstX_, hdr->dstY_, hdr->dstZ_, nbr.get(i)->x_, nbr.get(i)->y_, nbr.get(i)->z_);
		else if(type == MOST_FORWARD) curr = projDist(x_, y_, z_, hdr->dstX_, hdr->dstY_, hdr->dstZ_, nbr.get(i)->x_, nbr.get(i)->y_, nbr.get(i)->z_);
		if(curr < min) {
			target = nbr.get(i)->addr_;
			min = curr;
		}
	}
	// fails if returns the previous node (if back is false)
	if(!back && target == ch->last_hop_) 
		return -1;
	
	return target;
}

void
GeoNextNode::trace(char* fmt, ...)
{
	if(geo_trace_) {
	  va_list ap;
	  //fprintf(stderr, "scrive\n");
	  if (!logtargetnext_) return;

	  va_start(ap, fmt);
	  vsprintf(logtargetnext_->pt_->buffer(), fmt, ap);
	  logtargetnext_->pt_->dump();
	  va_end(ap);
	}
}


GEO_data::GEO_data() {

	 	 // Call constructors for variables
	 	 // ...
}
