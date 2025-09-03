#include "projectile.h"
#include <QImage>
#include <QOpenGLTexture>


Residu::Residu(float a, float b, float ti, float te, float dy, bool miroir, const QString& texturePath,
               float rotAngleX, float rotAngleY, float rotAngleZ,
               float rotSpeedX, float rotSpeedY, float rotSpeedZ)
{

    this->t_i = ti;

    this->rayon = 0.3f;

    this->a = a;
    this->b = b;
    this->dy = dy;

    if(miroir){
        this->miroir = 1.0;
    }
    else{
        this->miroir = -1.0;
    }

    this->t_e = te;

    printf("a = %f, b = %f\n", a, b);


    GLUquadric* quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricNormals(quadric, GLU_SMOOTH);


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

    this->rotAngleX = rotAngleX;
    this->rotAngleY = rotAngleY;
    this->rotAngleZ = rotAngleZ;
    this->rotSpeedX = rotSpeedX;
    this->rotSpeedY = rotSpeedY;
    this->rotSpeedZ = rotSpeedZ;
}

Residu::~Residu()
{
    if (texture) {
        delete texture;
    }

}

void Residu::loadTexture(const QString& texturePath)
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

void Residu::BindTexture() const
{
    if (texture) {
        texture->bind();
    }
}

