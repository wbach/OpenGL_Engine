import bpy
import xml.etree.cElementTree as ET
import copy

dataPath="E:\\Projects\\OpenGL_Engine\\Data\\"
prefabNode = ET.Element("prefab")
gameObjectsNode = ET.SubElement(prefabNode, "gameObjects")

# id = 1
# for mesh in bpy.data.meshes:
#     objectNode = ET.SubElement(gameObjectsNode, "gameObject" )
#     objectNode.attrib["id"] = str(id)
#     objectNode.attrib["name"] = mesh.name_full

#     localTransformNode = ET.SubElement(objectNode, "localTransform")
#     positionNode = ET.SubElement(localTransformNode, "position")
#  #   objectNode.attrib["x"] = str(mesh.)
#     rotationNode = ET.SubElement(localTransformNode, "rotation")
#     scaleNode = ET.SubElement(localTransformNode, "scale")
#     id = id + 1

def ToDegrees(a):
    return a * 180.0 / 3.14159265358979323846264338327950288

for obj in bpy.data.objects:
    obj.select_set(False)

def createRenderComponent(componentsNode, modelFilepath):
    rendererNode = ET.SubElement(componentsNode, "component")
    rendererNode.attrib["type"] = "Renderer"
    ET.SubElement(rendererNode, "textureIndex").text = "0"
    ET.SubElement(rendererNode, "meshOptimize").text = "true"
    ET.SubElement(rendererNode, "modelNormalization").text = "false"
    modelFileNamesNode = ET.SubElement(rendererNode, "modelFileNames")
    modelFileNameNode = ET.SubElement(modelFileNamesNode, "modelFileName")
    ET.SubElement(modelFileNameNode, "fileName").text = modelFilepath
    ET.SubElement(modelFileNameNode, "lvlOfDetail").text = "0"

def createMeshShapeNode(componentsNode):
    meshShapeNode = ET.SubElement(componentsNode, "component")
    meshShapeNode.attrib["type"] = "MeshShape"
    positionOffsetNode = ET.SubElement(meshShapeNode, "positionOffset")
    positionOffsetNode.attrib["x"] = "0.000000"
    positionOffsetNode.attrib["y"] = "0.000000"
    positionOffsetNode.attrib["z"] = "0.000000"
    ET.SubElement(meshShapeNode, "size").text = "1.000000"
    ET.SubElement(meshShapeNode, "autoOptimize").text = "false"
    modelFileNameNode = ET.SubElement(meshShapeNode, "modelFileName")
    ET.SubElement(modelFileNameNode, "fileName")
    ET.SubElement(modelFileNameNode, "modelNormalization").text = "false"
    ET.SubElement(modelFileNameNode, "meshOptimize").text = "true"

def createRigidbodyNode(componentsNode):
    rigidbodyNode = ET.SubElement(componentsNode, "component")
    rigidbodyNode.attrib["type"] = "Rigidbody"
    ET.SubElement(rigidbodyNode, "mass").text = "0.000000"
    ET.SubElement(rigidbodyNode, "isStatic").text = "false"
    ET.SubElement(rigidbodyNode, "noConctactResponse").text = "false"
    ET.SubElement(rigidbodyNode, "collisionShape")
    
def createCollisionComponents(componentsNode):
    # <component type="MeshShape">
    #     <positionOffset z="0.000000" y="0.000000" x="0.000000"/>
    #     <size>1.000000</size>
    #     <autoOptimize>false</autoOptimize>
    #     <modelFileName>
    #         <fileName/>
    #         <modelNormalization>true</modelNormalization>
    #         <meshOptimize>true</meshOptimize>
    #     </modelFileName>
    # </component>
    # <component type="Rigidbody">
    #     <mass>0.000000</mass>
    #     <isStatic>false</isStatic>
    #     <noConctactResponse>false</noConctactResponse>
    #     <collisionShape/>
    # </component>
    createMeshShapeNode(componentsNode)
    createRigidbodyNode(componentsNode)


# // @param in 3d position vector in Right handed Coordinate System
# // @param out 3d position vector in Unity Coordinate System
# void RHSToUnity(Vector3 in, ref Vector3 out)
# { 
#   out[0] = in[1]; 
#   out[1] = in[2]; 
#   out[2] = -in[0];
# }

