#include "captureWorker.h"

CaptureWorker::CaptureWorker() : running(false) {
    if (!m_FaceCascade.load("../../palm.xml")) {
        std::cerr << "Error loading haarcascade" << std::endl;
        return;
    }

}

CaptureWorker::~CaptureWorker() {
    stop();
}

QImage CaptureWorker::matToQImage(const cv::Mat &mat) {
    if (mat.channels() == 3) {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        return QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    } else if (mat.channels() == 1) {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    }
    return QImage();
}

void CaptureWorker::process() {
    cap.open(0);

    if (!cap.isOpened()) {
        qWarning("Camera not opened");
        return;
    }

    running = true;

    while (running) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) continue;

        cv::flip(frame, frame, 1);

        cv::Mat frameGray;
        cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);

        // Détecter les paumes
        std::vector<cv::Rect> faces;
        m_FaceCascade.detectMultiScale(frameGray, faces, 1.1, 4, 0, cv::Size(30, 30));

        // Dessiner les rectangles et centres
        if (!faces.empty()) {
            const auto& face = faces[0];
            cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
            cv::Point center(face.x + face.width / 2, face.y + face.height / 2);
            cv::circle(frame, center, 5, cv::Scalar(0, 255, 0), -1);
        }

        float x = -30; //default value
        float y;

        //Stocker les infos
        if (!faces.empty()) {
            const cv::Rect& face = faces[0];
            printf("origine : x = %d, y = %d, width = %d, height = %d\n",face.x, face.y, face.width, face.height);
            x = (face.x + face.width / 2.0) / frame.cols;
            y = (face.y + face.height / 2.0) / frame.rows;
            y = y * -1.0 + 1.0;      // retournement vertical
            addValueX(x);
            addValueY(y);
        }

        QImage image = matToQImage(frame);

        emit newFrame(image, getAverageX(), getAverageY());

        QThread::msleep(30); // 30 FPS
    }

    cap.release();
}

void CaptureWorker::addValueX(double val) {
    bufferX.push_back(val);
    sumX += val;

    if (bufferX.size() > 5) {
        sumX -= bufferX.front();
        bufferX.erase(bufferX.begin());
    }
}

double CaptureWorker::getAverageX() const {
    if (bufferX.empty()) return 0.0;
    return sumX / bufferX.size();
}

void CaptureWorker::addValueY(double val) {
    bufferY.push_back(val);
    sumY += val;

    if (bufferY.size() > 5) {
        sumY -= bufferY.front();
        bufferY.erase(bufferY.begin());
    }
}

double CaptureWorker::getAverageY() const {
    if (bufferY.empty()) return 0.0;
    return sumY / bufferY.size();
}

void CaptureWorker::stop() {
    running = false;
}
