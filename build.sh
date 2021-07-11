set -e #set -oerrexit

ROOT_DIR=`pwd`
echo Building root directory: ${ROOT_DIR}

mkdir -p build;
cd build;

cmake .. \
    -DROOT_DIR=${ROOT_DIR}

make