#pragma once
#include "CapsuleMath.h"


bool CapsuleMath::findCommonIntersection(CCapsule c, CCapsule c2, Vector& vec) {
	return c.InterestionOther(c2, vec);
}


#include "../../../Interfaces.h"
#include "../../OsirisSDK/Surface.h"
#include "../../../Hacks/OTHER/Debug.h"
#include "ScreenMath.h"

struct color {
	int r, g, b, a;
};
struct ncoord {
	float x, y = 0;
};



static void DrawPoint(Vector Point) {

	interfaces->surface->setDrawColor(0, 0, 0, 255);
	Debug::coords DrawVec;
	float x, y;
	if (!ScreenMath::worldToScreen(Point, x, y))
		return;

	interfaces->surface->drawCircle(x, y, 1, 5);

}
static void DrawPoint(Vector Point, color col) {

	interfaces->surface->setDrawColor(col.r, col.g, col.b, col.a);
	Debug::coords DrawVec;
	float x, y;
	if (!ScreenMath::worldToScreen(Point, x, y))
		return;

	interfaces->surface->drawCircle(x, y, 1, 5);

}
static void DrawPoint(ncoord Point, color col) {

	interfaces->surface->setDrawColor(col.r, col.g, col.b, col.a);
	interfaces->surface->drawCircle(Point.x, Point.y, 1, 5);

}

static float CalcDistance(float x, float y, float x2, float y2) {
	return std::sqrt(((x - x2) * (x - x2)) + ((y - y2) * (y - y2)));
}

static float calcRadius(Vector Center, float Radius) {
	float x, y, x2, y2;
	if (!ScreenMath::worldToScreen(Center, x, y))
		return 0;

	if (!ScreenMath::worldToScreen(Center+Radius, x2, y2))
		return 0;


	return CalcDistance(x, y, x2, y2);
}


static ncoord Intersection2D(CSphere sp1, CSphere sp2) {
	float spR1 = (calcRadius(sp1.getCenter(), sp1.getRadius()) / 2);
	float spR2 = (calcRadius(sp2.getCenter(), sp2.getRadius()) / 2);

	ncoord spC1;
	ncoord spC2;

	if (!ScreenMath::worldToScreen(sp1.getCenter(), spC1.x, spC1.y))
		return {0,0};

	if (!ScreenMath::worldToScreen(sp2.getCenter(), spC2.x, spC2.y))
		return {0,0};

	float distance = CalcDistance(spC1.x, spC1.y, spC2.x, spC2.y);


	if (distance > spR1 + spR2)
		return {0,0};

	if (distance < std::abs(spR1 - spR2)) {
		return spC1;
		//return { 0,0 };
	}


	float a = (spR1 * spR1 - spR2 * spR2 + distance * distance) / (2 * distance);
	float h = sqrt(spR1 * spR1 - a * a);

	//Point P0(x, y);
	//Point P1(c.x, c.y);
	//Point scale(float s) {
	//	return Point(x * s, y * s);
	//}

	ncoord P2;
	P2.x = ((spC2.x - spC1.x) * (a / distance)) + spC1.x;     //  P1.sub(P0).scale(a / d).add(P0);
	P2.y = ((spC2.y - spC1.y) * (a / distance)) + spC1.y;     //  P1.sub(P0).scale(a / d).add(P0);
	ncoord P0 = spC1;
	ncoord P1 = spC2;
	float d = distance;
	float x3, y3, x4, y4;
	x3 = P2.x + h * (P1.y - P0.y) / d;
	y3 = P2.y - h * (P1.x - P0.x) / d;
	x4 = P2.x - h * (P1.y - P0.y) / d;
	y4 = P2.y + h * (P1.x - P0.x) / d;

	return(ncoord{ x3, y3 });
}

static void DrawSphere(Vector Point, float radius, color col) {


	Debug::coords DrawVec;
	float x, y;
	if (!ScreenMath::worldToScreen(Point, x, y))
		return;
	interfaces->surface->setDrawColor(col.r/2, col.g/2, col.b/2, col.a/2);
	interfaces->surface->drawCircle(x, y, 1, (calcRadius(Point, radius))); // /2
	interfaces->surface->setDrawColor(col.r, col.g, col.b, col.a);
	interfaces->surface->drawCircle(x, y, 1, 2);
}

