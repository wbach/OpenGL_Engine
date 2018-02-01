#!/bin/bash

#$1 - Project name
# ./prepareProject.sh GameEngine ../CMake/Sources/GameEngineSources.cmake ../CMake/Includes/GameEngineIncludes.cmake ProjectGuid outputType depProject... > gameEngine.xml
# outputType : 
# 	StaticLibrary
# 	Application

sources=()
while IFS='' read -r line || [[ -n "$line" ]]; do
	if [[ $line == *"set"* ]]; then
	  continue
	fi
	if [[ $line == *\)* ]]; then
	  continue
	fi
	#v=${line::-1}
	sources+=($line)
	#($v)
done < "$2"

headers=()
while IFS='' read -r line || [[ -n "$line" ]]; do
	if [[ $line == *"set"* ]]; then
	  continue
	fi
	if [[ $line == *\)* ]]; then
	  continue
	fi
	#v=#${line::-1}
	headers+=($line)
done < "$3"

sdkVersion="10.0.16299.0"
projectName=$1
projectNameFile=$projectName".vcxproj";

configurationPlatformString=$'"\'$(Configuration)|$(Platform)\' '
configuration_1=$'== \'Debug|Win32\'"'
configuration_2=$'== \'Release|Win32\'"'
configuration_1=$configurationPlatformString$configuration_1
configuration_2=$configurationPlatformString$configuration_2
rootConditions=$'"exists(\'$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props\')"'
ProjectGuid=$4
OutputType=$5

inputArg=0
depend=()
for dep in "$@"
do
	inputArg=$((inputArg+1))
	if [[ $inputArg < 6 ]]; then
	  continue
	fi
	
	depend+=($dep)
done

#"{1AB884C5-B769-46D2-BDD0-8CEF3AD7AEB2}"
toolsDir='..\..\Tools\Windows'

additionalIncludesDir='
..\..\Sources\Utils;
..\..\Tools\common\rapidxml-1.13;
..\..\Tools\Windows\fbx_sdk\include;
..\..\Tools\Windows\Assimp\include;
..\..\Tools\Windows\SDL2-2.0.3\include\;
..\..\Tools\Windows\SDL2_net-2.0.1\include\;
..\..\Tools\common\glm\;
..\..\Tools\Windows;
..\..\Tools\Windows\freeImage;
..\..\Tools\Windows\freetype\include;
..\..\Tools\Windows\GLFW3\include;
..\..\Tools\Windows\gtest\include;
..\..\Tools\Windows\gmock\include;
'
for d in "${depend[@]}"
do
	additionalIncludesDir=$additionalIncludesDir'..\..\Sources\'$d'\;'
done

additionalLibs='
libfbxsdk-md.lib;
FreeImage.lib;
SDL2.lib;
glew32.lib;
glu32.lib;
SDL2main.lib;
SDL2test.lib;
SDL2_net.lib;
SDL2_image.lib;
opengl32.lib;
assimp.lib;
libfreetype.lib;
glfw3.lib;
glfw3dll.lib;
gtest.lib;
gmock.lib;'
for d in "${depend[@]}"
do
	additionalLibs=$additionalLibs$d'.lib;'
done

additionalLibsDir='
'$toolsDir'\fbx_sdk\lib\vs2015\x86\release;
'$toolsDir'\Assimp\lib\x86;
'$toolsDir'\GL;
'$toolsDir'\SDL2-2.0.3\lib\x86;
'$toolsDir'\SDL2_net-2.0.1\lib\x86;
'$toolsDir'\freeImage;
'$toolsDir'\freetype\lib;
'$toolsDir'\GLFW3\lib-vc2015;
'$toolsDir'\gtest\lib\x86\Release;
'$toolsDir'\gmock\lib\x86\Release;
'
for d in "${depend[@]}"
do
	additionalLibsDir=$additionalLibsDir'..\..\Build\bin\'$d'\$(Configuration)\;'
done

outDir='$(SolutionDir)..\..\Build\bin\'$projectName'\$(Configuration)\'
inDir='$(SolutionDir)..\..\Build\Intermediate\'$projectName'\$(Configuration)\'

echo '<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|Win32">
      <Configuration>Debug</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|Win32">
      <Configuration>Release</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
  </ItemGroup>
    <PropertyGroup Label="Globals">
    <ProjectGuid>'$ProjectGuid'</ProjectGuid>
    <RootNamespace>Utils</RootNamespace>
    <WindowsTargetPlatformVersion>'$sdkVersion'</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
      <PropertyGroup Condition='$configuration_1' Label="Configuration">
    <ConfigurationType>'$OutputType'</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v141</PlatformToolset>
    <CharacterSet>MultiByte</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition='$configuration_2' Label="Configuration">
    <ConfigurationType>'$OutputType'</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v141</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>MultiByte</CharacterSet>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
  <ImportGroup Label="Shared">
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition='$configuration_1'>
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition='$rootConditions' Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition='$configuration_2'>
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition='$rootConditions' Label="LocalAppDataPlatform" />
  </ImportGroup>
  <PropertyGroup Label="UserMacros" />
   <PropertyGroup Condition='$configuration_1'>
    <OutDir>'$outDir'</OutDir>
    <IntDir>'$inDir'</IntDir>
  </PropertyGroup>
  <PropertyGroup Condition='$configuration_2'>
    <OutDir>'$outDir'</OutDir>
    <IntDir>'$inDir'</IntDir>
  </PropertyGroup>
    <ItemDefinitionGroup Condition='$configuration_1'>
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <Optimization>Disabled</Optimization>
      <SDLCheck>true</SDLCheck>
      <AdditionalIncludeDirectories>'$additionalIncludesDir'%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>
	  <MultiProcessorCompilation>true</MultiProcessorCompilation>
	  <MinimalRebuild>false</MinimalRebuild>
          <PreprocessorDefinitions>_MBCS;_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <AdditionalDependencies>'$additionalLibs'%(AdditionalDependencies)</AdditionalDependencies>
      <AdditionalLibraryDirectories>'$additionalLibsDir'%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition='$configuration_2'>
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <Optimization>MaxSpeed</Optimization>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>true</SDLCheck>
       <AdditionalIncludeDirectories>'$additionalIncludesDir'%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>
	  <MultiProcessorCompilation>true</MultiProcessorCompilation>
	  <MinimalRebuild>false</MinimalRebuild>
          <PreprocessorDefinitions>_MBCS;_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
    <Link>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
      <SubSystem>Console</SubSystem>
      <AdditionalDependencies>'$additionalLibs'%(AdditionalDependencies)</AdditionalDependencies>
      <AdditionalLibraryDirectories>'$additionalLibsDir'%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
  </ItemDefinitionGroup><ItemGroup>
  '
	for cpp in "${sources[@]}"
	do
		echo $'<ClCompile Include="'" $vs_s"$cpp'" />'
	done
	echo '</ItemGroup>'
  echo '<ItemGroup>'
	
	for inc in "${headers[@]}"
	do
		echo $'<ClInclude Include="'" $vs_s"$inc'" />'
	done
	
  echo '</ItemGroup>'
	echo '<Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" /><ImportGroup Label="ExtensionTargets"></ImportGroup></Project>'
	
  #\"\'\$\(Configuration\)\|\$\(Platform\)\'==\'Debug\|Win32\'\"
  

