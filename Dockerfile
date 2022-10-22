FROM debian

RUN mkdir -p /home/dag_scheduler

WORKDIR /home/dag_scheduler
RUN ls -la .

RUN apt-get install -y apt-transport-https
RUN apt-get update
RUN apt-get install -y \
  g++ \
  uuid \
  uuid-dev \
  git-all \
  build-essential \
  valgrind \
  autoconf \
  automake \
  libtool \
  autoconf-archive \
  libgtest-dev \
  rapidjson-dev \
  libyaml-cpp-dev \
  doxygen
RUN autoreconf -i
RUN mkdir -p ./build; pushd ./build; ../configure && make; popd
