# This docker file uses the latest version of cmake but it also can be
# configured using the CMAKE_TAR variable. To build using another version
# run the docker build command as follows:

# docker build \
#   --build-arg CMAKE_TAR="<URL to cmake tar>"
#   --build-arg CMAKE_VER=<Version found in the URL above>
#   .
FROM debian

RUN mkdir -p /home/dag_scheduler

RUN apt-get update
RUN apt-get install -y \
  g++ \
  uuid \
  uuid-dev \
  git \
  wget \
  libssl-dev \
  build-essential \
  valgrind \
  doxygen

ARG CMAKE_TAR="https://github.com/Kitware/CMake/releases/download/v3.23.0-rc1/cmake-3.23.0-rc1.tar.gz"
ARG CMAKE_VER=3.23.0
RUN wget --no-check-certificate $CMAKE_TAR -O /opt/cmake-$CMAKE_VER.tar.gz && \
  mkdir -p /opt/cmake-$CMAKE_VER && \
  cd /opt && tar -xf cmake-$CMAKE_VER.tar.gz -C /opt/cmake-$CMAKE_VER 

RUN cd /opt/cmake-$CMAKE_VER/cmake-$CMAKE_VER-rc1 && \
  ./configure && \
  make -j 5 && \
  make install && \
  ln -s /usr/local/bin/cmake /usr/bin/cmake

ADD . /home/dag_scheduler
ENV GIT_SSL_NO_VERIFY=1
RUN cd /home/dag_scheduler && \
  mkdir -p build && \
  cd build && \
  cmake -DCMAKE_VERBOSE_MAKEFILE=On -DCMAKE_BUILD_TYPE=Debug ../ # && \
  make -j 5
