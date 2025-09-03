/**
 * @file residu.h
 * @brief Déclaration de la classe Residu, représentant un projectile résiduel après une découpe.
 * @author Auteur : Florian GIURGIU - Tom PAILLET
 */

#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <GL/glu.h>
#include <qopengl.h>
#include <QColor>
#include <QOpenGLTexture>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "residu.h"

/**
 * @class Projectile
 * @brief Classe représentant un projectile dans le jeu.
 */

class Projectile
{
private:
    float xt; // coordonnée de départ
    float yt; // coordonnée de départ
    float zt; // coordonnée de départ
    float xs; // coordonnée de départ
    float a; // coordonnée de départ
    float b; // coordonnée de départ
    float rayon; // rayon de la sphère
    float t_i; // temps d'impact

    float x; // coordonnée de départ
    float y; // coordonnée de départ
    float z; // coordonnée de départ


    QOpenGLTexture* texture; ///< Texture OpenGL
    QString m_TextureName; ///< Nom de la texture
    int m_FormeType = 0; ///< Type de forme (0 = sphère, 1 = cube, etc.)

    // vitesses de rotation
    float rotSpeedX = 0.0f, rotSpeedY = 0.0f, rotSpeedZ = 0.0f; // vitesses de rotation
    float rotAngleX = 0.0f, rotAngleY = 0.0f, rotAngleZ = 0.0f; // angles courants
public:
    /**
     * @brief Constructeur de la classe Projectile.
     * @param t_i Temps d'impact
     * @param texturePath Chemin vers la texture à appliquer
     * @param debug Indique si le mode débogage est activé
     */
    Projectile(float t_i, const QString& texturePath);

    /**
     * @brief Destructeur de la classe Projectile.
     */
    virtual ~Projectile();

    /**
     * @brief Charge la texture à partir du chemin spécifié.
     * @param texturePath Chemin vers la texture à charger
     */        
    void loadTexture(const QString& texturePath);

    GLuint m_TextureId; ///< Identifiant de la texture

    /**
     * @brief Affiche le projectile.
     * @param timeInDays Temps écoulé en jours
     */
    void Display(const float timeInDays);

    /**
     * @brief Charge la texture à partir du chemin spécifié.
     * @param texturePath Chemin vers la texture à charger
     */
    void LoadTexture(const QString& texturePath);

    /**
     * @brief Texture de la sphère.
     */
    void BindTexture() const;

    /**
     * @brief Détermine si le projectile a dépassé sa portée autorisée.
     * @return true si le projectile est trop éloigné de son point d'origine ou de départ ; false sinon.
     */
    bool isTooFar();

    /**
     * @brief Vérifie si le projectile est en dehors des limites de la scène.
     * @param xc Coordonnée x du projectile
     * @param yc Coordonnée y du projectile
     * @param zc Coordonnée z du projectile
     * @return true si le projectile est en dehors des limites ; false sinon.
     */
    bool isCut(int xc, int yc, int zc);

    /**
     * @brief Génère un résidu de coupe à un instant donné.
     * @param timeInDays Temps écoulé en jours depuis le début ou un événement de référence.
     * @param miroir Indique si la coupe doit être réalisée en mode miroir (true) ou non (false).
     * @return Un pointeur vers l'objet Residu généré.
     */
    Residu* genererCoupe(float timeInDays, bool miroir);
};


#endif // PROJECTILE_H
