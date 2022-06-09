BUILD_DIR=${1}

echo "Going to configure openssl in ${BUILD_DIR}..."

cd ${BUILD_DIR}/openssl-src && \
  export CC="/usr/bin/clang" && \
  export AR="/usr/bin/ar" && \
  export RANLIB="/usr/bin/ranlib" && \
  export LD="/usr/bin/ld" && \
  export LIBTOOL="/usr/bin/libtool" && \
  export MAKE="/usr/bin/make" && \
  export CFLAGS="-Wno-unknown-warning-option" && \
  ./Configure shared \
    --prefix=${BUILD_DIR}/openssl \
    --openssldir=${BUILD_DIR}/openssl
