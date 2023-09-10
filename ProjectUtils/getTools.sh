#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters, please put full tools path"
    exit 2
fi



SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

source windowsDepedencies.sh;

mkdir -p $toolsDir;

sourceToolsDir=$1;
targetDir=$SCRIPT_DIR'/'$toolsDir;
echo "Target destination : "$targetDir;
echo "Source destination : "$sourceToolsDir;

function copy()
{
    local skip=$1
    for i in "${@:$skip}"
    do
      SUB='Tools'
      if [[ "$i" == *"$SUB"* ]]; then
          x=`echo "$i" | tr -d ';'`
          x=`echo "$x" | grep -oP "^$toolsDir\K.*"`
          echo $targetDir"$x"
          mkdir -p $targetDir"$x"
          cp -r $sourceToolsDir"$x" $targetDir"$x"
      fi
    done
}

copy 4 $additionalIncludesDir
copy 2 $additionalDebug32LibsDir
copy 2 $additionalRelease32LibsDir
copy 2 $additionalDebug64LibsDir
copy 2 $additionalRelease64LibsDir


