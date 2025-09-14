#!/bin/bash

sourcesDir='../../Sources'
toolsDir='../../Tools'
#get tools skip 2 first lines
additionalIncludesDir='
'$sourcesDir'/;
'$toolsDir'/Windows;
'$sourcesDir'/Utils;
'$sourcesDir'/OpenGLApi;
'$sourcesDir'/GraphicsApi;
'$toolsDir'/common/glm;
'$toolsDir'/common/bullet/src/;
'$toolsDir'/common/wxWidgets/include/;
'$toolsDir'/Windows/wxWidgets/x64/Release/mswu;
'$toolsDir'/common/magic_enum-v0.9.7/include;
'$toolsDir'/common/rapidjson-1.1.0/include;
'$toolsDir'/common/rapidxml-1.13;
'$toolsDir'/Windows/assimp-5.0.1/include;
'$toolsDir'/Windows/SDL2-2.0.12/include/;
'$toolsDir'/Windows/SDL2-2.0.12/include/SDL2/;
'$toolsDir'/Windows/SDL2_net-2.0.1/include/;
'$toolsDir'/Windows/SDL2_ttf-2.0.15/include;
'$toolsDir'/Windows/SDL2_image-2.0.4/include/;
'$toolsDir'/Windows/glew-2.1.0/include;
'$toolsDir'/Windows/freeImage;
'$toolsDir'/Windows/freetype/include;
'$toolsDir'/Windows/GLFW3/include;
'$toolsDir'/Windows/gtest/include;
'$toolsDir'/Windows/gmock/include;
'$toolsDir'/Windows/Directx/Include/;
'
additionalDebug32LibsDir='
'$toolsDir'/Windows/fbx_sdk/lib/vs2017/x86/debug;
'$toolsDir'/Windows/assimp-5.0.1/lib/x86/release;
'$toolsDir'/Windows/GL;
'$toolsDir'/Windows/glew-2.1.0/lib/Release/Win32;
'$toolsDir'/Windows/SDL2_ttf-2.0.15/lib/x86;
'$toolsDir'/Windows/SDL2-2.0.12/lib/x86;
'$toolsDir'/Windows/SDL2_net-2.0.1/lib/x86;
'$toolsDir'/Windows/SDL2_image-2.0.4/lib/x86;
'$toolsDir'/Windows/freeImage/x86;
'$toolsDir'/Windows/freetype/win32;
'$toolsDir'/Windows/GLFW3/lib-vc2015;
'$toolsDir'/Windows/gtest/lib/x86/Release;
'$toolsDir'/Windows/gmock/lib/x86/Release;
'$toolsDir'/Windows/Directx/Lib/x86;
'$toolsDir'/Windows/bullet/x86/Debug;
'
additionalRelease32LibsDir='
'$toolsDir'/Windows/fbx_sdk/lib/vs2017/x86/release;
'$toolsDir'/Windows/assimp-5.0.1/lib/x86/release;
'$toolsDir'/Windows/GL;
'$toolsDir'/Windows/glew-2.1.0/lib/Release/Win32;
'$toolsDir'/Windows/SDL2_ttf-2.0.15/lib/x86;
'$toolsDir'/Windows/SDL2-2.0.12/lib/x86;
'$toolsDir'/Windows/SDL2_net-2.0.1/lib/x86;
'$toolsDir'/Windows/SDL2_image-2.0.4/lib/x86;
'$toolsDir'/Windows/freeImage/x86;
'$toolsDir'/Windows/freetype/win32;
'$toolsDir'/Windows/GLFW3/lib-vc2015;
'$toolsDir'/Windows/gtest/lib/x86/Release;
'$toolsDir'/Windows/gmock/lib/x86/Release;
'$toolsDir'/Windows/Directx/Lib/x86;
'$toolsDir'/Windows/bullet/x86/Release;
'
additionalDebug64LibsDir='
'$toolsDir'/Windows/fbx_sdk/lib/vs2017/x64/debug;
'$toolsDir'/Windows/assimp-5.0.1/lib/x64/release;
'$toolsDir'/Windows/GL;
'$toolsDir'/Windows/glew-2.1.0/lib/Release/x64;
'$toolsDir'/Windows/SDL2_ttf-2.0.15/lib/x64;
'$toolsDir'/Windows/SDL2-2.0.12/lib/x64;
'$toolsDir'/Windows/SDL2_net-2.0.1/lib/x64;
'$toolsDir'/Windows/SDL2_image-2.0.4/lib/x64;
'$toolsDir'/Windows/freeImage/x64;
'$toolsDir'/Windows/freetype/win64;
'$toolsDir'/Windows/GLFW3/lib-vc2015;
'$toolsDir'/Windows/gtest/lib/x64/Release;
'$toolsDir'/Windows/gmock/lib/x64/Release;
'$toolsDir'/Windows/Directx/Lib/x64;
'$toolsDir'/Windows/bullet/x64/Debug;
'
additionalRelease64LibsDir='
'$toolsDir'/Windows/fbx_sdk/lib/vs2017/x64/release;
'$toolsDir'/Windows/assimp-5.0.1/lib/x64/release;
'$toolsDir'/Windows/GL;
'$toolsDir'/Windows/glew-2.1.0/lib/Release/x64;
'$toolsDir'/Windows/SDL2_ttf-2.0.15/lib/x64;
'$toolsDir'/Windows/SDL2-2.0.12/lib/x64;
'$toolsDir'/Windows/SDL2_net-2.0.1/lib/x64;
'$toolsDir'/Windows/SDL2_image-2.0.4/lib/x64;
'$toolsDir'/Windows/freeImage/x64;
'$toolsDir'/Windows/freetype/win64;
'$toolsDir'/Windows/GLFW3/lib-vc2015;
'$toolsDir'/Windows/gtest/lib/x64/Release;
'$toolsDir'/Windows/gmock/lib/x64/Release;
'$toolsDir'/Windows/Directx/Lib/x64;
'$toolsDir'/Windows/bullet/x64/Release;
'$toolsDir'/Windows/wxWidgets/x64/Release;
'

