#!/bin/bash
LOCATION="$1"
NAME="$2"
OUTNAME=$NAME".cmake"
rm $OUTNAME
touch $OUTNAME
echo "set("$NAME >> $OUTNAME
find $LOCATION -iname *.cpp >> $OUTNAME
echo ")" >> $OUTNAME



