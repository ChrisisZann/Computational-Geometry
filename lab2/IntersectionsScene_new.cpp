#include "IntersectionsScene.h"
#include <utils.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>

/* Construct - Load  - Setup */

IntersectionsScene::IntersectionsScene()
{
	m_bg_col = Colour(0x44, 0x44, 0x44);
	reset();
}

void IntersectionsScene::reset()
{
	Scene::reset();

	// Clear everything
	m_canvas_0.clear();
	m_canvas_1.clear();
	m_canvas_2.clear();
	m_canvas_3.clear();

	// Divide window to Tasks
	m_bound_vertical.Set(C2DPoint(0, -3000), C2DPoint(0, 3000));
	m_bound_horizontal.Set(C2DPoint(4000, 0), C2DPoint(-4000, 0));
	m_canvas_0.add(m_bound_horizontal, Colour::black);
	m_canvas_0.add(m_bound_vertical, Colour::black);

	// Setup Task 1:
	{
		C2DPoint a1(-300, 100);
		C2DPoint a2(-100, 200);
		C2DPoint b1(-350, 230);
		C2DPoint b2(-50, 50);

		m_line_1 = C2DLine(a1, a2);
		m_line_2 = C2DLine(b1, b2);
		m_canvas_0.add(a1, Colour::orange);
		m_canvas_0.add(a2, Colour::orange);
		m_canvas_0.add(m_line_1, Colour::orange);
		m_canvas_1.add(b1, Colour::cyan);
		m_canvas_1.add(b2, Colour::cyan);
		m_canvas_1.add(m_line_2, Colour::cyan);
	}


	// Setup Task 2:
	{
		C2DPoint c1(166, 112);
		C2DPoint c2(290, 150);

		m_circle_1 = C2DCircle(c1, 80);
		m_circle_2 = C2DCircle(c2, 60);
		m_canvas_0.add(c1, Colour::orange);
		m_canvas_0.add(m_circle_1, Colour::orange);
		m_canvas_2.add(c2, Colour::cyan);
		m_canvas_2.add(m_circle_2, Colour::cyan);
	}

	// Task 3:
	{
		C2DPoint t1a(-300, -50);
		C2DPoint t1b(-40, -45);
		C2DPoint t1c(-70, -170);
		m_triangle_1 = C2DTriangle(t1a, t1b, t1c);

		C2DPoint t2a(-197, -266);
		C2DPoint t2b(-368, -136);
		C2DPoint t2c(-108, -76);
		m_triangle_2 = C2DTriangle(t2a, t2b, t2c);

		m_canvas_0.add(m_triangle_1, Colour::orange);
		m_canvas_3.add(m_triangle_2, Colour::cyan);
	}

	Task1(m_line_2.GetPointTo());
	Task2(m_circle_2.GetCentre());
	Task3(m_triangle_2.GetPoint3());
}

/* UI Handling */

void IntersectionsScene::mousePressed(int x, int y, int modif)
{
	Scene::mousePressed(x, y, modif);
	echo(x);
	echo(y);
	const C2DPoint p(x, y);
	if (m_bound_horizontal.IsOnRight(p) && !m_bound_vertical.IsOnRight(p)) Task1(p);
	if (m_bound_horizontal.IsOnRight(p) && m_bound_vertical.IsOnRight(p)) Task2(p);
	if (!m_bound_horizontal.IsOnRight(p) && !m_bound_vertical.IsOnRight(p)) Task3(p);
}

void IntersectionsScene::mouseMoved(int x, int y, int modif)
{
	Scene::mouseMoved(x, y, modif);
	mousePressed(x, y, modif);
}

/* Tasks */

void IntersectionsScene::Task1(const C2DPoint &p)
{
	C2DPoint p1 = m_line_2.GetPointFrom();   // To arxiko simeio paremenei idio.
	m_line_2 = C2DLine(p1, p);   // To teliko simeio tis grammis akolouthei to mouse.

	m_canvas_1.clear();
	m_canvas_1.add(p, Colour::cyan);
	m_canvas_1.add(p1, Colour::cyan);
	m_canvas_1.add(m_line_2, Colour::cyan);

	C2DPoint i;
	IntersectionsScene::line_intersection(m_line_1, m_line_2, i);

	m_canvas_1.add(i, Colour::red);
}



