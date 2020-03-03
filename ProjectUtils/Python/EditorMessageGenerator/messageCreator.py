from os import listdir
from os.path import isfile, join, splitext
import os

dir_path = os.path.realpath('.')

mypath="."
resultPath=dir_path + "/build/"
namespace="GameEngine"
indent="    "
basicTypes=["bool", "int", "int8", "int32", "uint8", "uint32", "float", "char", "std::string", "vec3", "vec2"]
complexTypes=["std::vector", "std::list"]

def GetHeaderFromCustomType(type):
    for complexType in complexTypes:
        if complexType in type:
            className = type.replace(complexType, "")
            className = className.replace("<", "")
            className = className.replace(">", "")
            return className
    return type

def ReadFile(fileName):
    file = open("./" + fileName, "r")
    print("File " + fileName +" content : ")
    result=[]
    for line in file:
        if line.endswith('\n'):
            line=line[:-1]
        result.append(line.split(' '))
    file.close()
    return result

def CreateClassFile(filename, params):
    file = open(resultPath + filename[0] + ".h","w")
    file.write("#pragma once\n")
    file.write("#include <GLM/GLMUtils.h>\n")
    file.write("#include <UtilsNetwork/IMessage.h>\n")
    file.write("#include <UtilsNetwork/MessageTarget.h>\n")
    file.write("#include \"GameEngine/DebugTools/EditorInterface/MessageTypes.h\"\n")

    for param in params:
        if not (param[0] in basicTypes):
            file.write("#include \"" + GetHeaderFromCustomType(param[0]) + ".h\"\n")

    file.write("\n")
    file.write("namespace " + namespace + "\n")
    file.write("{\n")
    file.write("class " + fileName[0] + " : public Network::IMessage\n")
    file.write("{\n")
    file.write("public:\n")
    file.write(indent + fileName[0] + "()\n")
    file.write(indent + ": IMessage(static_cast<uint8>(MessageTypes::" +  fileName[0] + "), Network::MessageTarget::All)\n")
    file.write(indent + "{\n")
    file.write(indent + "}\n")
    file.write("\n")

    for param in params:
        file.write(indent + param[0] + " " + param[1] + ";\n")

    file.write("};\n")
    file.write("} // namespace " + namespace + " \n")
    file.close()

def CreateSerializationFile(filename, params):
    # Utils::XmlNode root("NewGameObjectInd");
    # root.attributes_.insert({"id", std::to_string(msg->id)});
    # root.attributes_.insert({"parentId", std::to_string(msg->parentId)});
    # root.attributes_.insert({"name", msg->GetName()});
    # DEBUG_LOG(Utils::Xml::Write(root));
    # auto v = Network::CreatePayload(root);
    #include <Utils.h>
    #include <Utils/XML/XmlReader.h>
    #include <Utils/XML/XmlWriter.h>
    #include <UtilsNetwork/MessageFormat.h>
    #include <UtilsNetwork/Messages/XmlConverterUtils.h>

    file = open(resultPath + filename[0] + "XmlSerilizer.h","w")
    file.write("#pragma once\n")
    file.write("#include <Utils/XML/XmlWriter.h>\n")
    file.write("#include \"" + fileName[0] + ".h\"\n")
    file.write("\n")
    file.write("namespace " + namespace + "\n")
    file.write("{\n")
    file.write("Utils::XmlNode Convert(const "+ fileName[0] + "&);\n")
    file.write("IMessageData Serilize(const "+ fileName[0] + "&);\n")
    file.write("} // namespace " + namespace + " \n")
    file.close()

    file = open(resultPath + filename[0] + "XmlSerilizer.cpp","w")
    file.write("#include \"" + fileName[0] + "XmlSerilizer.h\"\n")
    file.write("#include <Utils.h>\n")
    file.write("#include <Utils/XML/XMLUtils.h>\n")
    file.write("#include <UtilsNetwork/MessageFormat.h>\n")
    file.write("#include <UtilsNetwork/Messages/XmlConverterUtils.h>\n")
    for param in params:
        if not (param[0] in basicTypes):
            file.write("#include \"" + GetHeaderFromCustomType(param[0]) + "XmlSerilizer.h\"\n")

    file.write("\n")
    file.write("namespace " + namespace + "\n")
    file.write("{\n")

    file.write("Utils::XmlNode Convert(const "+ fileName[0] + "& input)\n")
    file.write("{\n")
    file.write(indent + "Utils::XmlNode root(\"" + fileName[0] + "\");\n")

    for param in params:
        if param[0] in basicTypes:
            if param[0] == "std::string":
                file.write(indent + "root.attributes_.insert({\"" + param[1] + "\", input." + param[1] + "});\n")
            elif param[0] == "vec3" or param[0] == "vec2":
                file.write(indent + "root.AddChild(Convert(input." + param[1] +"));\n")
            else:
                file.write(indent + "root.attributes_.insert({\"" + param[1] + "\", std::to_string(input." + param[1] + ")});\n")
        else:
            file.write(indent + "root.AddChild(Convert(input." + param[1] +"));\n")

    file.write(indent + "return root;\n")
    file.write("}\n")

    file.write("IMessageData Serilize(const "+ fileName[0] + "& input)\n")
    file.write("{\n")
    file.write(indent + "auto root = Convert(input);\n")
    file.write(indent + "return Network::CreatePayload(root);\n")
    file.write("}\n")

    file.write("} // namespace " + namespace + " \n")
    file.close()

def CreateDeserializationFile(filename, params):
    file = open(resultPath + filename[0] + "XmlDeserilizer.h","w")
    file.write("#pragma once\n")
    file.write("#include \"" + fileName[0] + ".h\"\n")
    file.write("\n")
    file.write("namespace " + namespace + "\n")
    file.write("{\n")
    file.write("std::unique_ptr<Network::IMessage> Deserilize(const "+ fileName[0] + "&);\n")
    file.write("} // namespace " + namespace + " \n")
    file.close()

    file = open(resultPath + filename[0] + "XmlDeserilizer.cpp","w")
    file.write("#include \"" + fileName[0] + "XmlDeserilizer.h\"\n")
    file.write("\n")
    file.write("namespace " + namespace + "\n")
    file.write("{\n")
    file.write("std::unique_ptr<Network::IMessage> Deserilize(const "+ fileName[0] + "&)\n")
    file.write("{\n")
    file.write(indent + "return nullptr;\n")
    file.write("}\n")
    file.write("} // namespace " + namespace + " \n")
    file.close()


if __name__ == "__main__":
    if not os.path.exists(resultPath):
        os.makedirs(resultPath)

    onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]

    for fileNameWithExtension in onlyfiles:
        fileName=fileNameWithExtension.split('.')
        if fileName[1]=="template":
            params = ReadFile(fileNameWithExtension)
            print(fileName)
            print(params)
            CreateClassFile(fileName, params)
            CreateSerializationFile(fileName, params)
            CreateDeserializationFile(fileName, params)


