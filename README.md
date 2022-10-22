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

## Build file generation Unix Systems

```sh
> autoreconf -i
> ./configure
> make
```
