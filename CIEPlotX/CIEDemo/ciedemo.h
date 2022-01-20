#ifndef CIEDEMO_H
#define CIEDEMO_H

#include <QWidget>

class CIEChart;
class CIEWidget;
class CIEGlWidget;
class QPushButton;
class CIEDemo : public QWidget
{
    Q_OBJECT

public:
    CIEDemo(QWidget *parent = nullptr);
    ~CIEDemo();

private:
    void initWidget();
    void initLayout();

private:
    CIEChart *m_cieChart;
    CIEWidget *m_cieWidget;
    CIEGlWidget *m_cieGlWidget;

    QPushButton *m_openCieWidgetBtn;
    QPushButton *m_openGlWidgetBtn;
};
#endif // CIEDEMO_H
