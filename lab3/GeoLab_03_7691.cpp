#include "GeoLab_03_7691.h"
#include <utils.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <ctime>


TriangulationScene::TriangulationScene()
{
    m_bg_col = Colour(0x44, 0x44, 0x44);
    reset();
}

void TriangulationScene::reset()
{
    Scene::reset();

    m_show_step = 0;
    m_canvas.clear();
    m_tris.clear();
    m_pts.DeleteAll();
    m_pts.Add(new C2DPoint(-350, -200));
    m_pts.Add(new C2DPoint(350, -200));
    m_pts.Add(new C2DPoint(0, 300));
    m_tris.push_back(Tri(m_pts.GetAt(0), m_pts.GetAt(1), m_pts.GetAt(2)));
}

void TriangulationScene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);
    handleNewPoint(new C2DPoint(x, y));
}

void TriangulationScene::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);
    if (m_pts.GetLast()->Distance(C2DPoint(x, y)) > 20)
        handleNewPoint(new C2DPoint(x, y));
}

void TriangulationScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::LEFT)
    {
        if (m_show_step > 0)
            --m_show_step;

        if (m_canvas.isAtStart()) {

        }
        else {
            m_canvas.prev();
        }
    }
    else if (dir == vvr::RIGHT)
    {
        if (m_show_step < m_canvas.size()-1)
            ++m_show_step;

        if (m_canvas.isAtEnd()) {

        }
        else {
            m_canvas.next();
        }
    }
}

void TriangulationScene::handleNewPoint(C2DPoint *p)
{
    // Check whether a point already exists in the same coords.
    for (size_t i = 0; i < m_pts.size(); i++)
    {
        if (m_pts.GetAt(i)->x == p->x &&
            m_pts.GetAt(i)->y == p->y)
        {
            delete p;
            return;
        }
    }

    // Find enclosing triangle.
    unsigned encl_i, encl_count=0;
    for (int i = 0; i < m_tris.size(); i++) {
        if (m_tris[i].to_C2D().Contains(*p)) {
            encl_count++;
            encl_i = i;
        }
    }

    // If no enclosing triangle was found.
    if (encl_count != 1) {
        delete p;
        return;
    }

    // Create the 3 subdivision triangles.
    Tri new_tris[3];
    new_tris[0] = Tri(p, m_tris[encl_i].v1, m_tris[encl_i].v2);
    new_tris[1] = Tri(p, m_tris[encl_i].v2, m_tris[encl_i].v3);
    new_tris[2] = Tri(p, m_tris[encl_i].v3, m_tris[encl_i].v1);

    // Check if any of the 3 triangles are colinear.
    for (int i = 0; i < 3; i++) {
        if (new_tris[i].to_C2D().Collinear())
            return;
    }

    // Remove the enclosing triangle. It will be replaced by its subdivision triangles.
    m_tris.erase(m_tris.begin() + encl_i);
    m_pts.Add(p);
    m_canvas.clear();

    // Display the 3 the triangles.
    for (int i = 0; i < 3; i++) {
        m_canvas.add(new_tris[i].to_C2D(), Colour(255, 50, 50), true);
        m_canvas.add(new_tris[i].to_C2D(), Colour::black, false);
    }

    vector<Tri> tris_adj;

	// Chech the 3 new triangles for Delaunay violations.
    for (int i = 0; i < 3; i++)
    {
        new_tris[i];                                    // To trigwno ypo elegxo Delaunay.
        C2DTriangle tri_check = new_tris[i].to_C2D();  // C2DTriangle anaparastasi tou new_tris[i].
        Tri *tri_adjacent=NULL;                       // The adjacent triangle
        C2DPoint *v_opposite=NULL;                   // The opposite vertex of the adjacent triangle

        if (!IsDelaunay(tri_check, m_pts))
        {
            m_canvas.add(GetCircumCircle(tri_check), Colour::magenta, false);

            C2DPoint *v1 = new_tris[i].v1;
            C2DPoint *v2 = new_tris[i].v2;
            C2DPoint *v3 = new_tris[i].v3;
            
			bool adj_exists = FindAdjacentTriangle(m_tris, v2, v3, &tri_adjacent, &v_opposite);

			if ( adj_exists )
			{
				m_canvas.add(tri_adjacent->to_C2D(), Colour::darkOrange, true);
                m_canvas.add(tri_adjacent->to_C2D(), Colour::black, false);

				new_tris[i]   = Tri (v1, v3, v_opposite);
				*tri_adjacent = Tri (v1, v2, v_opposite);//adjacent points somewhere in m_tris

                tris_adj.push_back(*tri_adjacent); // Keep in order to add to canvas later.
            }
        }
    }

    m_canvas.newFrame(false);
    unsigned n = tris_adj.size();
    for (int i = 0; i < 3; i++) m_canvas.add(new_tris[i].to_C2D(), Colour::green, 1);
    for (int i = 0; i < n; i++) m_canvas.add(tris_adj[i].to_C2D(), Colour::green, 1);
    for (int i = 0; i < 3; i++) m_canvas.add(new_tris[i].to_C2D(), Colour::black, 0);
    for (int i = 0; i < n; i++) m_canvas.add(tris_adj[i].to_C2D(), Colour::black, 0);

    // Add the new triangles.
    m_tris.push_back(new_tris[0]);
    m_tris.push_back(new_tris[1]);
    m_tris.push_back(new_tris[2]);

    vector<unsigned> violations;
    FindViolations(m_tris, m_pts, violations);
    ShowViolations(m_tris, violations, m_canvas, Colour::magenta);

    m_canvas.newFrame(false);

    m_canvas.rew();
    for (int i = 0; i < m_show_step; i++)
        m_canvas.next();
}

