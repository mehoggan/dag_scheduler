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
  build-essential \
  valgrind \
  doxygen
