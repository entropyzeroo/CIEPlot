#ifndef CIEWIDGET_H
#define CIEWIDGET_H

#include <QWidget>

class CIEWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CIEWidget(int imSize, int interpNum, float brightness, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent*) override;
    void closeEvent(QCloseEvent*) override;

private:
    QVector<QPointF> m_ciePoints;

    QImage *m_cieImg;
};

#endif // CIEWIDGET_H
