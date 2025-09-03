#define _USE_MATH_DEFINES
#include <GL/glu.h>
#include "myglwidget.h"
#include "projectile.h"
#include <QApplication>
#include <QKeyEvent>
#include <QVector>
#include <QImage>
#include <QPainter>

const unsigned int WIN_WIDTH_HEIGHT = 800;
const float MAX_DIMENSION = 50.0f;
const int NUM_STARS = 500;
const float D_JOUEUR = 30.0f;
const float H_JOUEUR = 2.0f;
const float PORTEE = 5.0f;

GLuint texFond = 0;
GLuint texSol = 0;
GLuint texSpawner = 0;
GLuint texSword = 0;

MyGLWidget::MyGLWidget(QWidget* parent) : QOpenGLWidget(parent)
{
    srand(time(NULL));
    setFixedSize(WIN_WIDTH_HEIGHT, WIN_WIDTH_HEIGHT);


    connect(&m_AnimationTimer, &QTimer::timeout, [&] {
        m_TimeElapsed += 1.0f / 60.0f;


        const QString str1 = ":texture-agrumes.jpg";
        const QString str2 = ":texture-ananas.jpg";
        const QString str3 = ":texture-banane.jpg";
        const QString str4 = ":texture-orange.jpg";
        const QString str5 = ":texture-pomme.jpg";


        QVector<const QString*> chaines = { &str1, &str2, &str3, &str4, &str5 };

        std::srand(std::time(nullptr));

        this->updateJeu(chaines);

        update();
    });

    m_AnimationTimer.setInterval(16);
}

void MyGLWidget::initializeGL()
{
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);

    GLfloat lightPosition[4] = {0.0f, 2*H_JOUEUR, -10.0f, 1.0f};
    GLfloat lightAmbient[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);


    // Chargement des texture
    QImage imgFond(":panorama.jpg");
    QImage imgSol(":texture-sol.jpg");
    QImage imgSpawner(":spawner.jpg");
    QImage imgSword(":main.png");

    if (!imgFond.isNull()) imgFond = imgFond.mirrored(false, true).convertToFormat(QImage::Format_RGBA8888);
    if (!imgSol.isNull()) imgSol = imgSol.transformed(QTransform().rotate(90)).convertToFormat(QImage::Format_RGBA8888);

    if (!imgSword.isNull()) imgSword = imgSword.mirrored(false, true).convertToFormat(QImage::Format_RGBA8888);


    glGenTextures(1, &texFond);
    glBindTexture(GL_TEXTURE_2D, texFond);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgFond.width(), imgFond.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgFond.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &texSol);
    glBindTexture(GL_TEXTURE_2D, texSol);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgSol.width(), imgSol.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgSol.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &texSpawner);
    glBindTexture(GL_TEXTURE_2D, texSpawner);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgSpawner.width(), imgSpawner.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgSpawner.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &texSword);
    glBindTexture(GL_TEXTURE_2D, texSword);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgSword.width(), imgSword.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgSword.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBindTexture(GL_TEXTURE_2D, 0);
}

