#!/bin/bash

#$1 - Project name
# ./prepareProject.sh GameEngine ProjectGuid outputType depProject... > gameEngine.xml
# outputType :
#   StaticLibrary
#   Application

includeFilePath="../Solutions/CMake/Includes/"$1"Includes.cmake"
sourceFilePath="../Solutions/CMake/Sources/"$1"Sources.cmake"

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
done < "$sourceFilePath"

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
done < "$includeFilePath"

sdkVersion="10.0.16299.0"
#sdkVersion="10.0.17763.0"
toolkit="v141"
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
toolsDir='..\..\Tools\Windows'

bulletReleaseLibs='
Bullet2FileLoader.lib;
BulletInverseDynamicsUtils.lib;
Bullet3Collision.lib;
BulletRobotics.lib;
Bullet3Common.lib;
BulletSoftBody.lib;
Bullet3Dynamics.lib;
BulletWorldImporter.lib;
Bullet3Geometry.lib;
BulletXmlWorldImporter.lib;
Bullet3OpenCL_clew.lib;
ConvexDecomposition.lib;
BulletCollision.lib;
GIMPACTUtils.lib;
BulletDynamics.lib;
HACD.lib;
BulletFileLoader.lib;
LinearMath.lib;
BulletInverseDynamics.lib;
'
bulletDebugLibs='
Bullet2FileLoader_Debug.lib;
BulletInverseDynamicsUtils_Debug.lib;
Bullet3Collision_Debug.lib;
BulletRobotics_Debug.lib;
Bullet3Common_Debug.lib;
BulletSoftBody_Debug.lib;
Bullet3Dynamics_Debug.lib;
BulletWorldImporter_Debug.lib;
Bullet3Geometry_Debug.lib;
BulletXmlWorldImporter_Debug.lib;
Bullet3OpenCL_clew_Debug.lib;
ConvexDecomposition_Debug.lib;
BulletCollision_Debug.lib;
GIMPACTUtils_Debug.lib;
BulletDynamics_Debug.lib;
HACD_Debug.lib;
BulletFileLoader_Debug.lib;
LinearMath_Debug.lib;
BulletInverseDynamics_Debug.lib;
'

additionalIncludesDir='
..\..\Sources\OpenGLApi;
..\..\Sources\GraphicsApi;
..\..\Sources\Utils;
..\..\Sources;
..\..\Tools\common\rapidxml-1.13;
..\..\Tools\Windows\fbx_sdk\include;
..\..\Tools\Windows\assimp-5.0.1\include;
..\..\Tools\Windows\SDL2-2.0.12\include\;
..\..\Tools\Windows\SDL2-2.0.12\include\SDL2\;
..\..\Tools\Windows\SDL2_net-2.0.1\include\;
..\..\Tools\Windows\SDL2_ttf-2.0.15\include;
..\..\Tools\Windows\SDL2_image-2.0.4\include\;
..\..\Tools\common\glm\;
..\..\Tools\Windows;
..\..\Tools\Windows\glew-2.1.0\include;
..\..\Tools\Windows\freeImage;
..\..\Tools\Windows\freetype\include;
..\..\Tools\Windows\GLFW3\include;
..\..\Tools\Windows\gtest\include;
..\..\Tools\Windows\gmock\include;
../../Tools/common/bullet/src/;
../../Tools/Windows/Directx/Include/;
'
for d in "${depend[@]}"
do
  additionalIncludesDir=$additionalIncludesDir'..\..\Sources\'$d'\;'
done

additionalDebugLibs='
libfbxsdk-md.lib;
libxml2-md.lib;
zlib-md.lib;
FreeImage.lib;
SDL2.lib;
glew32.lib;
glu32.lib;
SDL2main.lib;
SDL2test.lib;
SDL2_net.lib;
SDL2_image.lib;
SDL2_ttf.lib;
opengl32.lib;
assimp.lib;
freetype.lib;
glfw3.lib;
glfw3dll.lib;
gtest.lib;
gmock.lib;
'$bulletDebugLibs'
d3d10.lib;
d3d10_1.lib;
d3d11.lib;
d3d9.lib;
d3dcompiler.lib;
D3DCSX.lib;
D3DCSXd.lib;
d3dx10.lib;
d3dx10d.lib;
d3dx11.lib;
d3dx11d.lib;
d3dx9.lib;
d3dx9d.lib;
d3dxof.lib;
dinput8.lib;
dsound.lib;
dwrite.lib;
DxErr.lib;
dxgi.lib;
dxguid.lib;
X3DAudio.lib;
xapobase.lib;
xapobased.lib;
XAPOFX.lib;
XInput.lib;
'