void IntersectionsScene::Task2(const C2DPoint &p)
{
	m_circle_2.SetCentre(p);
	m_canvas_2.clear();
	m_canvas_2.add(p, Colour::cyan);
	m_canvas_2.add(m_circle_2, Colour::cyan);

	const double x1 = m_circle_1.GetCentre().x;
	const double y1 = m_circle_1.GetCentre().y;
	const double r1 = m_circle_1.GetRadius();
	const double x2 = m_circle_2.GetCentre().x;
	const double y2 = m_circle_2.GetCentre().y;
	const double r2 = m_circle_2.GetRadius();

	double d = sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
	double l = ( ((r1*r1) + (d*d) - (r2*r2)) / (2*d) );
	double h = sqrt((r1*r1) - (l*l));

	double i1_x = ((l / d)*(x2 - x1)) + ((h / d)*(y2 - y1)) + x1;
	double i1_y = ((l / d)*(y2 - y1)) - ((h / d)*(x2 - x1)) + y1;
	double i2_x = ((l / d)*(x2 - x1)) - ((h / d)*(y2 - y1)) + x1;
	double i2_y = ((l / d)*(y2 - y1)) + ((h / d)*(x2 - x1)) + y1;

	double i1_x_f, i1_y_f, i2_x_f, i2_y_f;

	if (d > (r1 + r2) || d < (r1 - r2)){
		i1_x_f = 0;
		i1_y_f = 0;
		i2_x_f = 0;
		i2_y_f = 0;
	}
	else if (d == (r1 + r2) || d == (r1 - r2)){
		//one point
		i1_x_f = (i1_x + i2_x)/2;
		i1_y_f = (i2_y + i2_y)/2;
		i2_x_f = 0;
		i2_y_f = 0;
	}
	else{
		i1_x_f = i1_x;
		i1_y_f = i1_y;
		i2_x_f = i2_x;
		i2_y_f = i2_y;
	}

	C2DPoint i1(i1_x_f, i1_y_f);
	C2DPoint i2(i2_x_f, i2_y_f);

	m_canvas_2.add(i1, Colour::red);
	m_canvas_2.add(i2, Colour::red);
}

