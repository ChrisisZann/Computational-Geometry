#ifndef SCENE_INTERSECTIONS_H
#define SCENE_INTERSECTIONS_H

#include <scene.h>
#include <canvas.h>
#include <GeoLib.h>

#define APP_TITLE "VVR GeoLab - Intersections"

using namespace std;
using namespace vvr;

class IntersectionsScene : public vvr::Scene
{
public:
    IntersectionsScene();
    const char* getName() const override { return APP_TITLE; }

protected: // Overriden methods
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;

private: // Methods
    void Task1(const C2DPoint &p);
    void Task2(const C2DPoint &p);
    void Task3(const C2DPoint &p);
	void line_intersection(C2DLine l1, C2DLine l2, C2DPoint &p_i);

private: // Data
    C2DLine     m_bound_horizontal;
    C2DLine     m_bound_vertical;

    Canvas2D    m_canvas_0;
    Canvas2D    m_canvas_1;
    Canvas2D    m_canvas_2;
    Canvas2D    m_canvas_3;
    Canvas2D    m_canvas_4;
    C2DTriangle m_triangle_1;
    C2DTriangle m_triangle_2;
    C2DCircle   m_circle_1;
    C2DCircle   m_circle_2;
    C2DLine     m_line_1;
    C2DLine     m_line_2;
};

#endif
