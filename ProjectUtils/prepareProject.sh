#!/bin/bash

#$1 - Project name
# ./prepareProject.sh GameEngine ProjectGuid outputType depProject... > gameEngine.xml
# outputType :
#   StaticLibrary
#   Application
#SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )


includeFilePath="../Solutions/CMake/Includes/"$1"Includes.cmake"
sourceFilePath="../Solutions/CMake/Sources/"$1"Sources.cmake"

sources=()
while IFS= read -r line || [[ -n "$line" ]]; do
    if [[ $line == "set("* ]]; then
        continue
    fi
    if [[ $line == *\)* ]]; then
        continue
    fi

    # Usuwamy prefix ścieżki CMake
    prefix='${CMAKE_CURRENT_SOURCE_DIR}/'
    filteredFile=${line#"$prefix"}

    # Usuwamy końcowe znaki CR/LF
    filteredFile=$(echo "$filteredFile" | tr -d '\r\n')

    # Dodajemy do tablicy z cudzysłowami, żeby spacje i dziwne znaki nie powodowały problemów
    sources+=("$filteredFile")
done < "$sourceFilePath"

headers=()
while IFS= read -r line || [[ -n "$line" ]]; do
    if [[ $line == "set("* ]]; then
        continue
    fi
    if [[ $line == *\)* ]]; then
        continue
    fi

    # Usuwamy prefix ścieżki CMake
    prefix='${CMAKE_CURRENT_SOURCE_DIR}/'
    filteredFile=${line#"$prefix"}

    # Usuwamy końcowe znaki CR/LF
    filteredFile=$(echo "$filteredFile" | tr -d '\r\n')

    # Dodajemy do tablicy z cudzysłowami, aby zachować spacje w nazwach
    headers+=("$filteredFile")
done < "$includeFilePath"

sdkVersion="10.0"
toolkit="v143"
projectName=$1
projectNameFile=$projectName".vcxproj";
ProjectGuid=$2
OutputType=$3

inputArg=0
depend=()

for dep in "${@:4}"
do
  depend+=($dep)
done

#"{1AB884C5-B769-46D2-BDD0-8CEF3AD7AEB2}"
source windowsDepedencies.sh

#additionalIncludesDir=$additionalIncludesDir'$toolsDir'/Windows;

for d in "${depend[@]}"
do
  additionalIncludesDir=$additionalIncludesDir'..\..\Sources\'$d'\;'
done

for d in "${depend[@]}"
do
  additionalDebugLibs=$additionalDebugLibs$d'.lib;'
done

for d in "${depend[@]}"
do
  additionalReleaseLibs=$additionalReleaseLibs$d'.lib;'
done

for d in "${depend[@]}"
do
  additionalDebug32LibsDir=$additionalDebug32LibsDir'..\..\build\bin\'$d'\$(Configuration)\Win32\;'
done

for d in "${depend[@]}"
do
  additionalRelease32LibsDir=$additionalRelease32LibsDir'..\..\build\bin\'$d'\$(Configuration)\Win32\;'
done

for d in "${depend[@]}"
do
  additionalDebug64LibsDir=$additionalDebug64LibsDir'..\..\build\bin\'$d'\$(Configuration)\x64\;'
done

for d in "${depend[@]}"
do
  additionalRelease64LibsDir=$additionalRelease64LibsDir'..\..\build\bin\'$d'\$(Configuration)\x64\;'
done

outDir='$(SolutionDir)..\..\build\bin\'$projectName'\$(Configuration)\$(Platform)\'
inDir='$(SolutionDir)..\..\build\Intermediate\'$projectName'\$(Configuration)\$(Platform)\'$projectName'\'$projectName'\'

configurationPlatformStringD32=$'"\'$(Configuration)|$(Platform)\'==\'Debug|Win32\'"'
configurationPlatformStringR32=$'"\'$(Configuration)|$(Platform)\'==\'Release|Win32\'"'
configurationPlatformStringD64=$'"\'$(Configuration)|$(Platform)\'==\'Debug|x64\'"'
configurationPlatformStringR64=$'"\'$(Configuration)|$(Platform)\'==\'Release|x64\'"'
configurationPlatformStringFR64=$'"\'$(Configuration)|$(Platform)\'==\'FinalRelease|x64\'"'

echo '<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|Win32">
      <Configuration>Debug</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|Win32">
      <Configuration>Release</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
	  <ProjectConfiguration Include="FinalRelease|x64">
      <Configuration>FinalRelease</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>'$ProjectGuid'</ProjectGuid>
    <RootNamespace>'$projectName'</RootNamespace>
    <WindowsTargetPlatformVersion>'$sdkVersion'</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Condition='$configurationPlatformStringD32' Label="Configuration">
    <ConfigurationType>'$OutputType'</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>'$toolkit'</PlatformToolset>
    <CharacterSet>MultiByte</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition='$configurationPlatformStringR32' Label="Configuration">
    <ConfigurationType>'$OutputType'</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>'$toolkit'</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>MultiByte</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition='$configurationPlatformStringD64' Label="Configuration">
    <ConfigurationType>'$OutputType'</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>'$toolkit'</PlatformToolset>
    <CharacterSet>MultiByte</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition='$configurationPlatformStringR64' Label="Configuration">
    <ConfigurationType>'$OutputType'</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>'$toolkit'</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>MultiByte</CharacterSet>
  </PropertyGroup>
    <PropertyGroup Condition='$configurationPlatformStringFR64' Label="Configuration">
    <ConfigurationType>'$OutputType'</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>'$toolkit'</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>MultiByte</CharacterSet>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
  <ImportGroup Label="Shared">
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition='$configurationPlatformStringD32'>
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition='$'"exists(\'$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props\')"'' Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition='$configurationPlatformStringR32'>
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition='$'"exists(\'$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props\')"'' Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition='$configurationPlatformStringD64'>
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition='$'"exists(\'$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props\')"'' Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition='$configurationPlatformStringR64'>
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition='$'"exists(\'$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props\')"'' Label="LocalAppDataPlatform" />
  </ImportGroup>
    <ImportGroup Label="PropertySheets" Condition='$configurationPlatformStringFR64'>
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition='$'"exists(\'$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props\')"'' Label="LocalAppDataPlatform" />
  </ImportGroup>
  <PropertyGroup Label="UserMacros" />
  <PropertyGroup Condition='$configurationPlatformStringD32'>
    <LinkIncremental>true</LinkIncremental>
    <OutDir>'$outDir'\</OutDir>
    <IntDir>'$inDir'\</IntDir>
  </PropertyGroup>
  <PropertyGroup Condition='$configurationPlatformStringD64'>
    <LinkIncremental>true</LinkIncremental>
    <OutDir>'$outDir'\</OutDir>
    <IntDir>'$inDir'\</IntDir>
  </PropertyGroup>
  <PropertyGroup Condition='$configurationPlatformStringR32'>
    <LinkIncremental>false</LinkIncremental>
    <OutDir>'$outDir'\</OutDir>
    <IntDir>'$inDir'\</IntDir>
  </PropertyGroup>
  <PropertyGroup Condition='$configurationPlatformStringR64'>
    <LinkIncremental>false</LinkIncremental>
    <OutDir>'$outDir'\</OutDir>
    <IntDir>'$inDir'\</IntDir>
  </PropertyGroup>
   <PropertyGroup Condition='$configurationPlatformStringFR64'>
    <LinkIncremental>false</LinkIncremental>
    <OutDir>'$outDir'\</OutDir>
    <IntDir>'$inDir'\</IntDir>
  </PropertyGroup>
  <ItemDefinitionGroup Condition='$configurationPlatformStringD32'>
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <ConformanceMode>false</ConformanceMode>
      <MinimalRebuild>false</MinimalRebuild>
      <Optimization>Disabled</Optimization>
      <LanguageStandard>stdcpp20</LanguageStandard>
      <ObjectFileName>$(IntDir)%(RelativeDir)%(Filename)%(Extension)</ObjectFileName>
      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>
      <MultiProcessorCompilation>true</MultiProcessorCompilation>
      <AdditionalIncludeDirectories>'$additionalIncludesDir'%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <PreprocessorDefinitions>_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING; _CRT_SECURE_NO_WARNINGS;_MBCS;_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING;_DEBUG;WXUSINGDLL;_UNICODE;UNICODE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalOptions>/Zc:__cplusplus /bigobj %(AdditionalOptions)</AdditionalOptions>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <AdditionalDependencies>'$additionalDebugLibs'%(AdditionalDependencies)</AdditionalDependencies>
      <AdditionalLibraryDirectories>'$additionalDebug32LibsDir'%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition='$configurationPlatformStringD64'>
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <SDLCheck>false</SDLCheck>
      <ConformanceMode>false</ConformanceMode>
      <MinimalRebuild>false</MinimalRebuild>
      <Optimization>Disabled</Optimization>
      <LanguageStandard>stdcpp20</LanguageStandard>
      <ObjectFileName>$(IntDir)%(RelativeDir)%(Filename)%(Extension)</ObjectFileName>
      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>
      <MultiProcessorCompilation>true</MultiProcessorCompilation>
      <AdditionalIncludeDirectories>'$debug64IncludesDir'%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <PreprocessorDefinitions>_DEBUG;_CRT_SECURE_NO_WARNINGS;_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING;_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING;_MBCS;WXUSINGDLL;_UNICODE;UNICODE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>
      <BufferSecurityCheck>true</BufferSecurityCheck>
      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
      <AdditionalOptions>/Zc:__cplusplus /bigobj %(AdditionalOptions)</AdditionalOptions>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <AdditionalDependencies>'$additionalDebugLibs'%(AdditionalDependencies)</AdditionalDependencies>
      <AdditionalLibraryDirectories>'$additionalDebug64LibsDir'%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition='$configurationPlatformStringR32'>
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
      <PreprocessorDefinitions>_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING; _CRT_SECURE_NO_WARNINGS;_MBCS;_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING;WXUSINGDLL;_UNICODE;UNICODE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <LanguageStandard>stdcpp20</LanguageStandard>
      <BufferSecurityCheck>true</BufferSecurityCheck>
      <ObjectFileName>$(IntDir)%(RelativeDir)%(Filename)%(Extension)</ObjectFileName>
      <AdditionalOptions>/Zc:__cplusplus /bigobj %(AdditionalOptions)</AdditionalOptions>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <AdditionalDependencies>'$additionalReleaseLibs'%(AdditionalDependencies)</AdditionalDependencies>
      <AdditionalLibraryDirectories>'$additionalRelease32LibsDir'%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition='$configurationPlatformStringR64'>
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>false</SDLCheck>
      <ConformanceMode>false</ConformanceMode>
      <PreprocessorDefinitions>_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING;_CRT_SECURE_NO_WARNINGS;_MBCS;_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING;NDEBUG;WXUSINGDLL;_UNICODE;UNICODE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalIncludeDirectories>'$release64IncludesDir'%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <MultiProcessorCompilation>true</MultiProcessorCompilation>
      <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>
      <Optimization>MaxSpeed</Optimization>
      <InlineFunctionExpansion>AnySuitable</InlineFunctionExpansion>
      <FavorSizeOrSpeed>Speed</FavorSizeOrSpeed>
      <OmitFramePointers>true</OmitFramePointers>
      <MinimalRebuild>false</MinimalRebuild>
      <LanguageStandard>stdcpp20</LanguageStandard>
      <ObjectFileName>$(IntDir)%(RelativeDir)%(Filename)%(Extension)</ObjectFileName>
      <BufferSecurityCheck>false</BufferSecurityCheck>
      <AdditionalOptions>/Zc:__cplusplus /bigobj /Gw /GL %(AdditionalOptions)</AdditionalOptions>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
      <GenerateDebugInformation>false</GenerateDebugInformation>
      <LinkTimeCodeGeneration>UseLinkTimeCodeGeneration</LinkTimeCodeGeneration>
      <AdditionalDependencies>'$additionalReleaseLibs'%(AdditionalDependencies)</AdditionalDependencies>
      <AdditionalLibraryDirectories>'$additionalRelease64LibsDir'%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
    </Link>
  </ItemDefinitionGroup>
	<ItemDefinitionGroup Condition='$configurationPlatformStringFR64'>
    <ClCompile>
      <WarningLevel>Level4</WarningLevel>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>true</SDLCheck>
      <ConformanceMode>true</ConformanceMode>
      <PreprocessorDefinitions>FINAL_RELEASE;_CRT_SECURE_NO_WARNINGS;_MBCS;_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING;NDEBUG;WXUSINGDLL;_UNICODE;UNICODE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalIncludeDirectories>'$release64IncludesDir'%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <MultiProcessorCompilation>true</MultiProcessorCompilation>
      <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>
      <Optimization>Full</Optimization>
      <InlineFunctionExpansion>AnySuitable</InlineFunctionExpansion>
      <FavorSizeOrSpeed>Speed</FavorSizeOrSpeed>
      <OmitFramePointers>false</OmitFramePointers>
      <MinimalRebuild>false</MinimalRebuild>
      <LanguageStandard>stdcpp20</LanguageStandard>
      <ObjectFileName>$(IntDir)%(RelativeDir)%(Filename)%(Extension)</ObjectFileName>
      <BufferSecurityCheck>true</BufferSecurityCheck>
      <AdditionalOptions>/Zc:__cplusplus /bigobj /GL /GS /guard:cf %(AdditionalOptions)</AdditionalOptions>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
      <GenerateDebugInformation>false</GenerateDebugInformation>
      <LinkTimeCodeGeneration>UseLinkTimeCodeGeneration</LinkTimeCodeGeneration>
      <AdditionalDependencies>'$additionalReleaseLibs'%(AdditionalDependencies)</AdditionalDependencies>
      <AdditionalLibraryDirectories>'$additionalRelease64LibsDir'%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
    </Link>
  </ItemDefinitionGroup>
  <ItemGroup>'
  for cpp in "${sources[@]}"
  do
    echo $'    <ClCompile Include="'"$vs_s"$cpp'" />'
  done
  echo '  </ItemGroup>'
  echo '  <ItemGroup>'

  for inc in "${headers[@]}"
  do
    echo $'    <ClInclude Include="'"$vs_s"$inc'" />'
  done

  echo '  </ItemGroup>'
  echo '  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" /><ImportGroup Label="ExtensionTargets"></ImportGroup></Project>'

  #\"\'\$\(Configuration\)\|\$\(Platform\)\'==\'Debug\|Win32\'\"


