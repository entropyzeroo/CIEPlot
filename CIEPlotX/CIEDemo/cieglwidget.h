#ifndef CIEGLWIDGET_H
#define CIEGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <QPushButton>
#include <QTimer>

#include "ciemaker.h"

class QOpenGLShaderProgram;
class CIEGlWidget : public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT

public:
    CIEGlWidget(int imSize, int interpNum, float brightness, QWidget *parent = nullptr);
    ~CIEGlWidget();

protected:
    void paintGL() override;
    void initializeGL() override;
    void closeEvent(QCloseEvent*) override;

private:
    QOpenGLShaderProgram *m_shader;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;

    QVector<CPointF> m_cieCurvePoints;
    std::vector<CPointF>& g_cieCurve;

    QPushButton *m_btn;
    QTimer m_timer;
    int m_dynDrawVertexNum = 0;
};
#endif // CIEGLWIDGET_H
