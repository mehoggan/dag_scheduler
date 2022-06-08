set -x

BUILD_DIR=${1}

MAKE_CMD=$(which make)
echo "Going to build boostbeast in ${BUILD_DIR} with ${MAKE_CMD}..."

cd ${BUILD_DIR}/boostbeast-src && \
  export CC="/usr/bin/clang" && \
  export CXX="/usr/bin/clang++" && \
  export AR="/usr/bin/ar" && \
  export RANLIB="/usr/bin/ranlib" && \
  export LD="/usr/bin/ld" && \
  export LIBTOOL="/usr/bin/libtool" && \
  export MAKE="/usr/bin/make" && \
  export CFLAGS="-Wno-unknown-warning-option" && \
  export OPENSSL_ROOT="${BUILD_DIR}/openssl" && \
  export OPENSSL_ROOTDIR="${BUILD_DIR}/openssl" && \
  export OPENSSL_INCLUDE_DIR="${BUILD_DIR}/openssl/include" && \
  export OPENSSL_LIBRARIES="${BUILD_DIR}/openssl/lib" && \
  ./b2 -j2 cxxstd=11
