#include "ciechart.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

constexpr float axisOffset = 0.1;

CIEChart::CIEChart(QWidget *parent) : QWidget(parent),
    m_showSrgb(true),
    m_showP3(true),
    m_showBT2020(true)
{
    setMinimumSize(500, 500);
    m_ciePic.load(":/cie.png");

    m_mousePos.setParent(this);
    m_mousePos.setGeometry(0, 0, 100, 50);
    m_mousePos.show();

    setMouseTracking(true);
}

void CIEChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);
    p.drawPixmap(m_plotArea, m_ciePic);

    p.setPen(QPen(Qt::black, 2));
    p.drawLine(m_plotArea.bottomLeft(), m_plotArea.topLeft()+QPoint(0, 0.1*m_plotArea.height()));
    p.drawLine(m_plotArea.bottomLeft(), m_plotArea.bottomRight());

    if(!m_points.empty()){
        for(auto &pt:m_points){
//            p.setPen(QPen(Qt::black, 2));
            p.drawEllipse(mapToPosition(pt), 3, 3);
        }
    }

    if(m_showSrgb){
        colorSpace_t sRgb{{0.63, 0.34}, {0.31,0.595}, {0.15,0.06}};
        drawColorSpace(sRgb, Qt::cyan, &p);
        p.drawLine(mapToPosition({0.7, 0.8}), mapToPosition({0.78, 0.8}));
        p.setPen(QPen(Qt::black));
        p.drawText(mapToPosition({0.8, 0.8}), "sRGB");
    }

    if(m_showP3){
        colorSpace_t p3{{0.680, 0.320}, {0.265,0.690}, {0.150,0.060}};
        drawColorSpace(p3, Qt::blue, &p);
        p.drawLine(mapToPosition({0.7, 0.75}), mapToPosition({0.78, 0.75}));
        p.setPen(QPen(Qt::black));
        p.drawText(mapToPosition({0.8, 0.75}), "P3");
    }

    if(m_showBT2020){
        colorSpace_t bt2020{{0.708, 0.292}, {0.170,0.797}, {0.131,0.046}};
        drawColorSpace(bt2020, Qt::red, &p);
        p.drawLine(mapToPosition({0.7, 0.7}), mapToPosition({0.78, 0.7}));
        p.setPen(QPen(Qt::black));
        p.drawText(mapToPosition({0.8, 0.7}), "BT2020");
    }
}

void CIEChart::resizeEvent(QResizeEvent *)
{
    updatePlotArea();
    update();
}

void CIEChart::mouseMoveEvent(QMouseEvent *event)
{
    if(m_plotArea.contains(event->pos())){
        QPointF p = mapToValue(event->pos());
        m_mousePos.setText(QString("x:%1\ny:%2").arg(QString::number(p.x(), 'f', 3)).arg(QString::number(p.y(), 'f', 3)));
    }
}

void CIEChart::updatePlotArea()
{
    if(width()>height()){
        m_offset.setWidth((width()-height())/2);
        m_offset.setHeight(0);
    }else{
        m_offset.setWidth(0);
        m_offset.setHeight((height()-width())/2);
    }
    int chartSize = qMin(height(), width());
    m_plotArea.setRect(axisOffset*chartSize+m_offset.width(), m_offset.height(),
                       (1-axisOffset)*chartSize, (1-axisOffset)*chartSize);
}

QPointF CIEChart::mapToPosition(const QPointF &p)
{
    return QPointF(m_plotArea.x()+m_plotArea.width()*p.x(),
                  m_offset.height()+m_plotArea.height()*(1-p.y()));
}

QPointF CIEChart::mapToValue(const QPoint &p)
{
    return QPointF(1.0f*(p.x()-m_plotArea.x())/m_plotArea.width(),
                   1.0f*(m_plotArea.y()+m_plotArea.height()-p.y())/m_plotArea.height());
}

void CIEChart::addPoint(const QPointF &p)
{
    m_points.append(p);
    update();
}

void CIEChart::drawColorSpace(const colorSpace_t &cs, const QColor &c, QPainter *p)
{
    p->setPen(QPen(c, 2));
    p->drawLine(mapToPosition(cs.red), mapToPosition(cs.green));
    p->drawLine(mapToPosition(cs.green), mapToPosition(cs.blue));
    p->drawLine(mapToPosition(cs.blue), mapToPosition(cs.red));
}
