# User Notes

# Developer Notes

## Build Dependencies

If you wish to clone all dependencies at once at the time of clone run:

```sh
> git clone --recurse-submodules -j8 git@gitlab.com:m8665/dag_scheduler.git
```

If you did not clone recursively then run the following.

```sh
> pushd ./deps/openssl; git submodule update --init --recursive; popd
> pushd ./deps/boost; git submodule update --init --recursive; popd
```

Once the dependencies and all their dependencies are up-to-date, run the </br>
following command from the root directory of this repository. </br>

```sh
> ./scripts/setup.deps.sh
```

## OSX System Setup

```sh
> brew install autoconf automake libtool autoconf-archive googletest \
    rapidjson yaml-cpp
```

## Fedora DNF System Setup

```sh
> sudo dfn install -y ...
```

## Debian Apt System Setup

```sh
> sudo apt install -y git-all autoconf automake libtool autoconf-archive \
    libgtest-dev rapidjson-dev 	libyaml-cpp-dev
```

## Build file generation and compiling library and executable Unix Systems

```sh
> autoreconf -i
> rm -rf ./build && mkdir -p ./build
> cd build/ && ./configure
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
