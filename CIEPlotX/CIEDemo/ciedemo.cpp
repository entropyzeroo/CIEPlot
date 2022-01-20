#include "ciedemo.h"
#include "ciechart.h"
#include "ciewidget.h"
#include "cieglwidget.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

CIEDemo::CIEDemo(QWidget *parent)
    : QWidget(parent),
      m_cieWidget(nullptr),
      m_cieGlWidget(nullptr)
{
    initWidget();
    initLayout();
}

CIEDemo::~CIEDemo()
{
}

void CIEDemo::initWidget()
{
    m_cieChart = new CIEChart;

    m_openCieWidgetBtn = new QPushButton("widget demo");
    connect(m_openCieWidgetBtn, &QPushButton::clicked, this, [&]{
        if(m_cieWidget){
            m_cieWidget->show();
        }else{
            m_cieWidget = new CIEWidget(1000, 20, 0.7);
            m_cieWidget->show();
        }
    });
    m_openGlWidgetBtn = new QPushButton("opengl demo");
    connect(m_openGlWidgetBtn, &QPushButton::clicked, this, [&]{
        if(m_cieGlWidget){
            m_cieGlWidget->show();
        }else{
            m_cieGlWidget = new CIEGlWidget(1000, 20, 0.7);
            m_cieGlWidget->show();
        }
    });

}

void CIEDemo::initLayout()
{
    QVBoxLayout *demoBtnLayout = new QVBoxLayout;
    demoBtnLayout->addWidget(m_openCieWidgetBtn);
    demoBtnLayout->addWidget(m_openGlWidgetBtn);
    demoBtnLayout->addStretch(1);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_cieChart, 1);
    mainLayout->addLayout(demoBtnLayout);
    setLayout(mainLayout);
}

