#ifndef CIECHART_H
#define CIECHART_H

#include <QWidget>
#include <QLabel>
class CIEChart : public QWidget
{
    Q_OBJECT
public:
    typedef struct{
        QPointF red;
        QPointF green;
        QPointF blue;
    }colorSpace_t;
public:
    explicit CIEChart(QWidget *parent = nullptr);

    void addPoint(const QPointF &p);

private:
    void updatePlotArea();
    QPointF mapToPosition(const QPointF &p);
    QPointF mapToValue(const QPoint &p);
    void drawColorSpace(const colorSpace_t &cs, const QColor &c, QPainter *p);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QRect m_plotArea;
    QSize m_offset;
    QLabel m_mousePos;

    QList<QPointF> m_points;
    QPixmap m_ciePic;

    bool m_showSrgb;
    bool m_showP3;
    bool m_showBT2020;
};

#endif // CIECHART_H
