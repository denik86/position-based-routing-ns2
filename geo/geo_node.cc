#include "geo_node.h"
#include <cmath>
#include <iostream>
#include <string.h>
#include "geo_utility.h"

GNode::GNode(nsaddr_t id, float x, float y, float z, double ts, GNeighbors* nbr) : addr_(id), x_(x), y_(y), z_(z), ts_(ts), nat_(id, nbr) { }

// delete neighbor when NeighborAliveTimer exipred
void NeighborAliveTimer::expire(Event *e) {
	nbr_->delByAddr(id_);
}


GNeighbors::GNeighbors() {
	nbSize_ = 0;
}

GNeighbors::~GNeighbors() {
	//free(nbr_);
}

int GNeighbors::size() {
	return nbSize_;
}

void GNeighbors::print(char * out) {

	char s[255];
	strcpy(s, "[");
	for(int i = 0; i < nbSize_; i++) {
		if(i < nbSize_ - 1) {
			char ad[5];
			sprintf(ad, "%d", nbr_.at(i)->addr_);
			strcat(s, ad);
			strcat(s, " ");
		}
		else {
			char ad[5];
			sprintf(ad, "%d", nbr_.at(i)->addr_);
			strcat(s, ad);
		}
	}
	strcat(s, "]");
	for(int i = 0; i < 255; i++)
	{
		out[i] = s[i];
	}
}

bool GNeighbors::empty() {
	if(nbSize_ == 0) return true;
	return false;
}

// Add node in the neighbor list
// If the node is already present return false, otherwise return true 
bool GNeighbors::addNbr(nsaddr_t id, float x, float y, float z, float beacon_alive) {
	
	for(std::vector<GNode*>::iterator it = nbr_.begin(); it != nbr_.end(); it++) {
		// se presente aggiorno solo le coordinate
		if((*it)->addr_ == id) {
			(*it)->x_ = x;
			(*it)->y_ = y;
			(*it)->z_ = z;
			(*it)->ts_ = CURRENT;
			(*it)->nat_.force_cancel();
			(*it)->nat_.resched(beacon_alive);
			return false;
		}
	}
	// aggiungo il nodo dentro la lista
	GNode *nn = new GNode(id, x, y, z, CURRENT, this);
	nn->nat_.sched(beacon_alive);
	nbr_.push_back(nn);
	nbSize_++;
	return true;
}

void GNeighbors::addNode(GNode * node) {
	nbr_.push_back(node);
	nbSize_++;
}

nsaddr_t GNeighbors::delByAddr(nsaddr_t id) {
	 for (unsigned i=0; i<nbr_.size(); i++) {
		if(nbr_.at(i)->addr_ == id) {
			nbr_.erase(nbr_.begin()+i);
			nbSize_--;
			return id;
		}
	}
	return -1;
}

GNode* GNeighbors::get(int i) {
	return nbr_.at(i);
}

GNode* GNeighbors::getByAddr(nsaddr_t addr) {
	for (unsigned i=0; i<nbr_.size(); i++)
		if(nbr_.at(i)->addr_ == addr)
			return nbr_.at(i);
	return NULL;
}

GNode* GNeighbors::front() {
	return nbr_.at(0);
}

void GNeighbors::clear() {
		nbr_.clear();
		nbSize_ = 0;
}

