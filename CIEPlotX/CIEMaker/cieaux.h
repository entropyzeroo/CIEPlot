#ifndef CIEAUX_H
#define CIEAUX_H

#include "CIEMaker_global.h"
#include <vector>

template <typename T>
constexpr inline const T &cMin(const T &a, const T &b) { return (a < b) ? a : b; }
template <typename T>
constexpr inline const T &cMax(const T &a, const T &b) { return (a < b) ? b : a; }
template <typename T>
constexpr inline const T &cBound(const T &min, const T &val, const T &max)
{ return cMax(min, cMin(max, val)); }

// color
typedef struct{
    float r;
    float g;
    float b;
}rgb_t;

// point
class CIEMAKER_EXPORT CPointF
{
    friend class CLineF;
public:
    CPointF() : CPointF(0, 0, 0, 0, 0){};
    CPointF(float x, float y) : CPointF(x, y, 0, 0, 0){};
    CPointF(float x, float y, float r, float g, float b) : CPointF(x, y, {r, g, b}){};
    CPointF(float x, float y, rgb_t c) : _x(x), _y(y), _c(c){};

    float X() const{return _x;};
    float Y() const{return _y;};
    rgb_t C() const{return _c;};

    void setColor(rgb_t c){_c=c;};
    void setX(float x){_x=x;};
    void setY(float y){_y=y;};

    static CPointF getInterpPoint(const CPointF &p1, const CPointF &p2, double pos);
    void setInterpColor(const CPointF &p1, const CPointF &p2);
    double getPointDist(const CPointF &) const;

private:
    float _x, _y;
    rgb_t _c;
};

// line
class CIEMAKER_EXPORT CLineF
{
public:
    CLineF():_k(0.0), _b(0.0){};
    CLineF(const CPointF &p1, const CPointF &p2);

    double k() const    {return _k;};
    double b() const    {return _b;};
    CPointF SP() const  {return startPoint;};
    CPointF EP() const  {return endPoint;};
    double Y(double x) const {return _k*x+_b;};
    CPointF P(double pos) const;

    rgb_t  getInterColor(const CPointF &p) const;
    rgb_t  getInterColor(double p) const;
    CPointF getCrossPoint(const CLineF &l);
    static void addSegment(const CLineF &l, std::vector<CPointF> &points, const CLineF &colorLine, int n);

private:
    CPointF startPoint;
    CPointF endPoint;
    float _k, _b;
};



#endif // CIEAUX_H
