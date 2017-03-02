#include <vector>
#include <stdlib.h>
#include <cmath>
#include <random.h>
#include <trace.h>

/*
static double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}
*/

/* Computes the distance between two nodes (1) and (2) */
float dist(float, float, float, float, float, float);

/* Computes the angle formed by the two lines (1-p) and (2-p) */
float angle(float, float, float, 
	float, float, float, 
	float, float, float);

/* Computes the projected distance between nodes (p) and (2)
 * on the line (1-2) */
float projDist(float, float, float, 
	float, float, float, 
	float, float, float);


int orientation(float, float, float, float, float, float);