def convertAxis(v):
    out = copy.deepcopy(v)
    # out[0] = v[1]
    # out[1] = v[2] 
    # out[2] = -v[0]
    # out[0] = -v[2]
    # out[1] = v[0] 
    # out[2] = v[1]
    out[0] = v[0]
    out[1] = v[2] 
    out[2] = v[1]
    return out

# // @param in 3d position vector in Unity Coordinate System
# // @param out 3d position vector in Right handed Coordinate System
# void UnityToRHS(Vector3 in, ref Vector3 out)
# { 
#   out[0] = -in[2]; 
#   out[1] = in[0]; 
#   out[2] = in[1];
# }

id = 1
for obj in bpy.data.objects:
    if obj.type != "MESH" or obj.name_full == "RefImage":
        continue
    objectNode = ET.SubElement(gameObjectsNode, "gameObject" )
    objectNode.attrib["id"] = str(id)
    objectNode.attrib["name"] = obj.name_full
    #objectNode.attrib["type"] = obj.type

    localTransformNode = ET.SubElement(objectNode, "localTransform")
    
    location = copy.deepcopy(obj.location.xyz)
    axisConvertedLocation = convertAxis(location)
    positionNode = ET.SubElement(localTransformNode, "position")
    positionNode.attrib["x"] = str(axisConvertedLocation.x)
    positionNode.attrib["y"] = str(axisConvertedLocation.y) # Blender is other axis system
    positionNode.attrib["z"] = str(-axisConvertedLocation.z)
   
    rotation = copy.deepcopy(obj.rotation_euler)
    axisConvertedRotation = convertAxis(rotation)
    rotationNode = ET.SubElement(localTransformNode, "rotation")
    rotationNode.attrib["x"] = str(ToDegrees(axisConvertedRotation.x))
    rotationNode.attrib["y"] = str(ToDegrees(axisConvertedRotation.y)) # Blender is other axis system
    rotationNode.attrib["z"] = str(ToDegrees(axisConvertedRotation.z))
    
    scale = copy.deepcopy(obj.scale.xyz)
    axisConvertedScale = convertAxis(scale)
    scaleNode = ET.SubElement(localTransformNode, "scale")
    scaleNode.attrib["x"] = str(axisConvertedScale.x)
    scaleNode.attrib["y"] = str(axisConvertedScale.y)# Blender is other axis system
    scaleNode.attrib["z"] = str(axisConvertedScale.z)

    componentsNode = ET.SubElement(objectNode, "components")
    componentsNode.attrib["count"] = str(3)

    modelFilepath="BlenderExported/bengine_{}.obj".format(obj.name_full)

    createRenderComponent(componentsNode, modelFilepath)
    createCollisionComponents(componentsNode)

    # rendererNode = ET.SubElement(componentsNode, "component")
    # rendererNode.attrib["type"] = "Renderer"
    # ET.SubElement(rendererNode, "textureIndex").text = "0"
    # ET.SubElement(rendererNode, "meshOptimize").text = "true"
    # ET.SubElement(rendererNode, "modelNormalization").text = "false"
    # modelFileNamesNode = ET.SubElement(rendererNode, "modelFileNames")
    # modelFileNameNode = ET.SubElement(modelFileNamesNode, "modelFileName")
    # ET.SubElement(modelFileNameNode, "filename").text = modelFilepath
    # ET.SubElement(modelFileNameNode, "lvlOfDetail").text = "0"

    obj.select_set(True)
    obj.location[0] = 0
    obj.location[1] = 0
    obj.location[2] = 0
    obj.rotation_euler[0] = 0
    obj.rotation_euler[1] = 0
    obj.rotation_euler[2] = 0
    obj.scale[0] = 1
    obj.scale[1] = 1
    obj.scale[2] = 1


    bpy.ops.wm.obj_export(export_selected_objects=True, filepath=dataPath + modelFilepath)

    obj.location = location
    obj.rotation_euler = rotation
    obj.scale = scale

    obj.select_set(False)

    id = id + 1


tree = ET.ElementTree(prefabNode)
ET.indent(tree, space="\t", level=0)
tree.write(dataPath + "Scenes\\Prefabs\\BlenderScenePrefab.xml")