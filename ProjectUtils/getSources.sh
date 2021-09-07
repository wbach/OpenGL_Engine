#!/bin/bash
#example usage important is where u run the scrpit example for root/..../CMake: ./getSources.sh "../GameEngine/" GameEngineIncludes "h" "hpp"
LOCATION="$1"
NAME="$2"
OUTNAME=$NAME".cmake"
echo "Generate : "$OUTNAME
touch $OUTNAME
echo "set("$NAME >> $OUTNAME
i=0
  #  echo '${CMAKE_CURRENT_SOURCE_DIR}/../../Tools/' >> $OUTNAME
for var in "$@"
do
    i=$((i+1))
    if [[ $i < 3 ]]; then
      continue
    fi

    #find $LOCATION -iname *."$var" >> $OUTNAME
    array=()
    mapfile -d $'\0' array < <(find $LOCATION -iname *."$var")
    for f in $array
    do
        echo '${CMAKE_CURRENT_SOURCE_DIR}/'$f >> $OUTNAME
    done
done

echo ")" >> $OUTNAME








