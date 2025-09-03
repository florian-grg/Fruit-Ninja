/**
 * @file mainwidget.h
 * @brief Déclaration de la classe MainWidget, fenêtre principale de l'application.
 * @author Auteur : Florian GIURGIU - Tom PAILLET
 */

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "myglwidget.h"
#include "captureWorker.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <opencv2/opencv.hpp>

/**
 * @class MainWidget
 * @brief Fenêtre principale de l'application, gère l'interface et la communication entre la webcam et la scène 3D.
 */
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur de la fenêtre principale.
     * @param parent Pointeur vers le widget parent (par défaut nullptr).
     */
    MainWidget(QWidget *parent = nullptr);

    /**
     * @brief Destructeur de la fenêtre principale.
     */
    ~MainWidget();

private slots:
    /**
     * @brief Slot appelé lorsque le bouton "Nouvelle partie" est cliqué.
     */
    void onNewFrame(const QImage& image, float x, float y);

    /**
     * @brief Slot appelé lorsque le bouton "Nouvelle partie" est cliqué.
     */
    void onGameOver();

private:
    MyGLWidget *m_GLWidget; ///< Widget OpenGL pour le rendu 3D
    QLabel *m_WebcamLabel; ///< Label pour afficher l'image de la webcam
    QPushButton *m_buttonNew; ///< Bouton pour démarrer une nouvelle partie
    cv::VideoCapture m_Cap; ///< Capture vidéo de la webcam

    QTimer m_WebcamTimer; ///< Timer pour la mise à jour de l'image de la webcam
    bool m_CameraInitialized = false; ///< Indique si la caméra est initialisée

    void initCamera(); ///< Initialise la caméra

    QThread* m_CaptureThread = nullptr; ///< Thread pour le traitement de la capture vidéo
    CaptureWorker* m_CaptureWorker = nullptr; ///< Worker pour la capture vidéo
};

#endif // MAINWIDGET_H
