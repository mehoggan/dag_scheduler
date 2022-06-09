BUILD_DIR=${1}
BOOSTBEAST_REPO=${2}

echo "Going to clone boostbeast to ${BUILD_DIR}..."
 
git clone --recursive ${BOOSTBEAST_REPO} ${BUILD_DIR}/boostbeast-src
