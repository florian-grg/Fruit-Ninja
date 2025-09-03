/**
 * @file residu.h
 * @brief Déclatation de la classe captureWorker, qui gère la capture vidéo et le traitement d'image.
 * @author Auteur : Florian GIURGIU - Tom PAILLET
 */
#ifndef CAPTUREWORKER_H
#define CAPTUREWORKER_H


#include <QObject>
#include <QThread>
#include <QImage>
#include <vector>
#include <opencv2/opencv.hpp>

/**
 * @class CaptureWorker
 * @brief Classe qui gère la capture vidéo et le traitement d'image.
 */
class CaptureWorker : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructeur de la classe CaptureWorker.        
     */
    CaptureWorker();

    /**
     * @brief Destructeur de la classe CaptureWorker.
     */
    ~CaptureWorker();

public slots:
    /**
     * @brief Démarre la capture vidéo et le traitement d'image.
     */
    void process();

    /**
     * @brief Arrête la capture vidéo.
     */
    void stop();

signals:
    /**
     * @brief Signal émis à chaque nouvelle image capturée.
     * @param image Image capturée.
     * @param x Coordonnée x de la main.
     * @param y Coordonnée y de la main.
     */
    void newFrame(const QImage &image, float x, float y);

private:
    cv::VideoCapture cap; ///< Capture vidéo de la webcam
    std::atomic<bool> running; ///< Indique si la capture est en cours
    std::vector<double> bufferX; ///< Tampon pour les coordonnées x
    std::vector<double> bufferY; ///< Tampon pour les coordonnées y

    double sumX = 0.0; ///< Somme des coordonnées x
    double sumY = 0.0; ///< Somme des coordonnées y

    /**
     * @brief Retourne la moyenne des X sur les 5 dernières valeurs.
     * @return Moyenne des X.
     */
    double getAverageX() const;

    /**
     * @brief Retourne la moyenne des Y sur les 5 dernières valeurs.
     * @return Moyenne des Y.
     */
    double getAverageY() const;

    /**
     * @brief Ajoute une valeur au tampon des coordonnées x.
     * @param val Valeur à ajouter.
     */
    void addValueX(double val);

    /**
     * @brief Ajoute une valeur au tampon des coordonnées y.
     * @param val Valeur à ajouter.
     */
    void addValueY(double val);

    cv::CascadeClassifier m_FaceCascade; ///< Classificateur en cascade pour la détection de visage

    /**
     * @brief Convertit une image OpenCV (cv::Mat) en QImage.
     * @param mat Image OpenCV à convertir.
     * @return Image QImage convertie.
     */
    QImage matToQImage(const cv::Mat &mat);
};



#endif // CAPTUREWORKER_H
