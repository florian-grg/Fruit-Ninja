#include "projectile.h"
#include <QImage>
#include <QOpenGLTexture>


Projectile::Projectile(float t_i, const QString& texturePath)
{

    this->t_i = t_i;

    this->rayon = 0.3f;
    this->xs = 16.0f;

    float min = 3.55f * M_PI / 4.0f;
    float max = 4.45f * M_PI / 4.0f;

    float angle = min + ((float)rand() / RAND_MAX) * (max - min);

    float s = sin(angle);
    float c = cos(angle);

    this->xt = 30.0 + c*5;
    this->yt = s*5;

    float zmin = 0.5;
    float zmax = 3;

    this->zt = zmin + ((float)rand() / RAND_MAX) * (zmax - zmin);

    this->a = (zt)/(xt*xt-2*xs*xt);
    this->b = (-1)*2*a*xs;

    printf("a = %f, b = %f\n", a, b);

    // quadrique
    GLUquadric* quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricNormals(quadric, GLU_SMOOTH);

    // Déterminer la forme selon la texture
    m_TextureName = texturePath;
    if (texturePath.contains("banane", Qt::CaseInsensitive)) {
        m_FormeType = 1; // Cylindre
    } else if (texturePath.contains("pomme", Qt::CaseInsensitive)) {
        m_FormeType = 2; // Cube
    } else if (texturePath.contains("ananas", Qt::CaseInsensitive)) {
        m_FormeType = 3; // Pyramide
    } else {
        m_FormeType = 0; // Sphère par défaut
    }

    loadTexture(texturePath);


    rotSpeedX = ((float)rand() / RAND_MAX) * 120.0f - 60.0f;
    rotSpeedY = ((float)rand() / RAND_MAX) * 120.0f - 60.0f;
    rotSpeedZ = ((float)rand() / RAND_MAX) * 120.0f - 60.0f;
}

Projectile::~Projectile()
{
    if (texture) {
        delete texture;
    }

}

