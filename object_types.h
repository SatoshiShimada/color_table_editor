#ifndef OBJECT_TYPES_H
#define OBJECT_TYPES_H

#include "hpl_types.h"

typedef struct object_pos_t {
	object_pos_t(): x(0), y(0), th(0.0), type(OBJECT_UNKNOWN), ownership(OWNERSHIP_ANY) { }
	object_pos_t(int xt, int yt) : x(xt), y(yt), th(0.0), type(OBJECT_UNKNOWN), ownership(OWNERSHIP_ANY) { }
	object_pos_t(int xt, int yt, double tht) : x(xt), y(yt), th(tht), type(OBJECT_UNKNOWN), ownership(OWNERSHIP_ANY) { }
	object_pos_t(int xt, int yt, int typet) : x(xt), y(yt), th(0.0), type(typet), ownership(OWNERSHIP_ANY) { }
	object_pos_t(int xt, int yt, int typet, int townership) : x(xt), y(yt), th(0.0), type(typet), ownership(townership) { }
	object_pos_t(int xt, int yt, double tht, int typet) : x(xt), y(yt), th(tht), type(typet), ownership(OWNERSHIP_ANY) { }
	int x, y;
	double th;
	int type; // object type such as ball, goal post or robots.
	int ownership; // ours, theirs or any.
} object_pos;

#endif // OBJECT_TYPES_H

