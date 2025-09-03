/**
 * @file residu.h
 * @brief Déclaration de la classe Residu, représentant un projectile résiduel après une découpe.
 * @author Auteur : Tom PAILLET
 */

#ifndef RESIDU_H
#define RESIDU_H

#include <GL/glu.h>
#include <qopengl.h>
#include <QColor>
#include <QOpenGLTexture>
#include <stdlib.h>
#include <time.h>
#include <math.h>



/// Projectiles résiduels après une découpe


class Residu
{
private:
    float a; // coordonnée de départ
    float b; // coordonnée de départ
    float dy; // coordonnée de départ
    float rayon; // rayon de la sphère
    float t_i; // temps d'impact
    float t_e; // temps d'éjection

    float x; // coordonnée de départ
    float y; // coordonnée de départ
    float z; // coordonnée de départ

    float miroir; // miroir ou pas

    QOpenGLTexture* texture; ///< Texture OpenGL
    QString m_TextureName; ///< Nom de la texture
    int m_FormeType = 0; ///< Type de forme (0 = sphère, 1 = cube, etc.)

    float rotAngleX = 0.0f, rotAngleY = 0.0f, rotAngleZ = 0.0f; // angles courants
    float rotSpeedX = 0.0f, rotSpeedY = 0.0f, rotSpeedZ = 0.0f; // vitesses de rotation
public:
    /**
     * @brief Constructeur de la classe Residu.
     * @param a Coordonnée de départ en x
     * @param b Coordonnée de départ en y
     * @param ti Temps d'impact
     * @param te Temps d'éjection
     * @param dy Coordonnée de départ en z
     * @param miroir Indique si le résidu est un miroir
     * @param texturePath Chemin vers la texture à appliquer
     * @param rotAngleX Angle de rotation autour de l'axe X
     * @param rotAngleY Angle de rotation autour de l'axe Y
     * @param rotAngleZ Angle de rotation autour de l'axe Z
     * @param rotSpeedX Vitesse de rotation autour de l'axe X
     * @param rotSpeedY Vitesse de rotation autour de l'axe Y
     * @param rotSpeedZ Vitesse de rotation autour de l'axe Z
     */
    Residu(float a, float b, float ti, float te, float dy, bool miroir, const QString& texturePath,
           float rotAngleX = 0, float rotAngleY = 0, float rotAngleZ = 0,
           float rotSpeedX = 0, float rotSpeedY = 0, float rotSpeedZ = 0);

    /**
    * @brief Destructeur de la classe Residu.
    */
    virtual ~Residu();

    /**
     * @brief Charge la texture à partir du chemin spécifié.
     * @param texturePath Chemin vers la texture à charger.
     */
    void loadTexture(const QString& texturePath);


    GLuint m_TextureId; ///< Identifiant de la texture OpenGL

    /**
     * @brief Affiche le résidu en fonction du temps écoulé.
     * @param timeInDays Temps écoulé depuis le début de l'animation.
     */
    void Display(const float timeInDays);

    /**
     * @brief Charge la texture à partir du chemin spécifié.
     * @param texturePath Chemin vers la texture à charger.
     */
    void LoadTexture(const QString& texturePath);

    /**
     * @brief Lie la texture pour l'affichage.
     */
    void BindTexture() const;

    /**
     * @brief Vérifie si le résidu est trop éloigné.
     * @return true si le résidu est trop éloigné, false sinon.
     */
    bool isTooFar();

    /**
     * @brief Vérifie si le résidu a été découpé.
     * @param xc Coordonnée x de la découpe
     * @param yc Coordonnée y de la découpe
     * @param zc Coordonnée z de la découpe
     * @return true si le résidu a été découpé, false sinon.
     */
    bool isCut(int xc, int yc, int zc);
};


#endif // RESIDU_H