additionalReleaseLibs='
libfbxsdk-md.lib;
libxml2-md.lib;
zlib-md.lib;
FreeImage.lib;
SDL2.lib;
glew32.lib;
glu32.lib;
SDL2main.lib;
SDL2test.lib;
SDL2_net.lib;
SDL2_image.lib;
SDL2_ttf.lib;
opengl32.lib;
assimp.lib;
freetype.lib;
glfw3.lib;
glfw3dll.lib;
gtest.lib;
gmock.lib;
'$bulletReleaseLibs'
d2d1.lib;
d3d10.lib;
d3d10_1.lib;
d3d11.lib;
d3d9.lib;
d3dcompiler.lib;
D3DCSX.lib;
D3DCSXd.lib;
d3dx10.lib;
d3dx10d.lib;
d3dx11.lib;
d3dx11d.lib;
d3dx9.lib;
d3dx9d.lib;
d3dxof.lib;
dinput8.lib;
dsound.lib;
dwrite.lib;
DxErr.lib;
dxgi.lib;
dxguid.lib;
X3DAudio.lib;
xapobase.lib;
xapobased.lib;
XAPOFX.lib;
XInput.lib;
'
for d in "${depend[@]}"
do
  additionalDebugLibs=$additionalDebugLibs$d'.lib;'
done

for d in "${depend[@]}"
do
  additionalReleaseLibs=$additionalReleaseLibs$d'.lib;'
done

additionalDebug32LibsDir='
'$toolsDir'\fbx_sdk\lib\vs2017\x86\debug;
'$toolsDir'\assimp-5.0.1\lib\x86\release;
'$toolsDir'\GL;
'$toolsDir'\glew-2.1.0\lib\Release\Win32;
'$toolsDir'\SDL2_ttf-2.0.15\lib\x86;
'$toolsDir'\SDL2-2.0.12\lib\x86;
'$toolsDir'\SDL2_net-2.0.1\lib\x86;
'$toolsDir'\SDL2_image-2.0.4\lib\x86;
'$toolsDir'\freeImage\x86;
'$toolsDir'\freetype\win32;
'$toolsDir'\GLFW3\lib-vc2015;
'$toolsDir'\gtest\lib\x86\Release;
'$toolsDir'\gmock\lib\x86\Release;
'$toolsDir'\Directx\Lib\x86;
'$toolsDir'\bullet\x86\Debug;
'
additionalRelease32LibsDir='
'$toolsDir'\fbx_sdk\lib\vs2017\x86\release;
'$toolsDir'\assimp-5.0.1\lib\x86\release;
'$toolsDir'\GL;
'$toolsDir'\glew-2.1.0\lib\Release\Win32;
'$toolsDir'\SDL2_ttf-2.0.15\lib\x86;
'$toolsDir'\SDL2-2.0.12\lib\x86;
'$toolsDir'\SDL2_net-2.0.1\lib\x86;
'$toolsDir'\SDL2_image-2.0.4\lib\x86;
'$toolsDir'\freeImage\x86;
'$toolsDir'\freetype\win32;
'$toolsDir'\GLFW3\lib-vc2015;
'$toolsDir'\gtest\lib\x86\Release;
'$toolsDir'\gmock\lib\x86\Release;
'$toolsDir'\Directx\Lib\x86;
'$toolsDir'\bullet\x86\Release;
'

