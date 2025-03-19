# User Notes

# Developer Notes

## Build Dependencies

```sh
> git clone git@gitlab.com:m8665/dag_scheduler.git
```

## OSX System Setup

### Prerequisites
```sh
> brew install \
    autoconf@2.72 \
    automake \
    libtool \
    autoconf-archive \
    pkgconf \
    pkg-config \
    llvm \
    ossp-uuid
```

```sh
> cd [Path to Where You Want To Build Boost From]
> mkdir boostorg
> cd boostorg
> git clone https://github.com/boostorg/boost.git
> git checkout tags/boost-1.87.0 -b boost-1.87.0
> git submodule update --init --recursive
> ./bootstrap.sh --prefix=/usr/local/boost --with-toolset=clang
> ./b2 clean
> sudo mkdir /usr/local/boost
> sudo ./b2 \
    toolset=clang \
    cxxflags="-stdlib=libc++" \
    linkflags="-stdlib=libc++" \
    --prefix=/usr/local/boost install
> echo "export PKG_CONFIG_PATH=$(which pkg-config)" >> [Your RC file]
> echo "export PKG_CONFIG=$(which pkg-config)" >> [Your RC file]
> echo "export HOMEBREW_PREFIX=$(brew --prefix)" >> ~/.bash_profile
> echo "export LDFLAGS=\"-L[BoostBuild Prefix]/lib -L${HOMEBREW_PREFIX}/lib\"" >> ~/.bash_profile
> echo "export CPPFLAGS=\"-I[BoostBuild Prefix]/include -I${HOMEBREW_PREFIX}/include\"" >> ~/.bash_profile
```

## DNF Based System Setup (RHEL, CentOS, Fedora, ...)

```sh
> sudo dfn install -y ...
```

## Debian Apt System Setup

```sh
> sudo apt install -y \
  build-essential=12.9 \
  autotools-dev=20220109.1 \
  autoconf-archive=20220903-3 \
  checkinstall=1.6.2+git20170426.d24a630-3+b1 \
  zlib1g-dev=1:1.2.13.dfsg-1 \
  libbz2-dev=1.0.8-5+b1 \
  liblzma-dev=5.4.1-0.2 \
  libzstd-dev=1.5.4+dfsg2-5 \
  valgrind=1:3.19.0-1 \
  uuid-dev=2.38.1-5+deb12u3 \
  libgtest-dev=1.12.1-0.2 \
  libgmock-dev=1.12.1-0.2 \
  rapidjson-dev=1.1.0+dfsg2-7.1 \
  libyaml-cpp-dev=0.7.0+dfsg-8+b1 \
  doxygen=1.9.4-4 \
  autoconf-archive=20220903-3 \
  checkinstall=1.6.2+git20170426.d24a630-3+b1 \
  libtool=2.4.7-7~deb12u1 \
  libtool-bin=2.4.7-7~deb12u1 \
  make=4.3-4.1 \
  packaging-dev=0.8.2 \
  libboost-all-dev=1.74.0.3 \
  libssl-dev=3.0.15-1~deb12u1
```

## Build file generation and compiling library and executable Unix Systems

```sh
> autoreconf -i
> rm -rf ./build && mkdir -p ./build
> cd build/ && ../configure
> cd ../
> make -C build
```

## Running Tests

```sh
> make check

// If a test fails you can find that specific test in the build logs and then
// run:
> ./test/gtest_libdag_scheduler --gtest_filter=<name of test>
// example of gtest_filter is:
//   "TestYAMLDagDeserializer.make_dag_vertices_not_list_of_obj"

// If you cannot find the logs then you can simply just run:
> ./test/gtest_libdag_scheduler
```

## Build Dockerfile(s)

```sh
> rm -rf ./build
> docker build -f ./<distro>.docker
```

The base images for each distribution can be found at git@github.com:mehoggan/docker.io.git.
Instructions on how to build each container and push it to Docker Hub (docker.io) can
be found in the README.md of that repository.

## Notes on Autoformatting Code

First install clang-formatter

### OSX Setup

```sh
> brew install clang-format
```

### Running clang-format
```sh
> ./scripts/clang-format.sh
```

## Notes on vim Development on OSx##
The working vim environment can be found at git@github.com:mehoggan/vim_config.git

To get the ALE plugin to work you will need to run the following:

```sh
brew install clazy-standalone
brew install cppcheck cpplint
brew install ccls
brew install cspell
brew install flawfinder cquery
```

```sh
brew install bear
# After following the appropriate build steps in the build section above, run:
bear -- ./configure
bear -- make
```

## Force Change 1 ##

