/*
 * Creator: Jonathan Schindler
 * $1 Gesture Recognizer
 * 
 */

#include <vector>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include "Vector2D.h"

using namespace std;

struct Gesture {
	string name;
	vector<Vector2D> data;
	bool directionality;
	int samples;
};

struct RecordScore {
	Gesture T;
	float score;
};

// *** RESAMPLE
vector<Vector2D> resample( vector<Vector2D> points, int n);

vector<Vector2D> insertPt(vector<Vector2D> p, int i, Vector2D q);

// Returns the total length of the path
float pathLength(vector<Vector2D> points);

// Returns the distance between two points
float distancePt(Vector2D p1, Vector2D p2);


// *** SCALE
vector<Vector2D> scaleTo(vector<Vector2D> points, int size);

// *** TRANSLATE
vector<Vector2D> translateTo(vector<Vector2D> points, Vector2D k);

// Finds the CENTROID of a list of points
Vector2D Centroid(vector<Vector2D> points);

vector<Vector2D> rotateBy( vector<Vector2D> points, double angle);

// *** RECOGNIZE 
RecordScore recognize(vector<Vector2D> points, vector<Gesture> templates, int size);

float distanceBestAngle(vector<Vector2D> points, Gesture templates, double Qa, double Qb, double Qd);
float distanceAtAngle(vector<Vector2D> points, Gesture templates, double theta);
float pathDistance(vector<Vector2D> a, vector<Vector2D> b); 




