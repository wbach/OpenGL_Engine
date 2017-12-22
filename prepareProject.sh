#!/bin/bash
#$1 - Project name
sources=()
while IFS='' read -r line || [[ -n "$line" ]]; do
	if [[ $line == *"set"* ]]; then
	  continue
	fi
	if [[ $line == *\)* ]]; then
	  continue
	fi
	v=${line::-1}
	sources+=($v)
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
ProjectGuid="{1AB884C5-B769-46D2-BDD0-8CEF3AD7AEB2}"
additionalIncludesDir='..\Utils;..\Tools\common\rapidxml-1.13;..\Tools\Windows\fbx_sdk\include;..\Tools\Windows\Assimp\include;..\Tools\Windows\SDL2-2.0.3\include\;..\Tools\common\glm\;..\Tools\Windows;..\Tools\Windows\freeImage;..\Tools\Windows\freetype\include;..\Tools\Windows\GLFW3\include;'
additionalLibs='libfbxsdk-md.lib;FreeImage.lib;SDL2.lib;glew32.lib;glu32.lib;SDL2main.lib;SDL2test.lib;SDL2_image.lib;opengl32.lib;assimp.lib;libfreetype.lib;glfw3.lib;glfw3dll.lib;'
additionalLibsDir='..\Tools\fbx_sdk\lib\vs2015\x86\release;..\Tools\Assimp\lib\x86;..\Tools\GL;..\Tools\SDL2-2.0.3\lib\x86;..\Tools\freeImage;..\Tools\freetype\lib;..\Tools\GLFW3\lib-vc2015;'


projectNameFileString='<?xml version="1.0" encoding="utf-8"?>
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
    <ConfigurationType>StaticLibrary</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v141</PlatformToolset>
    <CharacterSet>MultiByte</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition='$configuration_2' Label="Configuration">
    <ConfigurationType>StaticLibrary</ConfigurationType>
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
    <OutDir>$(SolutionDir)\bin\'$projectName'\$(Configuration)\</OutDir>
    <IntDir>$(SolutionDir)\Intermediate\'$projectName'\$(Configuration)\</IntDir>
  </PropertyGroup>
  <PropertyGroup Condition='$configuration_2'>
    <OutDir>$(SolutionDir)\bin\'$projectName'\$(Configuration)\</OutDir>
    <IntDir>$(SolutionDir)\Intermediate\'$projectName'\$(Configuration)\</IntDir>
  </PropertyGroup>
    <ItemDefinitionGroup Condition='$configuration_1'>
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <Optimization>Disabled</Optimization>
      <SDLCheck>true</SDLCheck>
      <AdditionalIncludeDirectories>'$additionalIncludesDir'%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>
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
  #vs_sources=();
	for cpp in "${sources[@]}"
	do
		vs_s=$'<ClCompile Include="'
		#vs_sources+=($vs_s)
		projectNameFileString=$projectNameFileString" $vs_s"$cpp'" />'
	done
	projectNameFileString=$projectNameFileString'</ItemGroup><ItemGroup>'
	
	for inc in "${headers[@]}"
	do
		vs_s=$'<ClInclude Include="'
		projectNameFileString=$projectNameFileString" $vs_s"$inc'" />'
	done
	
	projectNameFileString=$projectNameFileString'</ItemGroup><Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" /><ImportGroup Label="ExtensionTargets"></ImportGroup></Project>'
	
  #\"\'\$\(Configuration\)\|\$\(Platform\)\'==\'Debug\|Win32\'\"
  

echo $projectNameFileString

