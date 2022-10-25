BUILD_DIR=${1}
SUB_FOLDER=${2}

set -e

echo "Going to install openssl..."

if [[ "${OSTYPE}" == "linux-gnu"* ]]; then
  cd ${BUILD_DIR}/${SUB_FOLDER} && \
    /usr/bin/make install
elif [[ "${OSTYPE}" == "darwin"* ]]; then
  cd ${BUILD_DIR}/${SUB_FOLDER} && \
    export CC="/usr/bin/clang" && \
    export AR="/usr/bin/ar" && \
    export RANLIB="/usr/bin/ranlib" && \
    export LD="/usr/bin/ld" && \
    export LIBTOOL="/usr/bin/libtool" && \
    export MAKE="/usr/bin/make" && \
    /usr/bin/make install
fi
