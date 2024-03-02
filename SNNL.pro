TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        blockbp.cpp \
        main.cpp \
        newralnet.cpp \
        perseptron.cpp \
        test.cpp

DISTFILES += \
    .gitignore \
    CMakeLists.txt \
    LICENSE \
    README.md \
    compile.bat

HEADERS += \
    blockbp.h \
    newralnet.h \
    perseptron.h \
    test.h
