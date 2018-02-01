#!/bin/bash
#example usage important is where u run the scrpit example for root/..../CMake: ./getSources.sh "../GameEngine/" GameEngineIncludes "h" "hpp"
LOCATION="$1"
NAME="$2"
OUTNAME=$NAME".cmake"
echo "Generate : "$OUTNAME
touch $OUTNAME
echo "set("$NAME >> $OUTNAME
i=0
for var in "$@"
do
	i=$((i+1))
	if [[ $i < 3 ]]; then
	  continue
	fi
	find $LOCATION -iname *."$var" >> $OUTNAME

done

echo ")" >> $OUTNAME








