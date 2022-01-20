#include "ciemaker.h"

#include <cmath>

std::vector<CPointF> g_cieCurve{{0.172787,0.004800},{0.170806,0.005472},{0.170085,0.005976},{0.160343,0.014496},{0.146958,0.026643},{0.139149,0.035211},{0.133536,0.042704},
                                 {0.126688,0.053441/*blue*/},{0.115830,0.073601},{0.109616,0.086866},{0.099146,0.112037},{0.091310,0.132737},{0.078130,0.170464},{0.068717,0.200773},
                                 {0.054675,0.254155},{0.040763,0.317049},{0.027497,0.387997/*cyan*/},{0.016270,0.463035},{0.008169,0.538504},{0.004876,0.587196},{0.003983,0.610526},
                                 {0.003859,0.654897},{0.004646,0.675970},{0.007988,0.715407},{0.013870,0.750246},{0.022244,0.779682},{0.027273,0.792153},{0.032820,0.802971},
                                 {0.038851,0.812059},{0.045327,0.819430},{0.052175,0.825200},{0.059323,0.829460},{0.066713,0.832306},{0.074299,0.833833/*green*/},{0.089937,0.833316},
                                 {0.114155,0.826231},{0.138695,0.814796},{0.154714,0.805884},{0.192865,0.781648},{0.229607,0.754347},{0.265760,0.724342},{0.301588,0.692326},
                                 {0.337346,0.658867},{0.373083,0.624470},{0.408717,0.589626},{0.444043,0.554734},{0.478755,0.520222},{0.512467,0.486611/*yellow*/},{0.544767,0.454454},
                                 {0.575132,0.424252},{0.602914,0.396516},{0.627018,0.372510},{0.648215,0.351413},{0.665746,0.334028},{0.680061,0.319765},{0.691487,0.308359},
                                 {0.700589,0.299317},{0.707901,0.292044},{0.714015,0.285945},{0.719017,0.280951},{0.723016,0.276964},{0.734674,0.265326/*red*/}};

std::vector<CPointF>& getCieCurve()
{
    return g_cieCurve;
}

// ***************************** CIE Maker *****************************
CIEMaker::CIEMaker(int interpNum, double brightness):
    m_interpNum(interpNum),
    m_brightness(brightness)
{
    initData();
}

void CIEMaker::initData()
{
    static size_t colorIdx[9]{7, 16, 33, 47, 61, 0, 21, 33, 61};
    int n = m_interpNum;
    float colorVal = m_brightness;
    m_whitePt.setColor({colorVal, colorVal, colorVal});
    rgb_t keyColor[]{
        {0, 0, colorVal},
        {0, colorVal, colorVal},
        {0, colorVal, 0},
        {colorVal, colorVal, 0},
        {colorVal, 0, 0}
    };
    for(size_t i=0; i<5; i++){
        g_cieCurve[colorIdx[i]].setColor(keyColor[i]);
    }
    for(size_t i=0;i<sizeof (colorIdx)/sizeof (colorIdx[0]);i++){
        m_colorPoints[i]=g_cieCurve[colorIdx[i]];
        m_colorPointIdx[i] = colorIdx[i]*n;
    }
    for(int i=0;i<5;i++){
        m_colorLines[i]=CLineF(m_colorPoints[i], m_colorPoints[(i+1)%5]);
    }

    m_whiteLines[0] = CLineF(m_whitePt, m_colorPoints[bottom]);
    m_whiteLines[1] = CLineF(m_whitePt, m_colorPoints[top]);
    m_whiteLines[2] = CLineF(m_whitePt, m_colorPoints[right]);

    m_cieCurvePoints.clear();
    for(size_t i=0;i<g_cieCurve.size()-1;i++){
        CLineF tmpLine(g_cieCurve[i], g_cieCurve[i+1]);
        if(i<colorIdx[blue]) CLineF::addSegment(tmpLine, m_cieCurvePoints, m_colorLines[4], n);
        else if(i<colorIdx[cyan]) CLineF::addSegment(tmpLine, m_cieCurvePoints, m_colorLines[0], n);
        else if(i<colorIdx[green]) CLineF::addSegment(tmpLine, m_cieCurvePoints, m_colorLines[1], n);
        else if(i<colorIdx[yellow]) CLineF::addSegment(tmpLine, m_cieCurvePoints, m_colorLines[2], n);
        else CLineF::addSegment(tmpLine, m_cieCurvePoints, m_colorLines[3], n);
    }
    int bottomPointNum = n*5>20?n*5:20;
    CLineF::addSegment(CLineF(g_cieCurve.back(), g_cieCurve.front()), m_cieCurvePoints, m_colorLines[4], bottomPointNum);
}

