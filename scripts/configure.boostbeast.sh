BUILD_DIR=${1}

echo "Going to configure boostbeast in ${BUILD_DIR}..."

cd ${BUILD_DIR}/boostbeast-src && \
  export CC="/usr/bin/clang" && \
  export AR="/usr/bin/ar" && \
  export RANLIB="/usr/bin/ranlib" && \
  export LD="/usr/bin/ld" && \
  export LIBTOOL="/usr/bin/libtool" && \
  export MAKE="/usr/bin/make" && \
  export CFLAGS="-Wno-unknown-warning-option" && \
  ./bootstrap.sh \
    --prefix=${CMAKE_BINARY_DIR}/boostbeast \
    --with-toolset=clang
