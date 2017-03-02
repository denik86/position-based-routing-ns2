#ifndef __geo_packet_h__
#define __geo_packet_h__

#define CURRENT Scheduler::instance().clock()

// Packet formats
#define GEOPKT_HELLO	0x01
#define GEOPKT_DATA		0x02

// Packet forwarding modes
#define GREEDY_MODE		0x01
// #define OTHER_MODE	0x02

// GEO Header macros
#define HDR_GEO(p) ((struct hdr_geo*)hdr_geo::access(p))
#define HDR_GEO_HELLO(p) ((struct hdr_geo_hello*)hdr_geo::access(p))
#define HDR_GEO_DATA(p) ((struct hdr_geo_data*)hdr_geo::access(p))

// General GEO packet - shared by all formats
struct hdr_geo {
	u_int8_t type_;
	u_int8_t mode_;
	int mpl_;
	float bornTime_;
	
	// source node
	nsaddr_t src_;
	float srcX_;
	float srcY_;
	float srcZ_;

	// destination node
	nsaddr_t dst_;
	float dstX_;
	float dstY_;
	float dstZ_;

	inline int size() {
		unsigned int s = 5 * sizeof(int) + 1 + 7 * sizeof(float);
		return s;
	}

	// Header access  methods
	static int offset_;
	inline static int& offset() {return offset_;}
	inline static hdr_geo* access(const Packet* p) {
		return (hdr_geo*) p->access(offset_);
	}
};

struct hdr_geo_hello {
	u_int8_t type;
	float x_;
	float y_;
	float z_;
	inline int size() {
		int s = 3*sizeof(float) + sizeof(u_int8_t);
		return s;
	}
};

struct hdr_geo_data {
	u_int8_t type;
	inline int size() {
		return sizeof(u_int8_t);
	}
};

union hdr_all_geo {
	hdr_geo gg;
	hdr_geo_hello ghello;
	hdr_geo_data gdata;
};

#endif 