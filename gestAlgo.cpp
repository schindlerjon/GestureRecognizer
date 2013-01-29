/*
 * Creator: Jonathan Schindler
 * $1 Gesture Recognizer
 * 
 */


#include <vector>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include "gestAlgo.h"

using namespace std;

vector<Vector2D> resample( vector<Vector2D> points, int n){
	
	float I = pathLength(points)/(n-1) - 0.0001;  // Interval Length
	float D = 0;	// Distance
	vector<Vector2D> newPoints;
	newPoints.push_back(points.at(0));

	for(int i = 1; i < (int) points.size(); i++){
		float d = distancePt(points.at(i-1),points.at(i));
		if( (D + d) > I){
			float qx = points.at(i-1).x + ((I - D)/d)*(points.at(i).x - points.at(i-1).x);
			float qy = points.at(i-1).y + ((I - D)/d)*(points.at(i).y - points.at(i-1).y);
			Vector2D q = Vector2D(qx,qy);
			newPoints.push_back(q);
			points = insertPt(points, i, q);
			D = 0;
			}
		else
			D += d;
	}
	
	return newPoints;
}

// Places a 
vector<Vector2D> insertPt(vector<Vector2D> p, int i, Vector2D q){
	vector<Vector2D> r;
	for(int x=0;x < i;x++)
		r.push_back(p[x]);

	r.push_back(q);
	
	for(int x = i; x < (int)p.size(); x++)
		r.push_back(p[x]);
	
	return r;
}

// Returns the total length of the path
float pathLength(vector<Vector2D> points){
	float path = 0; 
	for(int x=0; x < (int) points.size()-1; x++)
		path += distancePt(points[x], points[x+1]);
	return path;
}

// Returns the distance between two points
float distancePt(Vector2D p1, Vector2D p2){
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	return sqrt((dx * dx)+(dy * dy));
}

vector<Vector2D> scaleTo(vector<Vector2D> points, int size)
{
	vector<Vector2D> newPoints;

	// Get the Max and Min values to create a Rectangle
	Vector2D max = (Vector2D) points.at(0);
	Vector2D min = (Vector2D) points.at(0);
	for(int i=1; i < (int) points.size();i++){
		if(points.at(i).x > max.x)
			max.x = points.at(i).x;
		if(points.at(i).y > max.y)
			max.y = points.at(i).y;
		if(points.at(i).x < min.x)
			min.x = points.at(i).y;
		if(points.at(i).y < min.y)
			min.y = points.at(i).y;
	}

	for(int i=0; i < (int) points.size(); i++){
		float qx = (float) (points.at(i).x * size/ (max.x - min.x));
		float qy = (float) (points.at(i).y * size/ (max.y - min.y)); 
		Vector2D q = Vector2D(qx,qy);
		newPoints.push_back(q);
	}
	return newPoints;
}

vector<Vector2D> translateTo(vector<Vector2D> points, Vector2D k)
{
	Vector2D q;
	vector<Vector2D> newPoints;
	Vector2D c = Centroid(points);
	for( int i=0; i< (int)points.size(); i++){
		q.x = points.at(i).x + k.x - c.x;
		q.y = points.at(i).y + k.y - c.y;
		newPoints.push_back(q);
	}
	return newPoints;
}

Vector2D Centroid(vector<Vector2D> points)
{
	float xtot = 0;
	float ytot = 0;
	for(int i =0; i < (int) points.size(); i++){
		xtot += points.at(i).x;
		ytot += points.at(i).y;
	}
	xtot /= (float)points.size();
	ytot /= (float)points.size();
	return Vector2D(xtot, ytot);
}

vector<Vector2D> rotateBy(vector<Vector2D> points, double angle)
{
	Vector2D q;
	Vector2D c = Centroid(points);
	vector<Vector2D> newPoints;
	for(int i=0; i < (int) points.size(); i++)
	{
		q.x = (float) ((points.at(i).x - c.x)*cos(angle)-(points.at(i).y-c.y)*sin(angle) + c.x);
		q.y = (float) ((points.at(i).x - c.x)*sin(angle)-(points.at(i).y-c.y)*cos(angle) + c.y);
		newPoints.push_back(q);
	}
	return newPoints;
}

RecordScore recognize(vector<Vector2D> points, vector<Gesture> templates, int size)
{
	float b = 1000000000;
	Gesture T;
	for(int i = 0; i < (int) templates.size();i++){
		float d = distanceBestAngle(points, templates.at(i), -1, +1, .001);

		
	//	float m2 = sqrt( (float)(size*size+size*size));
	//	float score2 = (float) (1.0f - (b /(0.5 * m2)));
	//	cout << templates[i].name << ": "<< score2;

		if ( d < b ){
			b = d;
			T = templates.at(i);
		}
	}
	float m1 = sqrt( (float)(size*size+size*size));
	float score = (float) (1.0f - (b /(0.5f * m1)));
	RecordScore r;
	r.T = T;
	r.score = score;
	return r;
}

float distanceBestAngle(vector<Vector2D> points, Gesture T, double Qa, double Qb, double Qd)
{
	float p = 0.5f * (-1.0f + sqrt((float)5.0f));
	double x1 = p*Qa + (1.0f - p)*Qb;
	float f1 = distanceAtAngle(points, T, x1);
	double x2 = (1.0f - p)*Qa + p*Qb;
	float f2 = distanceAtAngle(points, T, x2);

	while ( abs(Qb - Qa) > Qd){
		if(f1 < f2){
			Qb = x2;
			x2 = x1;
			f2 = f1;
			x1 = p*Qa+(1.0f-p)*Qb;
			f1 = distanceAtAngle(points, T, x1);
		}else{
			Qa = x1;
			x1 = x2;
			f1 = f2;
			x2 = (1.0f-p)*Qa+p*Qb;
			f2 = distanceAtAngle(points, T, x2);
		}
	}
	return f1<f2 ? f1:f2;
}

float distanceAtAngle(vector<Vector2D> points, Gesture T, double theta)
{
	vector<Vector2D> newPoints = rotateBy(points, theta);
	float d = pathDistance(newPoints, T.data);
	return d;
}

float pathDistance(vector<Vector2D> a, vector<Vector2D> b)
{
	float d = 0;
	for(int i = 0; i < (int) a.size(); i++)
		d += distancePt(a.at(i), b.at(i));
	return d/a.size();
}