void TriangulationScene::draw()
{
    enterPixelMode();

    for (int i = 0; i < m_tris.size(); i++)
        m_tris[i].to_2D(Colour::cyan).draw();           // Draw triangles
    m_canvas.draw();                                    // Draw current step violations
    vvr::draw(m_pts, Colour::red);                      // Draw points
}

//! Task Functions

double sqr(double x){ return x*x*0,999999999 ; }

C2DCircle GetCircumCircle(const C2DTriangle &t)
{
	
	C2DPoint tp1 = t.GetPoint1();
	C2DPoint tp2 = t.GetPoint2();
	C2DPoint tp3 = t.GetPoint3();

	C2DPoint middlePt1 = C2DPoint((tp1.x + tp2.x) / 2, (tp1.y + tp2.y) / 2);

	double a_1 = -((tp2.x - tp1.x)/(tp2.y - tp1.y));
	double b_1 = middlePt1.y - (a_1 * middlePt1.x);
	
	C2DPoint middlePt2 = C2DPoint((tp2.x + tp3.x) / 2, (tp2.y + tp3.y) / 2);

	double a_2 = -((tp3.x - tp2.x)/(tp3.y - tp2.y));
	double b_2 = middlePt2.y - (a_2 * middlePt2.x);

	double cc_x = (b_2 - b_1) / (a_1 - a_2);
	double cc_y = a_1*cc_x + b_1;

	C2DPoint cercumCenter = C2DPoint(cc_x, cc_y);
	double cercumRadius = cercumCenter.Distance(t.GetPoint1());

	return C2DCircle(cercumCenter, cercumRadius);
}

bool IsDelaunay(const C2DTriangle &t, const C2DPointSet &pset)
{
	C2DCircle cc = GetCircumCircle(t);
    cc.SetRadius(cc.GetRadius() * 0.999);

	for (int i = 0; i < pset.size(); i++)
		if (cc.GetCentre().Distance(pset[i]) < cc.GetRadius())  return false;

    return true;
}

bool FindAdjacentTriangle(vector<Tri> &tris, C2DPoint *p1, C2DPoint *p2, Tri **tri_adj, C2DPoint **opp_ver)
{
    for (int i = 0; i < tris.size(); i++)
    {
        C2DPoint  *v1 = tris[i].v1;
        C2DPoint  *v2 = tris[i].v2;
        C2DPoint  *v3 = tris[i].v3;

        if (v1 == p1 && v2 == p2) { *opp_ver = v3; *tri_adj = &tris[i]; return true; }
        if (v1 == p2 && v2 == p1) { *opp_ver = v3; *tri_adj = &tris[i]; return true; }
        if (v2 == p1 && v3 == p2) { *opp_ver = v1; *tri_adj = &tris[i]; return true; }
        if (v2 == p2 && v3 == p1) { *opp_ver = v1; *tri_adj = &tris[i]; return true; }
        if (v3 == p1 && v1 == p2) { *opp_ver = v2; *tri_adj = &tris[i]; return true; }
        if (v3 == p2 && v1 == p1) { *opp_ver = v2; *tri_adj = &tris[i]; return true; }
    }

    return false;
}

bool FindNormalTriangle(vector<Tri> &tris, C2DPoint *p1, C2DPoint *p2, C2DPoint *p3, Tri **tri_norm)
{
    for (int i = 0; i < tris.size(); i++)
    {
        C2DPoint  *v1 = tris[i].v1;
        C2DPoint  *v2 = tris[i].v2;
        C2DPoint  *v3 = tris[i].v3;

        if (v1 == p1 && v2 == p2 && v3 == p3) *tri_norm = &tris[i]; return true;
		if (v1 == p1 && v2 == p3 && v3 == p2) *tri_norm = &tris[i]; return true;
		if (v1 == p2 && v2 == p1 && v3 == p3) *tri_norm = &tris[i]; return true;
		if (v1 == p2 && v2 == p3 && v3 == p3) *tri_norm = &tris[i]; return true;
		if (v1 == p3 && v2 == p1 && v3 == p2) *tri_norm = &tris[i]; return true;
		if (v1 == p3 && v2 == p2 && v3 == p1) *tri_norm = &tris[i]; return true;
    }

    return false;
}

void FindViolations(vector<Tri> &tris, const C2DPointSet &ptset, vector<unsigned> &violations)
{
    violations.clear();
    for (int i = 0; i < tris.size(); i++)
    {
        Tri &tri = tris[i];
        C2DTriangle t(*tri.v1, *tri.v2, *tri.v3);
        if (!IsDelaunay(t, ptset)) {
            violations.push_back(i);
        }
    }
}

void ShowViolations(vector<Tri> &tris, const vector<unsigned> &violations, Canvas2D &canvas, Colour &col)
{
    canvas.newFrame(false);
    for (int i = 0; i < violations.size(); i++) {
        Tri &tri = tris[violations[i]];
        C2DTriangle t(*tri.v1, *tri.v2, *tri.v3);
        canvas.add(GetCircumCircle(t), col, false);
    }
}

//! Main 

int main(int argc, char* argv[])
{
    return vvr::mainLoop(argc, argv, new TriangulationScene);
}
