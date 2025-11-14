#FROM ubuntu:latest
#FROM debian:bullseye
FROM debian:bookworm
#FROM madworx/debian-archive:wheezy


RUN dpkg --add-architecture i386 && apt-get update && apt-get install -y \
    build-essential cmake cmake-curses-gui git qt6-tools-dev qt6-serialport-dev qt6-base-private-dev libproc2-dev libopencv-dev \
    gcc-multilib g++-multilib \
    wine \
    wine32 \
    wine64 \
    libwine \
    libwine:i386 \
    libwine-dev \
    libwine-dev:i386 \
    fonts-wine \
    wine64-tools g++-mingw-w64-x86-64 gcc-mingw-w64-x86-64  gcc-mingw-w64-i686 g++-mingw-w64-i686

RUN apt-get -y clean

COPY assets/build.sh /build.sh
RUN chmod a+x  /build.sh



