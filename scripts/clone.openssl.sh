BUILD_DIR=${1}
OPENSSL_REPO=${2}

echo "Going to clone openssl to ${BUILD_DIR}..."

git clone --recursive ${OPENSSL_REPO} ${BUILD_DIR}/openssl-src
