#!/bin/bash
data="Data/"
newData="RequiredData/"
while IFS='' read -r line || [[ -n "$line" ]]; 
do
	outputfile=$newData$line;
	DIR=$(dirname "${outputfile}")
	echo "${line::(-1)}"
    #echo "$DIR/"	
	mkdir -p $DIR/ && cp ${line::(-1)} ${outputfile::(-1)}
done < "$1"