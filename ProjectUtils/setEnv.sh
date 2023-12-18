SCRIPTDIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
PARENTDIR="$(dirname "$SCRIPTDIR")"

export USERROOT=$PARENTDIR"/.sdk"
export PATH=$PATH:"$USERROOT/usr/bin"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"$USERROOT/lib":"$USERROOT/lib64":"$USERROOT/usr/lib/x86_64-linux-gnu"
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:"$USERROOT/usr/share/pkgconfig/":"$USERROOT/usr/lib/pkgconfig/":"$USERROOT/usr/lib64/pkgconfig/":"$USERROOT/usr/lib/x86_64-linux-gnu/pkgconfig/"
export CPLUS_INCLUDE_PATH="$USERROOT/usr/include/c++/12.2.0/"

alias pacman_="sudo pacman -r $USERROOT --gpgdir $USERROOT/etc/pacman.d/gnupg"

echo "Set user root : "$USERROOT

