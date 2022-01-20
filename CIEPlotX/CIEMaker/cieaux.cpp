#include "cieaux.h"

#include <cmath>
// ***************************** CPointF *****************************

CPointF CPointF::getInterpPoint(const CPointF &p1, const CPointF &p2, double pos)
{
    float x, y, r, g, b;
    x = p1._x*(1-pos)+p2._x*pos;
    y = p1._y*(1-pos)+p2._y*pos;
    r = p1._c.r*(1-pos)+p2._c.r*pos;
    g = p1._c.g*(1-pos)+p2._c.g*pos;
    b = p1._c.b*(1-pos)+p2._c.b*pos;
    return CPointF{x, y, {r, g, b}};
}

void CPointF::setInterpColor(const CPointF &p1, const CPointF &p2)
{
    double dist1 = p1.getPointDist(p2);
    double dist2 = getPointDist(p1);
    double pos = dist2/dist1;
    float r, g, b;
    r = p1._c.r*(1-pos)+p2._c.r*pos;
    g = p1._c.g*(1-pos)+p2._c.g*pos;
    b = p1._c.b*(1-pos)+p2._c.b*pos;
    setColor({r, g, b});
}

double CPointF::getPointDist(const CPointF &p) const
{
    return pow((p._x-_x)*(p._x-_x)+(p._y-_y)*(p._y-_y), 0.5);
}

// ***************************** CLineF *****************************
CLineF::CLineF(const CPointF &p1, const CPointF &p2)
{
    startPoint = p1;
    endPoint = p2;
    _k = (p1._y-p2._y)/(p1._x-p2._x+0.000001);
    _b = p1._y-_k*p1._x;
}

CPointF CLineF::P(double pos) const {
    pos = cBound(-1.0, pos, 1.0);
    float x = startPoint.X()+pos*std::abs(endPoint.X()-startPoint.X());
    return {x, _k*x+_b};
}

rgb_t CLineF::getInterColor(const CPointF &p) const{
    CPointF tmp = p;
    tmp.setInterpColor(startPoint, endPoint);
    return tmp.C();
}

rgb_t CLineF::getInterColor(double pos) const{
    pos = cBound(0.0, pos, 1.0);

    return CPointF::getInterpPoint(startPoint, endPoint, pos).C();
}

CPointF CLineF::getCrossPoint(const CLineF &l){
    if(l.k() == _k) return CPointF();

    double x = (l.b() - _b)/(_k - l.k());
    double y = _k*x+_b;
    CPointF p(x, y);

    return p;
}

void CLineF::addSegment(const CLineF &l, std::vector<CPointF> &points, const CLineF &colorLine, int n){
    if(n<=0) n=1;

    double direction = 1.0;
    if(l.startPoint.X()>l.endPoint.X()) direction = -1.0;
    double stepX = direction / n;
    for(int i=0;i<n;i++){
        CPointF p = l.P(stepX*i);
        p.setColor(colorLine.getInterColor(p));
        points.push_back(p);
    }
}



