#include "ciewidget.h"
#include "ciemaker.h"

#include <QPainter>
#include <QPushButton>
#include <QFileDialog>

CIEWidget::CIEWidget(int imSize, int interpNum, float brightness, QWidget *parent)
    : QWidget(parent)
{
    CIEMaker cie(interpNum, brightness);
    auto curve = cie.getCieCurvePoints();
    for(auto &c:curve){
        m_ciePoints.push_back({c.X(), 1-c.Y()});
    }

    m_cieImg = new QImage(cie.drawCIEDiagram(imSize), imSize, imSize, QImage::Format_ARGB32);

    setWindowTitle("cie 1931 xy Chromaticity");
    setFixedSize(imSize, imSize);

    QPushButton *saveBtn = new QPushButton("save", this);
    saveBtn->setGeometry(450, 0, 50, 30);
    connect(saveBtn, &QPushButton::clicked, this, [&]{
        QString savePath = QFileDialog::getSaveFileName();
        if(!savePath.isNull()){
            m_cieImg->save(savePath);
        }
    });
}

void CIEWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawImage(QRectF(0, 0, width(), height()), *m_cieImg);
}

void CIEWidget::closeEvent(QCloseEvent *)
{
    hide();
}