/*
The distance between two points is the length of the path connecting them. 
The shortest path distance is a straight line. 
In a 2 dimensional plane, the distance between points (X1, Y1) and (X2, Y2) is given by the Pythagorean theorem:
d=(x2−x1)2+(y2−y1)2

*/





bool SphereSphereIntersection(CSphere sp1, CSphere sp2, matrix3x4 bone1, matrix3x4 bone2, Vector& vec) {
	Vector Delta = sp1.getCenter().transform(bone1) - sp2.getCenter().transform(bone2);

	Vector SpC1 = sp1.getCenter();
	Vector SpC2 = sp1.getCenter() + Delta;
	
	float Distance2D = SpC1.distTo(SpC2);



	float combinedRadius = (sp1.getRadius() + sp2.getRadius());

	if (Distance2D > combinedRadius)
		return false;

	if ((std::abs(Delta.x) > combinedRadius) || (std::abs(Delta.y) > combinedRadius) || (std::abs(Delta.z) > combinedRadius))
		return false;

	if (std::abs((sp1.getRadius() - sp2.getRadius())) > Distance2D){
		//return true;
		return false;
	}

	vec = (sp1.getCenter() - (Delta / 2)).transform(bone1);
	return true;
}

bool CapsuleMath::findCommonIntersection(StudioBbox* box, matrix3x4 bone1, matrix3x4 bone2, Vector& vec) {

	CCapsule Cap1;
	CCapsule Cap2;

	Cap1.setup(box->bbMin, box->bbMax, box->capsuleRadius);
	Cap2.setup(box->bbMin, box->bbMax, box->capsuleRadius);
	color col;
	for (CSphere sp : Cap1.GetSpheres()) {
		//col.r = 0;
		//col.g = 255;
		//col.b = 0;
		//col.a = 255;
		//DrawSphere(sp.getCenter().transform(bone1), sp.getRadius(), col);
		//col.r = 255;
		//col.g = 0;
		//col.b = 0;
		//col.a = 255;
		for (CSphere sp2 : Cap2.GetSpheres()) {
			//DrawSphere(sp2.getCenter().transform(bone2), sp2.getRadius(), col);
			if(SphereSphereIntersection(sp, sp2, bone1, bone2, vec)){
				DrawPoint(vec);
				return true;
			}
		}
	}


	return false;
}





bool CapsuleMath::HitBoxRayIntersection(Vector StartPoint, Vector EndPoint, StudioBbox* box, matrix3x4 bone) {
	Vector vec;
	CustomRay Ray;
	Ray.Start = StartPoint;
	Ray.Direction = EndPoint;
	Ray.Destination = Ray.Direction;
	Ray.Delta = Ray.Destination - Ray.Start;
	if (box)
	{
		CCapsule newCap(box);

		for (CSphere sp : newCap.GetSpheres()) {
			CustomRay New; // = sp.getCenter().transform(Bones[box->bone]);
			New.Start = Ray.Start - sp.getCenter().transform(bone);
			New.Direction = Ray.Direction - sp.getCenter().transform(bone);
			New.Destination = New.Direction;
			if (sp.intersectsRay(New, vec)) {
				//DrawPoint(vec.transform(bone), { 0,255,0,255 });
				//DrawSphere(sp.getCenter().transform(bone), sp.getRadius(), { 0,255,255,255 });
				return true;
			}
		}

	}
	return false;
	//memory->conColorMsg({255,0,0,255}, "Point")
}

