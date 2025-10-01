FROM ubuntu:latest
#FROM debian:bullseye
#FROM madworx/debian-archive:wheezy


RUN apt-get update && apt-get install -y \
    build-essential cmake git qt6-tools-dev qt6-base-private-dev libproc2-dev libopencv-dev

RUN    apt-get install -y libproc2-dev libopencv-dev libopencv-dev
RUN    apt-get install -y qt6-tools-dev qt6-serialport-dev qt6-base-private-dev
RUN    apt-get install -y gcc-multilib g++-multilib
RUN    dpkg --add-architecture i386 && apt-get update && \
        apt-get install -y \
            wine \
            wine32 \
            wine64 \
            libwine \
            libwine:i386 \
            fonts-wine
RUN     apt-get install -y wine64-tools
RUN     apt-get install -y cmake-curses-gui

COPY assets/build.sh /build.sh
RUN chmod a+x  /build.sh



