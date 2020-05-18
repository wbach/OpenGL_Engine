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
projectName=$1
projectNameFile=$projectName".vcxproj";

configurationPlatformString=$'"\'$(Configuration)|$(Platform)\' '
configuration_1=$'== \'Debug|Win32\'"'
configuration_2=$'== \'Release|Win32\'"'
configuration_1=$configurationPlatformString$configuration_1
configuration_2=$configurationPlatformString$configuration_2
rootConditions=$'"exists(\'$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props\')"'
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

additionalIncludesDir='
..\..\Sources\OpenGLApi;
..\..\Sources\GraphicsApi;
..\..\Sources\Utils;
..\..\Sources;
..\..\Tools\common\rapidxml-1.13;
..\..\Tools\Windows\fbx_sdk\include;
..\..\Tools\Windows\Assimp\include;
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
libfreetype.lib;
glfw3.lib;
glfw3dll.lib;
gtest.lib;
gmock.lib;
Bullet2FileLoader_Debug.lib;
Bullet3Collision_Debug.lib;
Bullet3Common_Debug.lib;
Bullet3Dynamics_Debug.lib;
Bullet3Geometry_Debug.lib;
Bullet3OpenCL_clew_Debug.lib;
BulletCollision_Debug.lib;
BulletDynamics_Debug.lib;
BulletExampleBrowserLib_Debug.lib;
BulletFileLoader_Debug.lib;
BulletInverseDynamics_Debug.lib;
BulletInverseDynamicsUtils_Debug.lib;
BulletRobotics_Debug.lib;
BulletSoftBody_Debug.lib;
BulletWorldImporter_Debug.lib;
BulletXmlWorldImporter_Debug.lib;
BussIK_Debug.lib;
clsocket_Debug.lib;
ConvexDecomposition_Debug.lib;
GIMPACTUtils_Debug.lib;
gtest_Debug.lib;
gwen_Debug.lib;
HACD_Debug.lib;
LinearMath_Debug.lib;
OpenGLWindow_Debug.lib;
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
dsetup.lib;
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
libfreetype.lib;
glfw3.lib;
glfw3dll.lib;
gtest.lib;
gmock.lib;
Bullet2FileLoader.lib;
Bullet3Collision.lib;
Bullet3Common.lib;
Bullet3Dynamics.lib;
Bullet3Geometry.lib;
Bullet3OpenCL_clew.lib;
BulletCollision.lib;
BulletDynamics.lib;
BulletExampleBrowserLib.lib;
BulletFileLoader.lib;
BulletInverseDynamics.lib;
BulletInverseDynamicsUtils.lib;
BulletRobotics.lib;
BulletSoftBody.lib;
BulletWorldImporter.lib;
BulletXmlWorldImporter.lib;
BussIK.lib;
clsocket.lib;
ConvexDecomposition.lib;
GIMPACTUtils.lib;
gtest.lib;
gwen.lib;
HACD.lib;
LinearMath.lib;
OpenGLWindow.lib;
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
dsetup.lib;
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

additionalDebugLibsDir='
'$toolsDir'\fbx_sdk\lib\vs2017\x86\debug;
'$toolsDir'\Assimp\lib\x86;
'$toolsDir'\GL;
'$toolsDir'\glew-2.1.0\lib\Release\Win32;
'$toolsDir'\SDL2_ttf-2.0.15\lib\x86;
'$toolsDir'\SDL2-2.0.12\lib\x86;
'$toolsDir'\SDL2_net-2.0.1\lib\x86;
'$toolsDir'\SDL2_image-2.0.4\lib\x86;
'$toolsDir'\freeImage;
'$toolsDir'\freetype\lib;
'$toolsDir'\GLFW3\lib-vc2015;
'$toolsDir'\gtest\lib\x86\Release;
'$toolsDir'\gmock\lib\x86\Release;
'$toolsDir'\Directx\Lib\x86;
..\..\Tools\common\bullet\lib\Debug;
'
additionalReleaseLibsDir='
'$toolsDir'\fbx_sdk\lib\vs2017\x86\release;
'$toolsDir'\Assimp\lib\x86;
'$toolsDir'\GL;
'$toolsDir'\glew-2.1.0\lib\Release\Win32;
'$toolsDir'\SDL2_ttf-2.0.15\lib\x86;
'$toolsDir'\SDL2-2.0.12\lib\x86;
'$toolsDir'\SDL2_net-2.0.1\lib\x86;
'$toolsDir'\SDL2_image-2.0.4\lib\x86;
'$toolsDir'\freeImage;
'$toolsDir'\freetype\lib;
'$toolsDir'\GLFW3\lib-vc2015;
'$toolsDir'\gtest\lib\x86\Release;
'$toolsDir'\gmock\lib\x86\Release;
'$toolsDir'\Directx\Lib\x86;
..\..\Tools\common\bullet\lib\Release;
'

for d in "${depend[@]}"
do
  additionalDebugLibsDir=$additionalDebugLibsDir'..\..\Build\bin\'$d'\$(Configuration)\;'
done

for d in "${depend[@]}"
do
  additionalReleaseLibsDir=$additionalReleaseLibsDir'..\..\Build\bin\'$d'\$(Configuration)\;'
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
      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>
      <MultiProcessorCompilation>true</MultiProcessorCompilation>
      <MinimalRebuild>false</MinimalRebuild>
      <PreprocessorDefinitions>_CRT_SECURE_NO_WARNINGS;_MBCS;_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING;_DEBUG;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <BufferSecurityCheck>true</BufferSecurityCheck>
      <ObjectFileName>$(IntDir)%(RelativeDir)</ObjectFileName>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <AdditionalDependencies>'$additionalDebugLibs'%(AdditionalDependencies)</AdditionalDependencies>
      <AdditionalLibraryDirectories>'$additionalDebugLibsDir'%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
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
      <PreprocessorDefinitions>_CRT_SECURE_NO_WARNINGS;_MBCS;_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <BufferSecurityCheck>false</BufferSecurityCheck>
      <ObjectFileName>$(IntDir)%(RelativeDir)</ObjectFileName>
    </ClCompile>
    <Link>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
      <SubSystem>Console</SubSystem>
      <AdditionalDependencies>'$additionalReleaseLibs'%(AdditionalDependencies)</AdditionalDependencies>
      <AdditionalLibraryDirectories>'$additionalReleaseLibsDir'%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
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