wxWidgetsReleaseLibs='
wxbase33u.lib
wxbase33u_net.lib
wxbase33u_xml.lib
wxbase33u_aui.lib
wxbase33u_richtext.lib
wxbase33u_xrc.lib
wxmsw33u_core.lib
wxmsw33u_adv.lib
wxmsw33u_aui.lib
wxmsw33u_html.lib
wxmsw33u_media.lib
wxmsw33u_ribbon.lib
wxmsw33u_richtext.lib
wxmsw33u_xrc.lib
wxpng.lib
wxregexu.lib
wxzlib.lib
'

# wxWidgetsReleaseLibs='
# wxbase33u.lib
# wxbase33u_net.lib
# wxbase33u_xml.lib
# wxexpat.lib
# wxjpeg.lib
# wxlexilla.lib
# wxmsw33u_adv.lib
# wxmsw33u_aui.lib
# wxmsw33u_core.lib
# wxmsw33u_gl.lib
# wxmsw33u_html.lib
# wxmsw33u_media.lib
# wxmsw33u_propgrid.lib
# wxmsw33u_qa.lib
# wxmsw33u_ribbon.lib
# wxmsw33u_richtext.lib
# wxmsw33u_stc.lib
# wxmsw33u_webview.lib
# wxmsw33u_xrc.lib
# wxpng.lib
# wxregexu.lib
# wxscintilla.lib
# wxtiff.lib
# wxwebp.lib
# wxzlib.lib
# '

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
BulletCollision.lib;
BulletDynamics.lib;
BulletFileLoader.lib;
BulletInverseDynamics.lib;
ConvexDecomposition.lib;
GIMPACTUtils.lib;
HACD.lib;
LinearMath.lib;
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

additionalDebugLibs='
'$bulletDebugLibs'
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
#'$wxWidgetsReleaseLibs'
additionalReleaseLibs='
'$bulletReleaseLibs'
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