void Residu::Display(const float timeInDays)
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
    float directionX = 30.0f;
    float directionZ = 2.0f;
    float norm = sqrt(directionX * directionX + directionZ * directionZ);

    float tm = (timeInDays-t_i) * vitesse;
    float te = (timeInDays-t_i+t_e) * vitesse / 5 * miroir;


    x = tm;
    y = tm * dy +  te;
    z = 0.0f + this->a * tm * tm + this->b * tm;

    glTranslatef(x, y, z);


    float deltaTime = 1.0f / 60.0f;
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



        glPushMatrix();

        glRotatef(90 * miroir, 1.0, 0.0, 0.0);

        for (int i = 0; i <= 15; ++i) {
            float lat0 = M_PI * (-0.5 + (float)(i) / 30);
            float lat1 = M_PI * (-0.5 + (float)(i + 1) / 30);

            float z0 = sin(lat0), zr0 = cos(lat0);
            float z1 = sin(lat1), zr1 = cos(lat1);

            glBegin(GL_QUAD_STRIP);
            for (int j = 0; j <= 30; ++j) {
                float lng = 2 * M_PI * (float)(j - 1) / 30;
                float x = cos(lng), y = sin(lng);

                glNormal3f(x * zr0, y * zr0, z0);
                glVertex3f(x * zr0 * rayon, y * zr0 * rayon, z0 * rayon);

                glNormal3f(x * zr1, y * zr1, z1);
                glVertex3f(x * zr1 * rayon, y * zr1 * rayon, z1 * rayon);
            }
            glEnd();
        }
        glPopMatrix();


        glPushMatrix();
        glNormal3f(0, miroir, 0);
        glRotatef(90, 1.0, 0.0, 0.0);
        gluDisk(quadric, 0.0, rayon, 30, 1);
        glPopMatrix();


        gluDeleteQuadric(quadric);
        break;
    case 1: // Cylindre
    {
        float height = this->rayon * 3.0f;
        int slices = 30;
        float angleStep = M_PI / (float)slices;


        glEnable(GL_NORMALIZE);

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= slices; ++i) {
            float angle = i * angleStep;
            float x = cos(angle) * rayon * 0.5;
            float y = sin(angle) * rayon * miroir * 0.5;

            glNormal3f(cos(angle), sin(angle) * miroir, 0.0f);


            glTexCoord2f((float)i / slices, 0.0f);
            glVertex3f(x, y, 0.0f);

            glTexCoord2f((float)i / slices, 1.0f);
            glVertex3f(x, y, height);
        }
        glEnd();


        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.5f, 0.5f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        for (int i = 0; i <= slices; ++i) {
            float angle = i * angleStep;
            float x = cos(angle) * rayon * 0.5;
            float y = sin(angle) * rayon * miroir * 0.5;
            glTexCoord2f(0.5f * (1.0f + cos(angle)), 0.5f * (1.0f + sin(angle)));
            glVertex3f(x, y, 0.0f);
        }
        glEnd();


        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.5f, 0.5f);
        glVertex3f(0.0f, 0.0f, height);
        for (int i = 0; i <= slices; ++i) {
            float angle = i * angleStep;
            float x = cos(angle) * rayon * 0.5;
            float y = sin(angle) * rayon * miroir * 0.5;
            glTexCoord2f(0.5f * (1.0f + cos(angle)), 0.5f * (1.0f + sin(angle)));
            glVertex3f(x, y, height);
        }
        glEnd();


        glBegin(GL_QUADS);
        glNormal3f(0.0f, -miroir, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(rayon*0.5, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(rayon*0.5, 0.0f, height);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-rayon*0.5, 0.0f, height);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-rayon*0.5, 0.0f, 0.0f);
        glEnd();

        glDisable(GL_NORMALIZE);
    }
    break;
    case 2: // Cube
    {
        glEnable(GL_NORMALIZE);

        glBegin(GL_QUADS);
        // Face avant (z = -rayon)
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, rayon*miroir, -rayon);
        glTexCoord2f(1,0); glVertex3f( rayon, rayon*miroir, -rayon);
        glTexCoord2f(1,1); glVertex3f( rayon,  0, -rayon);
        glTexCoord2f(0,1); glVertex3f(-rayon,  0, -rayon);
        // Face arrière (z = rayon)
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, rayon*miroir, rayon);
        glTexCoord2f(1,0); glVertex3f( rayon, rayon*miroir, rayon);
        glTexCoord2f(1,1); glVertex3f( rayon,  0, rayon);
        glTexCoord2f(0,1); glVertex3f(-rayon,  0, rayon);
        // Face gauche (x = -rayon)
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, rayon*miroir, -rayon);
        glTexCoord2f(1,0); glVertex3f(-rayon,  0, -rayon);
        glTexCoord2f(1,1); glVertex3f(-rayon,  0,  rayon);
        glTexCoord2f(0,1); glVertex3f(-rayon, rayon*miroir,  rayon);
        // Face droite (x = rayon)
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0,0); glVertex3f(rayon, rayon*miroir, -rayon);
        glTexCoord2f(1,0); glVertex3f(rayon,  0, -rayon);
        glTexCoord2f(1,1); glVertex3f(rayon,  0,  rayon);
        glTexCoord2f(0,1); glVertex3f(rayon, rayon*miroir,  rayon);
        // Face découpée (y = 0)
        glNormal3f(0.0f, -miroir, 0.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, 0, -rayon);
        glTexCoord2f(1,0); glVertex3f(rayon, 0, -rayon);
        glTexCoord2f(1,1); glVertex3f(rayon, 0,  rayon);
        glTexCoord2f(0,1); glVertex3f(-rayon, 0,  rayon);
        // Face supérieure
        glNormal3f(0.0f, miroir, 0.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, rayon*miroir, -rayon);
        glTexCoord2f(1,0); glVertex3f(rayon, rayon*miroir, -rayon);
        glTexCoord2f(1,1); glVertex3f(rayon, rayon*miroir,  rayon);
        glTexCoord2f(0,1); glVertex3f(-rayon, rayon*miroir,  rayon);
        glEnd();

        glDisable(GL_NORMALIZE);
    }
    break;
    case 3: // Pyramide
        glBegin(GL_TRIANGLES);
        // Base carrée
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, rayon*miroir, 0);
        glTexCoord2f(1,0); glVertex3f( rayon, rayon*miroir, 0);
        glTexCoord2f(1,1); glVertex3f( rayon,  0, 0);

        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, rayon*miroir, 0);
        glTexCoord2f(1,1); glVertex3f( rayon,  0, 0);
        glTexCoord2f(0,1); glVertex3f(-rayon,  0, 0);

        for (int i = 0; i < 4; ++i) {
            float angle1 = (i+0.5) * M_PI_2;
            float angle2 = (i+1.5) * M_PI_2;
            float x1 = rayon * cos(angle1) * sqrt(2), y1 = rayon * sin(angle1) * sqrt(2);
            float x2 = rayon * cos(angle2) * sqrt(2), y2 = rayon * sin(angle2) * sqrt(2);
            if (y1 < 0){
                y1 = 0;
            }else{
                y1 = y1*miroir;
            }
            if (y2 < 0){
                y2 = 0;
            }else{
                y2 = y2*miroir;
            }

            // Calcul des lumières
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

        glNormal3f(0.0f, miroir * (1.0), 0.0f);
        glTexCoord2f(0,0); glVertex3f(-rayon, 0, 0);
        glTexCoord2f(1,0); glVertex3f( rayon, 0, 0);
        glTexCoord2f(1,1); glVertex3f(0, 0, rayon*2);

        glEnd();
        break;
    default:
        // sphère d'erreur
        quadric = gluNewQuadric();
        gluQuadricTexture(quadric, GL_TRUE);
        gluSphere(quadric, this->rayon, 30, 30);
        gluDeleteQuadric(quadric);
        break;
    }

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


bool Residu::isTooFar(){

    if (this->x > 32){
        return true;
    }
    return false;
}

bool Residu::isCut(int xc, int yc, int zc){
    if (sqrt(pow((x-xc),2)+pow((y-yc),2)+pow((z-zc),2)) < 1.5){
        return true;
    }
    return false;
}