uint8_t* CIEMaker::drawCIEDiagram(int picSize)
{
    int width = (picSize+3)>>2<<2;
    int byteCountPerLine = width*4;
    int height = picSize;
    uint8_t *buf = new uint8_t[width*height*4];

    for(int y=0;y<picSize;y++){
        uint8_t *line = buf+y*byteCountPerLine;
        for(int x=0;x<picSize;x++){
            CPointF curP(1.0*x/picSize,1.0-1.0*y/picSize);
            if(isPointInsideBound(curP)){
                CLineF whiteP(m_whitePt, curP);
                CPointF crossPoint;
                areaFlag areaflag = crossArea(curP);
                switch (areaflag) {
                case leftA:
                    crossPoint = getCrossPoint(whiteP, m_colorPointIdx[bottom], m_colorPointIdx[top]);
                    break;
                case rightA:
                    crossPoint = getCrossPoint(whiteP, m_colorPointIdx[top], m_colorPointIdx[right]);
                    break;
                default:
                    crossPoint = getCrossPoint(whiteP, m_colorPointIdx[right], m_cieCurvePoints.size());
                    break;
                }
                CLineF whiteToBoundLine(m_whitePt, crossPoint);
                rgb_t c = whiteToBoundLine.getInterColor(curP);
                line[x*4] = cBound(0, int(c.b*255), 255);
                line[x*4+1] = cBound(0, int(c.g*255), 255);
                line[x*4+2] = cBound(0, int(c.r*255), 255);
                line[x*4+3] = 255;
            }else{
                line[x*4] = 255;
                line[x*4+1] = 255;
                line[x*4+2] = 255;
                line[x*4+3] = 0;
            }
        }
    }
    return buf;
}

CPointF CIEMaker::getCrossPoint(const CLineF &l, int start, int end){
    double minDist = INT_MAX;
    int minIdx = 0;
    for(int i=start;i<end;i++){
        CPointF &p = m_cieCurvePoints[i];
        double tmpDist = std::abs(p.Y()-l.k()*p.X()-l.b());
        if(tmpDist<minDist){
            minDist = tmpDist;
            minIdx = i;
        }
    }

    return m_cieCurvePoints[minIdx];
}

CIEMaker::areaFlag CIEMaker::crossArea(const CPointF &p){
    if     (p.Y() >= m_whiteLines[0].Y(p.X()) && p.Y() <= m_whiteLines[1].Y(p.X()))
        return leftA;
    else if(p.Y() >= m_whiteLines[1].Y(p.X()) && p.Y() >= m_whiteLines[2].Y(p.X()))
        return rightA;
    else return bottomA;
}

bool CIEMaker::isPointInsideBound(const CPointF &p){
    bool rectCondition = p.Y() < m_colorPoints[top].Y() &&
                         p.X() > m_colorPoints[left].X() &&
                         p.X() < m_colorPoints[right].X();

    if(!rectCondition) return rectCondition;

    CLineF bottomLine(m_colorPoints[right], m_colorPoints[bottom]);
    bool isBottom = p.Y() < m_whiteLines[0].Y(p.X()) && p.Y() < m_whiteLines[2].Y(p.X());

    if(isBottom) return rectCondition && p.Y() > bottomLine.k()*p.X()+bottomLine.b();
    else         return rectCondition && _isPointInsideBound(p, m_colorPointIdx[bottom], m_colorPointIdx[right]);
}

bool CIEMaker::_isPointInsideBound(const CPointF &p, int start, int end){
    double minDist1 = INT_MAX;
    double minDist2 = INT_MAX;
    int minIdx1 = 0;
    int minIdx2 = 0;
    double tmpDist = 0;
    for(int i=start;i<end;i++){
        CPointF &q = m_cieCurvePoints[i];
        tmpDist = powf(p.Y()-q.Y(),2)+powf(p.X()-q.X(),2);
        if(tmpDist<minDist1 && q.Y()<p.Y()){
            minDist1 = tmpDist;
            minIdx1 = i;
        }
        if(tmpDist<minDist2 && q.Y()>p.Y()){
            minDist2 = tmpDist;
            minIdx2 = i;
        }
    }

    CLineF distLine(m_cieCurvePoints[minIdx1], m_cieCurvePoints[minIdx2]);
    if(minIdx2<=m_colorPointIdx[left] && minIdx1<=m_colorPointIdx[left])
        return p.Y()>distLine.Y(p.X());
    else
        return p.Y()<distLine.Y(p.X());
}
