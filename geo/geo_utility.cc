#include "geo_utility.h"

/* Computes the distance between two nodes (1) and (2) */
float dist(float x1, float y1, float z1, float x2, float y2, float z2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));
}

/* Computes the angle formed by the two lines (1-p) and (2-p) */
float angle(float x1, float y1, float z1, 
	float x2, float y2, float z2, 
	float xp, float yp, float zp)
{
	float ux, uy, uz, vx, vy, vz;
	ux = x2-x1;
	uy = y2-y1;
	uz = z2-z1;
	vx = xp-x1;
	vy = yp-y1;
	vz = zp-z1;
	float a = ux*vx+uy*vy+uz*vz;
	float modU = sqrt(pow(ux, 2) + pow(uy, 2) + pow(uz, 2));
	float modV = sqrt(pow(vx, 2) + pow(vy, 2) + pow(vz, 2));
	float cosfi = a / (modU * modV);
	return acos(cosfi);
}

/* Computes the projected distance between nodes (p) and (2)
 * on the line (1-2) */
float projDist(float x1, float y1, float z1, 
	float x2, float y2, float z2, 
	float xpa, float ypa, float zpa)
{
	float xp = xpa - x1;
	float yp = ypa - y1;
	float zp = zpa - z1;
	float a, b, c;
	a = (x2-x1) / dist(x1,y1,z1,x2,y2,z2);
	b = (y2-y1) / dist(x1,y1,z1,x2,y2,z2);
	c = (z2-z1) / dist(x1,y1,z1,x2,y2,z2);
	float xpp, ypp, zpp;
	xpp = x1+(a*xp+b*yp+c*zp)*a;
	ypp = y1+(a*xp+b*yp+c*zp)*b;
	zpp = z1+(a*xp+b*yp+c*zp)*c;
	//printf("(%f,%f,%f)\n", xpp,ypp,zpp);

	return dist(xpp, ypp, zpp, x2, y2, z2);
	return 1;
}

int orientation(float x1, float y1, float x2, float y2, float xp, float yp)
{
	float val;
    val = (y2 - y1) * (xp - x2) - (x2 - x1) * (yp - y2);
    //fprintf(stderr, "val = %f\n", val);
    if (abs(val) <= 0.001) return 0;  // colinear (prevent floating error)
    return (val > 0)? 1: 2; // clock or counterclock wise
}