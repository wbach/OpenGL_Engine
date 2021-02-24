#!/bin/bash
data="Data/"
newData="RequiredData/"
while IFS='' read -r line || [[ -n "$line" ]]; 
do
	filename="../Data/"$line
	outputfile=$newData$line;
	DIR=$(dirname "${outputfile}")
	echo "${filename::(-1)}"
    #echo "$DIR/"	
	mkdir -p $DIR/ && cp ${filename::(-1)} ${outputfile::(-1)}
done < "$1"