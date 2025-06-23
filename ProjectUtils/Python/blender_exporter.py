import bpy
import xml.etree.cElementTree as ET
import copy

#dataPath="E:\\Projects\\OpenGL_Engine\\Data\\"
dataPath="/home/baszek/Projects/OpenGL_Engine/Data/"

class Vector:
    def __init__(self, x = 0.0, y = 0.0, z = 0.0):
        self.x = x
        self.y = y
        self.z = z

def ToDegrees(a):
    return a * 180.0 / 3.14159265358979323846264338327950288

for obj in bpy.data.objects:
    obj.select_set(False)

def exportWaveFrontObject(obj : bpy.types.Object, file):
    location = copy.deepcopy(obj.location.xyz)
    rotation = copy.deepcopy(obj.rotation_euler)
    scale = copy.deepcopy(obj.scale.xyz)

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

    bpy.ops.wm.obj_export(export_selected_objects=True, filepath=file)

    obj.location = location
    obj.rotation_euler = rotation
    obj.scale = scale

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

def first_match(iterable, predicate):
    try:
        return next(idx for idx,n in enumerate(iterable) if predicate(n))
    except StopIteration:
        return None

def createMeshShapeNode(componentsNode, colliderFile):
    meshShapeNode = ET.SubElement(componentsNode, "component")
    meshShapeNode.attrib["type"] = "MeshShape"
    positionOffsetNode = ET.SubElement(meshShapeNode, "positionOffset")
    positionOffsetNode.attrib["x"] = "0.000000"
    positionOffsetNode.attrib["y"] = "0.000000"
    positionOffsetNode.attrib["z"] = "0.000000"
    ET.SubElement(meshShapeNode, "size").text = "1.000000"
    ET.SubElement(meshShapeNode, "autoOptimize").text = "false"
    modelFileNameNode = ET.SubElement(meshShapeNode, "modelFileName")
    ET.SubElement(modelFileNameNode, "fileName").text = colliderFile
    ET.SubElement(modelFileNameNode, "modelNormalization").text = "false"
    ET.SubElement(modelFileNameNode, "meshOptimize").text = "true"

def createRigidbodyNode(componentsNode, isStatic = "false", collisionShape = ""):
    rigidbodyNode = ET.SubElement(componentsNode, "component")
    rigidbodyNode.attrib["type"] = "Rigidbody"
    ET.SubElement(rigidbodyNode, "mass").text = "0.000000"
    ET.SubElement(rigidbodyNode, "isStatic").text = isStatic
    ET.SubElement(rigidbodyNode, "noConctactResponse").text = "false"
    ET.SubElement(rigidbodyNode, "collisionShape").text = collisionShape

def createCollisionComponents(componentsNode, colliderFile):
    createMeshShapeNode(componentsNode, colliderFile)
    createRigidbodyNode(componentsNode)

def createLocalTransformNode(parentNode, position, rotation, scale):
    localTransformNode = ET.SubElement(parentNode, "localTransform")
    positionNode = ET.SubElement(localTransformNode, "position")
    positionNode.attrib["x"] = str(position.x)
    positionNode.attrib["y"] = str(position.y)
    positionNode.attrib["z"] = str(position.z)

    rotationNode = ET.SubElement(localTransformNode, "rotation")
    rotationNode.attrib["x"] = str(rotation.x)
    rotationNode.attrib["y"] = str(rotation.y)
    rotationNode.attrib["z"] = str(rotation.z)

    scaleNode = ET.SubElement(localTransformNode, "scale")
    scaleNode.attrib["x"] = str(scale.x)
    scaleNode.attrib["y"] = str(scale.y)
    scaleNode.attrib["z"] = str(scale.z)

def convertAxis(v):
    out = copy.deepcopy(v)
    out[0] = v[0]
    out[1] = v[2]
    out[2] = v[1]
    return out

def createTerrainTexture(parentNode, type, scale, texture):
    textureNode = ET.SubElement(parentNode, "texture")
    ET.SubElement(textureNode, "textureType").text = type
    ET.SubElement(textureNode, "scale").text = str(scale)
    ET.SubElement(textureNode, "textureFileName").text = texture

