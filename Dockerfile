FROM ubuntu:20.04

RUN apt update

RUN apt install -y cmake

RUN apt install -y build-essential

RUN apt install -y libboost-filesystem1.67-dev libboost-date-time1.67-dev libboost-log1.67-dev libboost-regex1.67-dev libboost-system1.67-dev libboost-test1.67-dev libboost-program-options1.67-dev

RUN apt install -y libavdevice-dev libavfilter-dev libavformat-dev libswscale-dev

RUN apt install -y libxcb-xinerama0

RUN apt install -y libfftw3-dev

RUN apt install -y libsdl-dev

RUN apt install -y libssl-dev

RUN apt install -y libhidapi-dev

RUN apt install -y libjpeg-dev

RUN apt install -y libzip-dev

RUN apt install -y libeigen3-dev

RUN apt install -y libopencv-dev

RUN apt install -y qt5-default qtdeclarative5-dev qml-module-qt-labs-settings qml-module-qt-labs-folderlistmodel qml-module-qtquick-dialogs qml-module-qtquick-controls

RUN apt install -y qml-module-qtquick-layouts qml-module-qtquick-window2 libgles2-mesa-dev qtquickcontrols2-5-dev

# add you libs here
