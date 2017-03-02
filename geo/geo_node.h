#ifndef GEO_NODE_H_
#define GEO_NODE_H_

#include <packet.h>
#include <vector>
#include <timer-handler.h>
#include "geo_pkt.h"
#include <string>

class GNode;
class GNeighbors;
/* Tmer for beacon callback */
class NeighborAliveTimer : public TimerHandler {

public:
	NeighborAliveTimer(nsaddr_t id, GNeighbors *nbr) : TimerHandler() {id_ = id; nbr_ = nbr;}
protected:
	nsaddr_t id_;
	GNeighbors *nbr_;
	virtual void expire(Event * e);
};

class GNode {
public:
	nsaddr_t addr_;
	float x_;
	float y_;
	float z_;
	double ts_; //last time stamp of hello msg from it
	NeighborAliveTimer nat_;
	GNode(nsaddr_t, float, float, float, double, GNeighbors*);
};

class GNeighbors {
private:
	std::vector<GNode*> nbr_; //list of neighbors
	int nbSize_; // number of neighbors
public:
	GNeighbors();
  	~GNeighbors();
  	int size();
	bool addNbr(nsaddr_t, float, float, float, float);
	void addNode(GNode*);
	nsaddr_t delByAddr(nsaddr_t);
	void clear();
	bool empty();
	GNode * front();
	GNode * get(int);
	GNode * getByAddr(nsaddr_t);
	void print(char *);
};

#endif