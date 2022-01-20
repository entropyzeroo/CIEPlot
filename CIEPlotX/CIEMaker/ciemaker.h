#ifndef CIEMAKER_H
#define CIEMAKER_H

#include "CIEMaker_global.h"
#include "cieaux.h"

// cie painter
using uint8_t = unsigned char;

std::vector<CPointF>& CIEMAKER_EXPORT getCieCurve();

class CIEMAKER_EXPORT CIEMaker
{
private:
    enum pointFlag{blue, cyan, green, yellow, red, bottom, left, top, right};
    enum areaFlag{leftA, rightA, bottomA};
public:
    CIEMaker(int interpNum=20, double brightness=1.0);

    uint8_t* drawCIEDiagram(int picSize = 500);
    std::vector<CPointF> getCieCurvePoints() const{return m_cieCurvePoints;};

private:
    void initData();

    areaFlag crossArea(const CPointF &p);
    CPointF getCrossPoint(const CLineF &l, int start, int end);

    bool _isPointInsideBound(const CPointF &p, int start, int end);
    bool isPointInsideBound(const CPointF &p);
private:
    int m_interpNum;
    double m_brightness;

    std::vector<CPointF> m_cieCurvePoints;
    CPointF m_whitePt{0.3333,0.3333};

    int m_colorPointIdx[9]{0};
    CPointF m_colorPoints[9]{};
    CLineF m_colorLines[5]{};

    CLineF m_whiteLines[3]{};
};

#endif // CIEMAKER_H