void MyGLWidget::resizeGL(int width, int height)
{
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void MyGLWidget::updateJeu(QVector<const QString*> chaines)
{
    if (!isRunning) return;
    if (m_TimeElapsed > timeLastThrow + delay || timeLastThrow == 0) {
        timeLastThrow = m_TimeElapsed;

        int index = std::rand() % chaines.size();
        const QString& choisie = *chaines[index];

        m_Projectiles.push_back(new Projectile(m_TimeElapsed, choisie));
    }

    //Suppression des éléments trop loin
    for (auto it = m_Projectiles.begin(); it != m_Projectiles.end(); ) {
        if ((*it)->isTooFar()) {
            it = m_Projectiles.erase(it);
            finJeu();
            return;
        }
        else if ((*it)->isCut(xd,yd,zd)){
            score++;
            m_Residu.push_back((*it)->genererCoupe(m_TimeElapsed, true));
            m_Residu.push_back((*it)->genererCoupe(m_TimeElapsed, false));
            it = m_Projectiles.erase(it);
        }
        else {
            ++it;
        }
    }

    for (auto it = m_Residu.begin(); it != m_Residu.end(); ){
        if ((*it)->isTooFar()) {
            it = m_Residu.erase(it);
        }
        else {
            ++it;
        }
    }

}

void MyGLWidget::paintGL()
{
    printf("paintGL est appelé !\n");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    gluLookAt(
        D_JOUEUR, 0.0f, H_JOUEUR,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f
        );

    glPushAttrib(GL_LIGHTING_BIT | GL_POLYGON_BIT | GL_TEXTURE_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);



    // Decoupeur

    GLUquadric* quadric = nullptr;

    gluDeleteQuadric(quadric);

    glPushMatrix();
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, texSword);
    glTranslatef(xd,yd,zd);

    const float lCut = 0.1;

    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(lCut , lCut * -1,  lCut * -1);
    glTexCoord2f(1,0); glVertex3f(lCut , lCut,  lCut * -1);
    glTexCoord2f(1,1); glVertex3f(lCut , lCut,  lCut);
    glTexCoord2f(0,1); glVertex3f(lCut , lCut * -1,+ lCut);

    glEnd();
    glPopMatrix();


    const float WALL_X_MIN = 0.0f;
    const float WALL_X_MAX = 32.0f;
    const float WALL_Y_MIN = -4.0f;
    const float WALL_Y_MAX = 4.0f;
    const float WALL_Z_MIN = 0.0f;

    glBindTexture(GL_TEXTURE_2D, texFond);



    quadric = gluNewQuadric();
    glPushMatrix();

    gluQuadricTexture(quadric, GL_TRUE);
    glRotatef(310,0,0,1);
    gluSphere(quadric, 50, 30, 30);
    gluDeleteQuadric(quadric);

    glPopMatrix();




    float l = 1;
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texSpawner);
    glTranslatef(l,0,l);

    glBegin(GL_QUADS);
    // Face avant (z = -l)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0,0); glVertex3f(-l, -l, -l);
    glTexCoord2f(1,0); glVertex3f( l, -l, -l);
    glTexCoord2f(1,1); glVertex3f( l,  l, -l);
    glTexCoord2f(0,1); glVertex3f(-l,  l, -l);

    // Face arrière (z = l)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0,0); glVertex3f(-l, -l, l);
    glTexCoord2f(1,0); glVertex3f( l, -l, l);
    glTexCoord2f(1,1); glVertex3f( l,  l, l);
    glTexCoord2f(0,1); glVertex3f(-l,  l, l);

    // Face gauche (x = -l)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0,0); glVertex3f(-l, -l, -l);
    glTexCoord2f(1,0); glVertex3f(-l,  l, -l);
    glTexCoord2f(1,1); glVertex3f(-l,  l,  l);
    glTexCoord2f(0,1); glVertex3f(-l, -l,  l);

    // Face droite (x = l)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0,0); glVertex3f(l, -l, -l);
    glTexCoord2f(1,0); glVertex3f(l,  l, -l);
    glTexCoord2f(1,1); glVertex3f(l,  l,  l);
    glTexCoord2f(0,1); glVertex3f(l, -l,  l);

    // Face bas (y = -l)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0,0); glVertex3f(-l, -l, -l);
    glTexCoord2f(1,0); glVertex3f(l, -l, -l);
    glTexCoord2f(1,1); glVertex3f(l, -l,  l);
    glTexCoord2f(0,1); glVertex3f(-l, -l,  l);

    // Face haut (y = l)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0,0); glVertex3f(-l, l, -l);
    glTexCoord2f(1,0); glVertex3f(l, l, -l);
    glTexCoord2f(1,1); glVertex3f(l, l,  l);
    glTexCoord2f(0,1); glVertex3f(-l, l,  l);
    glEnd();


    glPopMatrix();



    // sol
    glBindTexture(GL_TEXTURE_2D, texSol);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(WALL_X_MIN, WALL_Y_MIN, WALL_Z_MIN);
    glTexCoord2f(1,0); glVertex3f(WALL_X_MAX, WALL_Y_MIN, WALL_Z_MIN);
    glTexCoord2f(1,1); glVertex3f(WALL_X_MAX, WALL_Y_MAX, WALL_Z_MIN);
    glTexCoord2f(0,1); glVertex3f(WALL_X_MIN, WALL_Y_MAX, WALL_Z_MIN);
    glEnd();


    glBindTexture(GL_TEXTURE_2D, 0);
    glPopAttrib();

    //Cylindre
    GLUquadric* quad = gluNewQuadric();
    gluQuadricDrawStyle(quad, GLU_LINE);

    glPushMatrix();
    glTranslatef(30.0f, 0.0f, 0.0f);
    gluCylinder(quad, 5.0f, 5.0f, 4.0f, 120, 15);
    glPopMatrix();

    gluDeleteQuadric(quad);



    for (Projectile* proj : m_Projectiles) {
        proj->Display(m_TimeElapsed);
    }
    for (Residu* res : m_Residu) {
        res->Display(m_TimeElapsed);
    }

    glFlush();



    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width(), height(), 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);




    QPainter painter(this);

    QString var1 = "Temps : " + QString::number(m_TimeElapsed - timeStart, 'f', 1);
    QString var2 = "Score : " + QString::number(score);

    if (m_TimeElapsed - timeStart <= 0) {
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 40));
        painter.drawText(150,150,"MINECRAFT NINJA");
    }else if(!isRunning){
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 40));
        painter.drawText(250,150,"GAME OVER");
        painter.drawText(260, 230, var1);
        painter.drawText(300, 310, var2);
    }else{
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 14));



        painter.drawText(10, 30, var1);
        painter.drawText(10, 60, var2);
    }





    painter.end();


    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}


void MyGLWidget::startGame(){
    m_AnimationTimer.start();
    timeStart = m_TimeElapsed;
    score = 0;
    isRunning = true;
}

void MyGLWidget::updateCoordinates(float x, float y){

    this->xd = 25.0;
    this->yd = x*5-2.5;
    this->zd = y*2.5+0.5;
}

void MyGLWidget::finJeu(){

    isRunning = false;
    m_AnimationTimer.stop();

    for (auto it = m_Projectiles.begin(); it != m_Projectiles.end(); ){
        it = m_Projectiles.erase(it);
    }
    for (auto it = m_Residu.begin(); it != m_Residu.end(); ){
        it = m_Residu.erase(it);
    }


    emit gameOver();
}