def createTerrainShapeComponent(parentNode, heightMap):
    shapeNode = ET.SubElement(parentNode, "component")
    shapeNode.attrib["type"] = "TerrainShape"
    ET.SubElement(shapeNode, "heightMapFileName").text = heightMap
    offsetNode = ET.SubElement(shapeNode, "positionOffset")
    offsetNode.attrib["x"] = "0.0"
    offsetNode.attrib["y"] = "0.0"
    offsetNode.attrib["z"] = "0.0"

def createTerrainRendererComponent(parentNode, heightMap):
    terrainRenderNode = ET.SubElement(parentNode, "component")
    terrainRenderNode.attrib["type"] = "TerrainRenderer"
    terrainTexturesNode = ET.SubElement(terrainRenderNode, "textureFileNames")
    createTerrainTexture(terrainTexturesNode, "heightmap", 1.0, heightMap)
    #createTerrainTexture(terrainTexturesNode, "blendMap", 1.0, "Textures/Terrain/BlendMaps/heightmap_Kingdom_2.png")
    createTerrainTexture(terrainTexturesNode, "backgorundTexture", 0.2, "Textures/Terrain/Ground/oreon/grass0_DIF.jpg")



prefabNode = ET.Element("prefab")
prefabNode.attrib["name"] = bpy.path.basename(bpy.context.blend_data.filepath)

createLocalTransformNode(prefabNode, Vector(), Vector(), Vector(1, 1, 1))
childrenNode = ET.SubElement(prefabNode, "children")

id = 1
for obj in bpy.data.objects:
    if obj.type != "MESH" or obj.name_full == "RefImage" or ("collider" in obj.name_full):
        continue

    objectNode = ET.SubElement(childrenNode, "gameObject" )
    objectNode.attrib["id"] = str(id)
    objectNode.attrib["name"] = obj.name_full
    #objectNode.attrib["type"] = obj.type

    location = copy.deepcopy(obj.location.xyz)
    axisConvertedLocation = convertAxis(location)
    axisConvertedLocation[2] = -axisConvertedLocation[2]

    rotation = copy.deepcopy(obj.rotation_euler)
    axisConvertedRotation = convertAxis(rotation)
    axisConvertedRotation[0] = ToDegrees(axisConvertedRotation.x)
    axisConvertedRotation[1] = ToDegrees(axisConvertedRotation.y)
    axisConvertedRotation[2] = ToDegrees(axisConvertedRotation.z)

    scale = copy.deepcopy(obj.scale.xyz)
    axisConvertedScale = convertAxis(scale)

    createLocalTransformNode(objectNode, axisConvertedLocation, axisConvertedRotation, axisConvertedScale)

    componentsNode = ET.SubElement(objectNode, "components")
    componentsNode.attrib["count"] = str(3)

    modelFilepath="BlenderExported/bengine_{}.obj".format(obj.name_full)
    exportWaveFrontObject(obj, file=dataPath + modelFilepath)

    colliderIndex = first_match(obj.children, lambda obj: ("collider" in obj.name_full))
    colliderFile = ""
    if colliderIndex != None:
        # remove parent transform
        obj.location[0] = 0
        obj.location[1] = 0
        obj.location[2] = 0
        obj.rotation_euler[0] = 0
        obj.rotation_euler[1] = 0
        obj.rotation_euler[2] = 0
        obj.scale[0] = 1
        obj.scale[1] = 1
        obj.scale[2] = 1

        colliderFile="BlenderExported/collider_bengine_{}.obj".format(obj.name_full)
        exportWaveFrontObject(obj.children[colliderIndex], file=dataPath + colliderFile)

        # restore parent transform
        obj.location = location
        obj.rotation_euler = rotation
        obj.scale = scale

    # if ("terrain" in obj.name_full or "Terrain" in obj.name_full):
    #     createTerrainRendererComponent(componentsNode, modelFilepath)

    #     if colliderFile == "":
    #         colliderFile = modelFilepath
    #     createTerrainShapeComponent(componentsNode, colliderFile)
    #     createRigidbodyNode(componentsNode, "true", "TerrainShape")
    # else:
        createRenderComponent(componentsNode, modelFilepath)
        createCollisionComponents(componentsNode, colliderFile)

    id = id + 1


tree = ET.ElementTree(prefabNode)
ET.indent(tree, space="\t", level=0)
tree.write(dataPath + "Scenes/Prefabs/BlenderScenePrefab.xml")