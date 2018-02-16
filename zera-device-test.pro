QT += qml quick sql network #quickcontrols2
CONFIG += c++11
DEFINES += VEIN_COLORS_SUPPORT


#dependencies
VEIN_DEP_EVENT = 1
VEIN_DEP_COMP = 1
VEIN_DEP_NET2 = 1
VEIN_DEP_TCP2 = 1
VEIN_DEP_HELPER = 1
VEIN_DEP_HASH = 1
VEIN_DEP_QML = 1
VEIN_DEP_LOGGER = 1

contains(DEFINES, OE_BUILD) {
  message(Openembedded build)
  include(../oe-build.pri)
}
else {
  INCLUDEPATH += /work/qt_projects/distrib/usr/include/
  LIBS += -L/work/qt_projects/vein-framework/libs
  exists( /work/qt_projects/vein-framework//vein-framework.pri ) {
    include(/work/qt_projects/vein-framework//vein-framework.pri)
  }
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    vfdt_localintrospection.cpp \
    vfdt_networkcontroller.cpp \
    vfdt_testcontroller.cpp \
    vfdt_testrunner.cpp \
    vfdt_testresult.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
target.path = /usr/bin
INSTALLS += target

HEADERS += \
    vfdt_networkcontroller.h \
    vfdt_localintrospection.h \
    vfdt_testcontroller.h \
    vfdt_testrunner.h \
    vfdt_testresult.h
