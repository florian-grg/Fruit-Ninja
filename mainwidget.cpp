#include "mainwidget.h"
#include <QHBoxLayout>
#include <QImage>
#include <QPixmap>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
    // Initialiser la taille de la fenêtre
    setFixedSize(1250, 800);

    // Créer les widgets
    m_GLWidget = new MyGLWidget(this);
    m_WebcamLabel = new QLabel(this);
    m_WebcamLabel->setFixedSize(300, 300); // Taille de l'image webcam
    m_WebcamLabel->setScaledContents(true); // Redimensionner l'image pour remplir le label
    m_buttonNew = new QPushButton("C'est parti !", this);

    QObject::connect(m_buttonNew, &QPushButton::clicked, [&]() {
        m_buttonNew->hide();
        m_GLWidget->startGame();
    });

    // organisation
    QHBoxLayout *layout = new QHBoxLayout(this);
    QVBoxLayout *layout2 = new QVBoxLayout(this);
    QVBoxLayout *layout3 = new QVBoxLayout(this);

    layout3->addWidget(m_buttonNew);

    layout2->addWidget(m_WebcamLabel);
    layout2->addLayout(layout3);

    layout->addWidget(m_GLWidget);
    layout->addLayout(layout2);
    setLayout(layout);


    // Initialisation du thread de la caméra
    m_CaptureWorker = new CaptureWorker();
    m_CaptureThread = new QThread();
    m_CaptureWorker->moveToThread(m_CaptureThread);

    // Connexions
    connect(m_CaptureThread, &QThread::started, m_CaptureWorker, &CaptureWorker::process);
    connect(m_CaptureWorker, &CaptureWorker::newFrame, this, &MainWidget::onNewFrame);
    connect(this, &MainWidget::destroyed, m_CaptureThread, &QThread::quit);
    connect(m_CaptureThread, &QThread::finished, m_CaptureWorker, &QObject::deleteLater);

    connect(m_GLWidget, &MyGLWidget::gameOver, this, &MainWidget::onGameOver);

    m_CaptureThread->start();

}

MainWidget::~MainWidget()
{

}

void MainWidget::onNewFrame(const QImage& image, float x, float y)
{
    m_WebcamLabel->setPixmap(QPixmap::fromImage(image));

    if (x != -300) m_GLWidget->updateCoordinates(x, y);
}

void MainWidget::onGameOver(){
    m_buttonNew->show();
}
