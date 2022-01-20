#include "cieglwidget.h"
#include <QOpenGLShaderProgram>

#define GL_VERSION_STR "#version 330 core\n"

#define GET_GLSTR(x) GL_VERSION_STR#x

const char *vsrc = GET_GLSTR(

 layout (location = 0) in vec3 aPos;
 layout (location = 1) in vec3 aColor;
 out vec3 ourColor;
 void main()
 {
  gl_Position = vec4(aPos, 1.0);
  ourColor = aColor;
 }
);

const char *fsrc =GET_GLSTR(
 out vec4 FragColor;
 in vec3 ourColor;
 void main()
 {
  FragColor = vec4(ourColor, 1.0);
 }
);

 enum{
     Color_Violet = 0,
     Color_Blue = 7,
     Color_Cyan = 16,
     Color_Green = 33,
     Color_Yellow = 47,
     Color_Red = 61
 };


 static void addInterpPoint(QVector<CPointF> &curve, const CPointF &start, const CPointF &end, int num)
 {
     double step = 1.0/num;
     for(int i=0; i<num; i++){
         curve.push_back(CPointF::getInterpPoint(start, end, step));
     }
 }

static void initPointColor(float brightness)
{
    auto &g_cieCurve = getCieCurve();
    g_cieCurve[Color_Violet].setColor({brightness/2, 0.0, brightness});
    g_cieCurve[Color_Blue].setColor({0.0, 0.0, brightness});
    g_cieCurve[Color_Cyan].setColor({0.0, brightness, brightness});
    g_cieCurve[Color_Green].setColor({0.0, brightness, 0.0});
    g_cieCurve[Color_Yellow].setColor({brightness, brightness, 0.0});
    g_cieCurve[Color_Red].setColor({brightness, 0.0, 0.0});

    auto colorInit = [&](int start, int end){
     for(int i=start+1; i<end; i++){
         g_cieCurve[i].setInterpColor(g_cieCurve[start], g_cieCurve[end]);
     }
    };

    colorInit(Color_Violet, Color_Blue);
    colorInit(Color_Blue, Color_Cyan);
    colorInit(Color_Cyan, Color_Green);
    colorInit(Color_Green, Color_Yellow);
    colorInit(Color_Yellow, Color_Red);
}

CIEGlWidget::CIEGlWidget(int imSize, int interpNum, float brightness, QWidget *parent)
    : QOpenGLWidget(parent),
      QOpenGLFunctions(),
      m_shader(nullptr),
      g_cieCurve(getCieCurve())
{
    setWindowTitle("1931 cie xy opengl");
    setFixedSize(imSize, imSize);
    // 初始化边界点颜色
    initPointColor(brightness);
    // opengl渲染起始点为中心白点
    m_cieCurvePoints.push_back({0.3333, 0.3333, brightness, brightness, brightness});
    // 对光谱边界插值
    for(size_t i=0; i<g_cieCurve.size()-1;i++){
        addInterpPoint(m_cieCurvePoints, g_cieCurve[i], g_cieCurve[i+1], interpNum);
    }
    // 闭合曲线
    addInterpPoint(m_cieCurvePoints, g_cieCurve[0], g_cieCurve[1], interpNum);

    m_btn = new QPushButton("停止", this);
    m_btn->setGeometry(450, 0, 50, 30);
    connect(m_btn, &QPushButton::clicked, this, [&]{
       if(m_timer.isActive()){
           m_timer.stop();
           m_dynDrawVertexNum = m_cieCurvePoints.size();
           m_btn->setText("启动");
       } else{
           m_timer.start(5);
           m_btn->setText("停止");
       }
    });
    // 定时器控制动态绘制色度图
    connect(&m_timer, &QTimer::timeout, this, [&]{m_dynDrawVertexNum = (m_dynDrawVertexNum+1)%m_cieCurvePoints.size();update();});
    m_timer.start(5);
}

CIEGlWidget::~CIEGlWidget()
{
}

void CIEGlWidget::closeEvent(QCloseEvent *)
{
    hide();
}

void CIEGlWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //设置背景色为白色
    //1.创建着色器程序
    m_shader = new QOpenGLShaderProgram;
    m_shader->addShaderFromSourceCode(QOpenGLShader::Vertex,vsrc);
    m_shader->addShaderFromSourceCode(QOpenGLShader::Fragment,fsrc);
    m_shader->link();
    m_shader->bind();//激活Program对象

    // 将CIE光谱轨迹点转化为opengl顶点
    int verticesNum = m_cieCurvePoints.size()*5;
    float *vertices = new float[verticesNum];
    int idx = 0;
    for(int i=0; i<m_cieCurvePoints.size(); i++){
        vertices[idx]=m_cieCurvePoints[i].X()*2-1;
        vertices[idx+1]=m_cieCurvePoints[i].Y()*2-1;
        vertices[idx+2]=m_cieCurvePoints[i].C().r;
        vertices[idx+3]=m_cieCurvePoints[i].C().g;
        vertices[idx+4]=m_cieCurvePoints[i].C().b;
        idx+=5;
    }
    //2.初始化vbo
    vbo.create();
    vbo.bind(); //绑定到当前的OpenGL上下文,
    vbo.allocate(vertices, verticesNum*sizeof(GLfloat));
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw); //设置为一次修改，多次使用


    //3.初始化VAO,设置顶点数据状态(顶点，法线，纹理坐标等)
    vao.create();
    vao.bind();

    // void setAttributeBuffer(int location, GLenum type, int offset, int tupleSize, int stride = 0);
    m_shader->setAttributeBuffer(0, GL_FLOAT, 0, 2, 5 * sizeof(float)); //设置aPos顶点属性
    m_shader->setAttributeBuffer(1, GL_FLOAT, 2 * sizeof(float), 3, 5 * sizeof(float)); //设置aColor顶点颜色
    //offset:第一个数据的偏移量
    //tupleSize:一个数据有多少个元素,比如位置为xyz,颜色为rgb,所以是3
    //stride:步长,下个数据距离当前数据的之间距离,比如右下位置和左下位置之间间隔了:3个xyz值+3个rgb值,所以填入 6 * sizeof(float)


    m_shader->enableAttributeArray(0); //使能aPos顶点属性
    m_shader->enableAttributeArray(1); //使能aColor顶点颜色


    //4.解绑所有对象
    vao.release();
    vbo.release();
}

void CIEGlWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // 渲染Shader
    vao.bind(); //绑定激活vao
    glDrawArrays(GL_TRIANGLE_FAN, 0, m_dynDrawVertexNum);
    vao.release(); //解绑
}