additionalDebug64LibsDir='
'$toolsDir'\fbx_sdk\lib\vs2017\x64\debug;
'$toolsDir'\assimp-5.0.1\lib\x64\release;
'$toolsDir'\GL;
'$toolsDir'\glew-2.1.0\lib\Release\x64;
'$toolsDir'\SDL2_ttf-2.0.15\lib\x64;
'$toolsDir'\SDL2-2.0.12\lib\x64;
'$toolsDir'\SDL2_net-2.0.1\lib\x64;
'$toolsDir'\SDL2_image-2.0.4\lib\x64;
'$toolsDir'\freeImage\x64;
'$toolsDir'\freetype\win64;
'$toolsDir'\GLFW3\lib-vc2015;
'$toolsDir'\gtest\lib\x64\Release;
'$toolsDir'\gmock\lib\x64\Release;
'$toolsDir'\Directx\Lib\x64;
'$toolsDir'\bullet\x64\Debug;
'
additionalRelease64LibsDir='
'$toolsDir'\fbx_sdk\lib\vs2017\x64\release;
'$toolsDir'\assimp-5.0.1\lib\x64\release;
'$toolsDir'\GL;
'$toolsDir'\glew-2.1.0\lib\Release\x64;
'$toolsDir'\SDL2_ttf-2.0.15\lib\x64;
'$toolsDir'\SDL2-2.0.12\lib\x64;
'$toolsDir'\SDL2_net-2.0.1\lib\x64;
'$toolsDir'\SDL2_image-2.0.4\lib\x64;
'$toolsDir'\freeImage\x64;
'$toolsDir'\freetype\win64;
'$toolsDir'\GLFW3\lib-vc2015;
'$toolsDir'\gtest\lib\x64\Release;
'$toolsDir'\gmock\lib\x64\Release;
'$toolsDir'\Directx\Lib\x64;
'$toolsDir'\bullet\x64\Release;
'

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
  <ItemDefinitionGroup Condition='$configurationPlatformStringD32'>
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <ConformanceMode>false</ConformanceMode>
      <MinimalRebuild>false</MinimalRebuild>
      <Optimization>Disabled</Optimization>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <ObjectFileName>$(IntDir)%(RelativeDir)</ObjectFileName>
      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>
      <MultiProcessorCompilation>true</MultiProcessorCompilation>
      <AdditionalIncludeDirectories>'$additionalIncludesDir'%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <PreprocessorDefinitions>_CRT_SECURE_NO_WARNINGS;_MBCS;_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING;_DEBUG;%(PreprocessorDefinitions)</PreprocessorDefinitions>
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
      <SDLCheck>true</SDLCheck>
      <ConformanceMode>false</ConformanceMode>
        <MinimalRebuild>false</MinimalRebuild>
      <Optimization>Disabled</Optimization>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <ObjectFileName>$(IntDir)%(RelativeDir)</ObjectFileName>
      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>
      <MultiProcessorCompilation>true</MultiProcessorCompilation>
      <AdditionalIncludeDirectories>'$additionalIncludesDir'%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <PreprocessorDefinitions>_CRT_SECURE_NO_WARNINGS;_MBCS;_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING;_DEBUG;%(PreprocessorDefinitions)</PreprocessorDefinitions>
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
      <SDLCheck>false</SDLCheck>
      <AdditionalIncludeDirectories>'$additionalIncludesDir'%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>
      <MultiProcessorCompilation>true</MultiProcessorCompilation>
      <MinimalRebuild>false</MinimalRebuild>
      <PreprocessorDefinitions>_CRT_SECURE_NO_WARNINGS;_MBCS;_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <BufferSecurityCheck>false</BufferSecurityCheck>
      <ObjectFileName>$(IntDir)%(RelativeDir)</ObjectFileName>
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
      <SDLCheck>true</SDLCheck>
      <ConformanceMode>false</ConformanceMode>
      <PreprocessorDefinitions>_CRT_SECURE_NO_WARNINGS;_MBCS;_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalIncludeDirectories>'$additionalIncludesDir'%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <MultiProcessorCompilation>true</MultiProcessorCompilation>
      <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>
      <Optimization>MaxSpeed</Optimization>
      <MinimalRebuild>false</MinimalRebuild>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <ObjectFileName>$(IntDir)%(RelativeDir)</ObjectFileName>
      <BufferSecurityCheck>false</BufferSecurityCheck>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <AdditionalDependencies>'$additionalReleaseLibs'%(AdditionalDependencies)</AdditionalDependencies>
      <AdditionalLibraryDirectories>'$additionalRelease64LibsDir'%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
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