void IntersectionsScene::Task3(const C2DPoint &p)
{
	const C2DPoint &p1 = m_triangle_2.GetPoint1();
	const C2DPoint &p2 = m_triangle_2.GetPoint2();
	m_triangle_2.Set(p1, p2, p);
	m_canvas_3.clear();
	m_canvas_3.add(m_triangle_2, Colour::cyan);

	C2DPoint tr1_p1 = m_triangle_1.GetPoint1();
	C2DPoint tr1_p2 = m_triangle_1.GetPoint2();
	C2DPoint tr1_p3 = m_triangle_1.GetPoint3();

	C2DPoint tr2_p1 = m_triangle_2.GetPoint1();
	C2DPoint tr2_p2 = m_triangle_2.GetPoint2();
	C2DPoint tr2_p3 = m_triangle_2.GetPoint3();

	C2DLine tr1_l1 =  C2DLine(tr1_p1, tr1_p3);
	C2DLine tr1_l2 =  C2DLine(tr1_p2, tr1_p3);
	C2DLine tr1_l3 =  C2DLine(tr1_p1, tr1_p2);

	C2DLine tr2_l1 =  C2DLine(tr2_p1, tr2_p3);
	C2DLine tr2_l2 =  C2DLine(tr2_p2, tr2_p3);

	C2DPoint pp1;
	C2DPoint pp2;
	C2DPoint pp3;
	C2DPoint pp4;
	C2DPoint pp5;
	C2DPoint pp6;

	IntersectionsScene::line_intersection(tr2_l1, tr1_l1, pp1);
	IntersectionsScene::line_intersection(tr2_l1, tr1_l2, pp2);
	IntersectionsScene::line_intersection(tr2_l1, tr1_l3, pp3);
	IntersectionsScene::line_intersection(tr2_l2, tr1_l1, pp4);
	IntersectionsScene::line_intersection(tr2_l2, tr1_l2, pp5);
	IntersectionsScene::line_intersection(tr2_l2, tr1_l3, pp6);

	C2DPointSet pts_set;
	
	if(pp1.x != 0 && pp1.y != 0) {pts_set.AddCopy(pp1);}

	if(pp2.x != 0 && pp2.y != 0) {pts_set.AddCopy(pp2);}
	
	if(pp3.x != 0 && pp3.y != 0) {pts_set.AddCopy(pp3);}
	
	if(pp4.x != 0 && pp4.y != 0) {pts_set.AddCopy(pp4);}
	
	if(pp5.x != 0 && pp5.y != 0) {pts_set.AddCopy(pp5);}
	
	if(pp6.x != 0 && pp6.y != 0) {pts_set.AddCopy(pp6);}

	if(m_triangle_1.Contains(p)){pts_set.AddCopy(p);m_canvas_3.add(p, Colour::white);}

	if (m_triangle_2.Contains(tr1_p1)){pts_set.AddCopy(tr1_p1);}
	
	if (m_triangle_2.Contains(tr1_p2)){pts_set.AddCopy(tr1_p2);}
	
	if (m_triangle_2.Contains(tr1_p3)){pts_set.AddCopy(tr1_p3);}



	C2DPolygon myPoly = C2DPolygon(pts_set, true);
	C2DPointSet orderedPts;
	myPoly.GetPointsCopy(orderedPts);

	if(pts_set.size() != 0){

		C2DTriangle my_tri;
		for (int i=2 ; i<pts_set.size() ; i++){
			
			my_tri = C2DTriangle(*pts_set.GetAt(i-2), *pts_set.GetAt(i-1), *pts_set.GetAt(i));
			m_canvas_3.add(my_tri, Colour::cyan, true);
		}

		for (int i=2 ; i<orderedPts.size() ; i++){
			
			my_tri = C2DTriangle(*orderedPts.GetAt(i-2), *orderedPts.GetAt(i-1), *orderedPts.GetAt(i));
			m_canvas_3.add(my_tri, Colour::cyan, true);
		}
	}
	/**/
}

void IntersectionsScene::line_intersection(C2DLine l1, C2DLine l2, C2DPoint &p_i){
	/***********************************************/
	double x1 = l1.GetPointFrom().x;
	double y1 = l1.GetPointFrom().y;
	double x2 = l1.GetPointTo().x;
	double y2 = l1.GetPointTo().y;

	double a1 = (y2 - y1) / (x2 - x1);
	double b1 = y1 - a1 * x1;
	/***********************************************/

	double x3 = l2.GetPointFrom().x;
	double y3 = l2.GetPointFrom().y;
	double x4 = l2.GetPointTo().x;
	double y4 = l2.GetPointTo().y;

	double a2 = (y4 - y3) / (x4 - x3);
	double b2 = y3 - a2 * x3;
	/***********************************************/

	double i_x = (b1 - b2) / (a2 - a1);
	double i_y = a1*i_x + b1;
	/***********************************************/

	double i_x_final, i_y_final;


	if ( l1.Crosses(l2) ){
		i_x_final = i_x;
		i_y_final = i_y;
	}
	else{
		i_x_final = 0;
		i_y_final = 0;
	}
	/***********************************************/

	p_i = C2DPoint(i_x_final, i_y_final);
}

/* Drawing */

void IntersectionsScene::draw()
{
	enterPixelMode();

	m_canvas_0.draw();
	m_canvas_1.draw();
	m_canvas_2.draw();
	m_canvas_3.draw();
}

/* Application Entry Point */

int main(int argc, char* argv[])
{
	return vvr::mainLoop(argc, argv, new IntersectionsScene);
}
