if [ -f /etc/os-release ]; then
    # freedesktop.org and systemd
    . /etc/os-release
    OS=$NAME
    VER=$VERSION_ID
elif type lsb_release >/dev/null 2>&1; then
    # linuxbase.org
    OS=$(lsb_release -si)
    VER=$(lsb_release -sr)
elif [ -f /etc/lsb-release ]; then
    # For some versions of Debian/Ubuntu without lsb_release command
    . /etc/lsb-release
    OS=$DISTRIB_ID
    VER=$DISTRIB_RELEASE
elif [ -f /etc/debian_version ]; then
    # Older Debian/Ubuntu/etc.
    OS=Debian
    VER=$(cat /etc/debian_version)
elif [ -f /etc/SuSe-release ]; then
    # Older SuSE/etc.
    ...
elif [ -f /etc/redhat-release ]; then
    # Older Red Hat, CentOS, etc.
    ...
else
    # Fall back to uname, e.g. "Linux <version>", also works for BSD, etc.
    OS=$(uname -s)
    VER=$(uname -r)
fi

echo $OS' '$VER

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
echo "Script dir : "$SCRIPT_DIR

# if [ "$OS" = "Ubuntu" ]; then
#    $SCRIPT_DIR/prepareSdkUbuntu.sh
# fi
source $SCRIPT_DIR/setEnv.sh
PROJECT_DIR="$(dirname "$SCRIPTDIR")"
cd $USERROOT
EXTERNALLIBS="ExternalLibs"
mkdir -p $EXTERNALLIBS
cd $EXTERNALLIBS
PWD=$(pwd)
echo $PWD

DISTCC=false
for arg ; do
    if [[ "$arg" == "distcc" ]] ; then
        DISTCC=true
        continue
    fi
done

rm -rf *

if "$DISTCC"; then
    cmake ../../ProjectUtils/SDK/ -DSDK_DIR:STRING=$PWD -DPROJECT_DIR:STRING=$PROJECT_DIR -DCMAKE_C_COMPILER_LAUNCHER="distcc" -DCMAKE_CXX_COMPILER_LAUNCHER="distcc"
    make -j12
else
    cmake ../../ProjectUtils/SDK/ -DSDK_DIR:STRING=$PWD -DPROJECT_DIR:STRING=$PROJECT_DIR
    make -j`nproc`
fi

