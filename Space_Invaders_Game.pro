TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        player.cpp

INCLUDEPATH += "C:/Users/George/Desktop/Automatic Control and Robotics/Semester 2/Information Engineering/SFML-2.5.1/include"

LIBS += -L"C:/Users/George/Desktop/Automatic Control and Robotics/Semester 2/Information Engineering/SFML-2.5.1/lib"
CONFIG(debug, debug|release){
    LIBS += -lsfml-audio-d -lsfml-graphics-d -lsfml-network-d -lsfml-system-d -lsfml-window-d
} else {
    LIBS += -lsfml-audio -lsfml-graphics -lsfml-network -lsfml-system -lsfml-window
}

# -------------------Make sure the project name is not too long---------------

HEADERS += \
    player.h