bool CapsuleMath::HitBoxRayIntersection(Vector StartPoint, Vector EndPoint, StudioBbox box, matrix3x4 bone) {
	Vector vec;
	CustomRay Ray;
	Ray.Start = StartPoint;
	Ray.Direction = EndPoint;
	Ray.Destination = Ray.Direction;
	Ray.Delta = Ray.Destination - Ray.Start;
	if (&box)
	{
		CCapsule newCap(&box);

		for (CSphere sp : newCap.GetSpheres()) {
			CustomRay New; // = sp.getCenter().transform(Bones[box->bone]);
			New.Start = Ray.Start - sp.getCenter().transform(bone);
			New.Direction = Ray.Direction - sp.getCenter().transform(bone);
			New.Destination = New.Direction;
			if (sp.intersectsRay(New, vec)) {
				//DrawPoint(vec.transform(bone), { 0,255,0,255 });
				//DrawSphere(sp.getCenter().transform(bone), sp.getRadius(), { 0,255,255,255 });
				return true;
			}
		}

	}
	return false;
	//memory->conColorMsg({255,0,0,255}, "Point")

}

#include <future>

/* TODO: Get This working*/
bool CapsuleMath::HitBoxRayIntersectionAsync(Vector StartPoint, Vector EndPoint, StudioBbox box, matrix3x4 bone) {
	Vector vec;
	CustomRay Ray;
	Ray.Start = StartPoint;
	Ray.Direction = EndPoint;
	Ray.Destination = Ray.Direction;
	Ray.Delta = Ray.Destination - Ray.Start;



	std::vector<std::shared_future<bool>> Calcs;


	if (&box)
	{
		CCapsule newCap(&box);
		int i = 0;
		for (CSphere sp : newCap.GetSpheres()) {
			CustomRay New; // = sp.getCenter().transform(Bones[box->bone]);
			New.Start = Ray.Start - sp.getCenter().transform(bone);
			New.Direction = Ray.Direction - sp.getCenter().transform(bone);
			New.Destination = New.Direction;
			Calcs.push_back(std::async(std::launch::async, SphereMath::intersectsRay, sp, New));
			i++;
			if (i >= 3) {
				for (std::shared_future<bool> Calc : Calcs) {
					Calc.wait();
					if (Calc.get()) {
						return true;
					}
				}
				Calcs.clear();
				i = 0;
			}

		}
	}


	for (std::shared_future<bool> Calc : Calcs) {
		Calc.wait();
		if (Calc.get())
			return true;
	}


	return false;
	//memory->conColorMsg({255,0,0,255}, "Point")

}

bool CapsuleMath::HitBoxRayIntersectionA(Vector StartPoint, Vector EndPoint, StudioBbox box, matrix3x4 bone) {
	return CapsuleMath::HitBoxRayIntersectionAsync(StartPoint, EndPoint, box, bone);
}

bool CapsuleMath::findCommonIntersection(StudioBbox* box, matrix3x4 bone1, matrix3x4 bone2, matrix3x4 bone3, Vector& vec) {

	return true;

}

#include "../../OsirisSDK/LocalPlayer.h"
#include "../../OsirisSDK/Entity.h"
#include "../EntityListCull.hpp"
#include "../../../Hacks/RageBot/Multipoints.h"
void CapsuleMath::TestRayIntesection() {

}
/*
	CCapsule Cap1;
	CCapsule Cap2;

	Cap1.setup(box->bbMin.transform(bone1), box->bbMax.transform(bone1), box->capsuleRadius);
	Cap2.setup(box->bbMin.transform(bone2), box->bbMax.transform(bone2), box->capsuleRadius);
	color col;
	

	col.r = 255;
	col.g = 0;
	col.b = 0;
	col.a = 255;
	for (CSphere sp : Cap1.GetSpheres()) {
		DrawSphere(sp.getCenter(), sp.getRadius(), col);
	}
	col.r = 0;
	col.g = 255;
	col.b = 0;
	col.a = 255;

	for (CSphere sp : Cap2.GetSpheres()) {
		DrawSphere(sp.getCenter(), sp.getRadius(), col);
	}
	
col.r = 255;
col.b = 255;
col.a = 255;
col.g = 0;

for (CSphere sp : Cap1.GetSpheres()) {
	for (CSphere sp2 : Cap2.GetSpheres()) {
		ncoord Point = Intersection2D(sp, sp2);
		if (Point.x != 0) {
			DrawPoint(Point, col);
		}
	}
}

if (Cap1.InterestionOther(Cap2, vec))
return true;

return false;

*/