void Projectile::loadTexture(const QString& texturePath)
{
    QImage img(texturePath);
    if (img.isNull()) {
        return;
    }

    QImage glImg = img.convertToFormat(QImage::Format_RGBA8888).mirrored(false, true);

    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glImg.width(), glImg.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, glImg.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Projectile::BindTexture() const
{
    if (texture) {
        texture->bind();
    }
}

void Projectile::Display(const float timeInDays)
{
    glPushMatrix();

    GLfloat r = 1.0f;
    GLfloat v = 1.0f;
    GLfloat b = 1.0f;
    GLfloat ambient[4] = { r * 0.2f, v * 0.2f, b * 0.2f, 1.0f };
    GLfloat diffuse[4] = { r, v, b, 1.0f };
    GLfloat specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 5.0f);


    float vitesse = 5.0f;

    float tm = (timeInDays-t_i) * vitesse;


    x = tm;
    y = tm * yt / xt ;
    z = 0.0f + this->a * tm * tm + this->b * tm;


    glTranslatef(x, y, z);


    float deltaTime = 1.0f / 60.0f; // 60 FPS
    rotAngleX += rotSpeedX * deltaTime;
    rotAngleY += rotSpeedY * deltaTime;
    rotAngleZ += rotSpeedZ * deltaTime;
    glRotatef(rotAngleX, 1, 0, 0);
    glRotatef(rotAngleY, 0, 1, 0);
    glRotatef(rotAngleZ, 0, 0, 1);

    // texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    GLUquadric* quadric = nullptr;

    switch (m_FormeType) {
    case 0: // Sphère
        quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric, this->rayon, 30, 30);
        gluDeleteQuadric(quadric);
        break;
    case 1: { // Cylindre
        quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        float cylRadius = this->rayon * 0.5;
        float cylHeight = this->rayon * 3;
        gluCylinder(quadric, cylRadius, cylRadius, cylHeight, 30, 1);
        gluDeleteQuadric(quadric);


        quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);

        glPushMatrix();
        glRotatef(180, 1, 0, 0);
        gluDisk(quadric, 0, cylRadius, 30, 1);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, cylHeight);
        gluDisk(quadric, 0, cylRadius, 30, 1);
        glPopMatrix();
        gluDeleteQuadric(quadric);

        break;
    }
    case 2: // Cube
        glBegin(GL_QUADS);

        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, -rayon, -rayon);
        glTexCoord2f(1,0); glVertex3f( rayon, -rayon, -rayon);
        glTexCoord2f(1,1); glVertex3f( rayon,  rayon, -rayon);
        glTexCoord2f(0,1); glVertex3f(-rayon,  rayon, -rayon);

        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, -rayon, rayon);
        glTexCoord2f(1,0); glVertex3f( rayon, -rayon, rayon);
        glTexCoord2f(1,1); glVertex3f( rayon,  rayon, rayon);
        glTexCoord2f(0,1); glVertex3f(-rayon,  rayon, rayon);

        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, -rayon, -rayon);
        glTexCoord2f(1,0); glVertex3f(-rayon,  rayon, -rayon);
        glTexCoord2f(1,1); glVertex3f(-rayon,  rayon,  rayon);
        glTexCoord2f(0,1); glVertex3f(-rayon, -rayon,  rayon);

        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0,0); glVertex3f(rayon, -rayon, -rayon);
        glTexCoord2f(1,0); glVertex3f(rayon,  rayon, -rayon);
        glTexCoord2f(1,1); glVertex3f(rayon,  rayon,  rayon);
        glTexCoord2f(0,1); glVertex3f(rayon, -rayon,  rayon);

        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, -rayon, -rayon);
        glTexCoord2f(1,0); glVertex3f(rayon, -rayon, -rayon);
        glTexCoord2f(1,1); glVertex3f(rayon, -rayon,  rayon);
        glTexCoord2f(0,1); glVertex3f(-rayon, -rayon,  rayon);

        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, rayon, -rayon);
        glTexCoord2f(1,0); glVertex3f(rayon, rayon, -rayon);
        glTexCoord2f(1,1); glVertex3f(rayon, rayon,  rayon);
        glTexCoord2f(0,1); glVertex3f(-rayon, rayon,  rayon);
        glEnd();
        break;
    case 3: // Pyramide
        glBegin(GL_TRIANGLES);

        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, -rayon, 0);
        glTexCoord2f(1,0); glVertex3f( rayon, -rayon, 0);
        glTexCoord2f(1,1); glVertex3f( rayon,  rayon, 0);

        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, -rayon, 0);
        glTexCoord2f(1,1); glVertex3f( rayon,  rayon, 0);
        glTexCoord2f(0,1); glVertex3f(-rayon,  rayon, 0);


        for (int i = 0; i < 4; ++i) {
            float angle1 = (i+0.5) * M_PI_2;
            float angle2 = (i+1.5) * M_PI_2;
            float x1 = rayon * cos(angle1) * sqrt(2), y1 = rayon * sin(angle1) * sqrt(2);
            float x2 = rayon * cos(angle2) * sqrt(2), y2 = rayon * sin(angle2) * sqrt(2);



            float v1x = x1, v1y = y1, v1z = -rayon*2;
            float v2x = x2-x1, v2y = y2-y1, v2z = 0;

            float nx = v1y*v2z - v1z*v2y;
            float ny = v1z*v2x - v1x*v2z;
            float nz = v1x*v2y - v1y*v2x;

            float norm = sqrt(nx*nx + ny*ny + nz*nz);
            nx /= norm; ny /= norm; nz /= norm;

            glNormal3f(nx, ny, nz);
            glTexCoord2f(0.5,1); glVertex3f(0,0,rayon*2);
            glTexCoord2f(0,0); glVertex3f(x1, y1, 0);
            glTexCoord2f(1,0); glVertex3f(x2, y2, 0);
        }
        glEnd();
        break;
    default:
        // Sphere d'erreur
        quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric, this->rayon, 30, 30);
        gluDeleteQuadric(quadric);
        break;
    }

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


bool Projectile::isTooFar(){

    if (this->x > 32){
        return true;
    }
    return false;
}

bool Projectile::isCut(int xc, int yc, int zc){
    if (sqrt(pow((x-xc),2)+pow((y-yc),2)+pow((z-zc),2)) < 1.5 && sqrt(pow((x-30),2)+pow((y-0),2)) < 5){
        return true;
    }
    return false;
}


Residu* Projectile::genererCoupe(float timeInDays, bool miroir){
    return new Residu(
        a, b, t_i, t_i-timeInDays, yt/xt, miroir, m_TextureName,
        rotAngleX, rotAngleY, rotAngleZ,
        rotSpeedX, rotSpeedY, rotSpeedZ
    );
}

