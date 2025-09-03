/**
 * @file residu.h
 * @brief Déclaration de la classe Residu, représentant un projectile résiduel après une découpe.
 * @author Auteur : Florian GIURGIU - Tom PAILLET
 */

#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include "projectile.h"
#include "residu.h"
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QTimer>

/**
 * @class MyGLWidget
 * @brief Classe représentant le widget OpenGL pour le rendu 3D et la logique du jeu.
 */
class MyGLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur de la classe MyGLWidget.
     * @param parent Pointeur vers le widget parent (par défaut nullptr).
     */
    MyGLWidget(QWidget * parent = nullptr);

    /**
     * @brief Met à jour les coordonnées du découpeur selon la position de la main.
     * @param x Coordonnée x de la main.
     * @param y Coordonnée y de la main.
     */
    void updateCoordinates(float x, float y);

    /**
     * @brief Démarre le jeu.
     */
    void startGame();

protected:
    /**
     * @brief Fonction appelée lors de l'initialisation OpenGL.
     */
    void initializeGL();

    /**
     * @brief Fonction appelée lors du redimensionnement de la fenêtre.
     * @param width Largeur de la fenêtre.
     * @param height Hauteur de la fenêtre.
     */
    void resizeGL(int width, int height);

    /**
     * @brief Fonction de rendu appelée à chaque frame.
     */
    void paintGL();

    /**
     @brief Megt à jour la logique du jeu.
     @param chaines : vecteur de chaînes de caractères représentant les objets à afficher.
     */
    void updateJeu(QVector<const QString*> chaines);

signals:
    /**
     * @brief Signal émis lorsque le jeu est terminé.
     */
    void gameOver();

private:
    /**
     * @brief Fonction appelée à la fin du jeu pour afficher le score et les animations finales.
     */
    void finJeu();

    bool isRunning = false; ///< Indique si le jeu est en cours d'exécution

    float m_TimeElapsed { 0.0f }; ///< Temps écoulé depuis le début du jeu
    float delay = 2; ///< Délai entre les animations
    float timeLastThrow = 0; ///< Temps de la dernière animation
    QTimer m_AnimationTimer; ///< Timer pour les animations
    std::vector<Projectile*> m_Projectiles; ///< Vecteur de projectiles
    std::vector<Residu*> m_Residu; ///< Vecteur de résidus

    float timeStart; ///< Temps de début du jeu
    int score; ///< Score du joueur

    float xd = -300; ///< Coordonnée x du découpeur
    float yd; ///< Coordonnée y du découpeur
    float zd; ///< Coordonnée z du découpeur

};

#endif // MYGLWIDGET_H
