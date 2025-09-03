#-------------------------------------------------
#
# Project created by QtCreator 2012-10-23T11:26:13
#
#-------------------------------------------------

QT       -= gui core

equals(QT_MAJOR_VERSION, 5) {
        QT += opengl widgets
} else:equals(QT_MAJOR_VERSION, 6) {
        QT += openglwidgets
}

TARGET = minecraft_ninja
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# ajout des libs au linker
win32 {
    win32-msvc* {
        LIBS     += opengl32.lib glu32.lib
    } else {
        LIBS     += -lopengl32 -lglu32
    }
} else:macx {
        LIBS     += -framework OpenGL
} else {
        LIBS     += -lGL -lGLU
}


SOURCES += \
    captureWorker.cpp \
    mainwidget.cpp \
    minecraft_ninja.cpp \
    myglwidget.cpp \
    projectile.cpp \
    residu.cpp

RESOURCES += \
    res/textures.qrc

INCLUDEPATH +=$$(OPENCV_DIR)\..\..\include

LIBS += -L$$(OPENCV_DIR)\lib \
    -lopencv_core4100 \
    -lopencv_highgui4100 \
    -lopencv_imgproc4100 \
    -lopencv_imgcodecs4100 \
    -lopencv_videoio4100 \
    -lopencv_features2d4100 \
    -lopencv_calib3d4100 \
    -lopencv_objdetect4100

HEADERS += \
    captureWorker.h \
    mainwidget.h \
    myglwidget.h \
    projectile.h \
    residu.h

DISTFILES +=

