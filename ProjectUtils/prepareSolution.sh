#!/bin/bash

projectGlobalId='{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}'
output='Microsoft Visual Studio Solution File, Format Version 12.00
# Visual Studio 15
VisualStudioVersion = 17.9.34607.119
MinimumVisualStudioVersion = 10.0.40219.1'

for arg in "$@"
do
	IFS=': ' read -r -a array <<< "$arg"

	d=0
	projectName=''
	projectId=''
	projectDepend=()
	for ar in "${array[@]}"
	do
		d=$((d+1))
		case "$d" in
		  "1") projectName=$ar;;
		  "2") projectId=$ar;;
		  *) projectDepend+=($ar);;
		esac

	done

	if [ -z "$projectName" ]; then
		continue
	fi

	output=$output$'\n''Project("'$projectGlobalId'") = "'$projectName'", "../../Sources/'$projectName'/'$projectName'.vcxproj", "'$projectId'"'

	if [ "${#projectDepend[@]}" -gt 0 ]; then
		output=$output$'\n\t''ProjectSection(ProjectDependencies) = postProject'
	fi

	for dep in "${projectDepend[@]}"
	do
		output=$output$'\n\t\t'$dep' = '$dep
	done

	if [ "${#projectDepend[@]}" -gt 0 ]; then
		output=$output$'\n\t''EndProjectSection'
	fi

	output=$output$'\n''EndProject'
done
	output=$output'
Global
	GlobalSection(SolutionConfigurationPlatforms) = preSolution
		Debug|x64 = Debug|x64
		Debug|x86 = Debug|x86
		Release|x64 = Release|x64
		Release|x86 = Release|x86
	EndGlobalSection'
output=$output'
	GlobalSection(SolutionProperties) = preSolution
		HideSolutionNode = FALSE
	EndGlobalSection
	GlobalSection(ExtensibilityGlobals) = postSolution
		SolutionGuid = {B385937C-CA50-4CB7-9FC4-DA6E5384F017}
	EndGlobalSection
EndGlobal'
echo "$output"

