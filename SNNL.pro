TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        headers/layer.cpp \
        headers/mathfunc.cpp \
        headers/network.cpp \
        headers/neural.cpp \
        headers/neuron.cpp \
        headers/sublayer.cpp \
        headers/test.cpp \
        main.cpp

DISTFILES += \
    .gitignore \
    CMakeLists.txt \
    LICENSE \
    README.md \
    SNNL.pro.user \
    compile.bat

HEADERS += \
    headers/layer.hpp \
    headers/mathfunc.hpp \
    headers/network.hpp \
    headers/neural.hpp \
    headers/neuron.hpp \
    headers/sublayer.hpp \
    headers/test.h
