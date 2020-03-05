from os import listdir
from os.path import isfile, join, splitext
import os

dir_path = os.path.realpath('.')

mypath="."
resultPath=dir_path + "/build/"
namespace=["GameEngine", "DebugNetworkInterface"]
indent="    "
basicTypes=["bool", "int", "int8", "int32", "uint8", "uint32", "float", "char", "std::string", "vec3", "vec2"]
complexTypes=["std::vector", "std::list"]

def StartNamespace(file):
    for name in namespace:
        file.write("namespace " + name + "\n")
        file.write("{\n")

def EndNamespace(file):
    for name in reversed(namespace):
        file.write("} // namespace " + name + "\n")

def GetHeaderFromCustomType(type):
    for complexType in complexTypes:
        if complexType in type:
            className = type.replace(complexType, "")
            className = className.replace("<", "")
            className = className.replace(">", "")
            return className
    return type

def IsComplextType(type):
    for complexType in complexTypes:
        if complexType in type:
            return True
    return False

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
    file.write("#include \"MessageTypes.h\"\n")

    for param in params:
        if not (param[0] in basicTypes):
            file.write("#include \"" + GetHeaderFromCustomType(param[0]) + ".h\"\n")

    file.write("\n")
    StartNamespace(file)

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
    EndNamespace(file)

    file.close()

def CreateSerializationFile(filename, params):
    file = open(resultPath + filename[0] + "XmlSerializer.h","w")
    file.write("#pragma once\n")
    file.write("#include <Utils/XML/XmlWriter.h>\n")
    file.write("#include \"" + fileName[0] + ".h\"\n")
    file.write("\n")

    StartNamespace(file)
    file.write("std::unique_ptr<Utils::XmlNode> Convert(const "+ fileName[0] + "&);\n")
    file.write("Network::IMessageData Serialize(const "+ fileName[0] + "&);\n")
    EndNamespace(file)
    file.close()

    file = open(resultPath + filename[0] + "XmlSerializer.cpp","w")
    file.write("#include \"" + fileName[0] + "XmlSerializer.h\"\n")
    file.write("#include <Utils.h>\n")
    file.write("#include <Utils/XML/XMLUtils.h>\n")
    file.write("#include <UtilsNetwork/MessageFormat.h>\n")
    file.write("#include <UtilsNetwork/Messages/XmlConverterUtils.h>\n")
    for param in params:
        if not (param[0] in basicTypes):
            file.write("#include \"" + GetHeaderFromCustomType(param[0]) + "XmlSerializer.h\"\n")

    file.write("\n")

    StartNamespace(file)
    file.write("std::unique_ptr<Utils::XmlNode> Convert(const "+ fileName[0] + "& input)\n")
    file.write("{\n")
    file.write(indent + "auto root = std::make_unique<Utils::XmlNode>(\"" + fileName[0] + "\");\n")

    for param in params:
        if param[0] in basicTypes:
            if param[0] == "std::string":
                file.write(indent + "root->attributes_.insert({\"" + param[1] + "\", input." + param[1] + "});\n")
            elif param[0] == "vec3" or param[0] == "vec2":
                file.write(indent + "root->AddChild(std::move(Utils::Convert(\"" + param[1] + "\", input." + param[1] +")));\n")
            else:
                file.write(indent + "root->attributes_.insert({\"" + param[1] + "\", std::to_string(input." + param[1] + ")});\n")
        elif IsComplextType(param[0]):
            file.write(indent + "root->AddChild(std::move(Utils::Convert(\""+ param[1] + "\", input.params)));\n")
        else:
            file.write(indent + "root->AddChild(std::move(Convert(input." + param[1] +")));\n")

    file.write(indent + "return root;\n")
    file.write("}\n")

    file.write("Network::IMessageData Serialize(const "+ fileName[0] + "& input)\n")
    file.write("{\n")
    file.write(indent + "auto root = Convert(input);\n")
    file.write(indent + "return Network::CreatePayload(*root);\n")
    file.write("}\n")
    EndNamespace(file)
    file.close()

def CreateDeserializationFile(filename, params):
    file = open(resultPath + filename[0] + "XmlDeserializer.h","w")
    file.write("#pragma once\n")
    file.write("#include \"" + fileName[0] + ".h\"\n")
    file.write("#include <Utils/XML/XmlReader.h>\n")
    file.write("\n")
    StartNamespace(file)
    file.write("void SetParam(" + filename[0] + "&, Utils::XmlNode&);\n")
    file.write("std::unique_ptr<Network::IMessage> Deserialize"+ filename[0] +"(Utils::XmlReader& reader);\n")
    EndNamespace(file)
    file.close()

    file = open(resultPath + filename[0] + "XmlDeserializer.cpp","w")
    file.write("#include \"" + fileName[0] + "XmlDeserializer.h\"\n")
    file.write("#include <Utils/XML/XMLUtils.h>\n")
    for param in params:
        if not (param[0] in basicTypes):
            file.write("#include \"" + GetHeaderFromCustomType(param[0]) + "XmlDeserializer.h\"\n")
    file.write("\n")
    StartNamespace(file)
    file.write("void SetParam(" + filename[0] + "& output, Utils::XmlNode& input)\n")
    file.write("{\n")
    for param in params:
        file.write(indent + "if (input.IsAttributePresent(\"" + param[1] + "\"))\n")
        file.write(indent + "{\n")

        if param[0] in basicTypes:
            if param[0] == "std::string":
                file.write(indent + indent + "output." + param[1] + " = input.attributes_.at(\"" + param[1] + "\");\n")
            if param[0] == "int" or param[0] == "int8" or param[0] == "int32" or param[0] == "uint8" or param[0] == "uint32" or param[0] == "char":
                file.write(indent + indent + "output." + param[1] + " = std::stoi(input.attributes_.at(\"" + param[1] + "\"));\n")
            if param[0] == "float":
                file.write(indent + indent + "output." + param[1] + " = std::stof(input.attributes_.at(\"" + param[1] + "\"));\n")
            if param[0] == "vec2":
                file.write(indent + indent + "output." + param[1] + " = Utils::ConvertToVec2(*input.GetChild(\"" + param[1] + "\"));\n")
            if param[0] == "vec3":
                file.write(indent + indent + "output." + param[1] + " = Utils::ConvertToVec3(*input.GetChild(\"" + param[1] + "\"));\n")
        elif IsComplextType(param[0]):
            file.write(indent + indent + "for (auto& child : input.GetChild(\"" + param[1] + "\")->GetChildren())\n")
            file.write(indent + indent + "{\n")
            file.write(indent + indent + indent + GetHeaderFromCustomType(param[0]) + " obj;\n")
            file.write(indent + indent + indent + "SetParam(obj, *child);\n")
            file.write(indent + indent + indent + "output." + param[1] + ".push_back(obj);\n")
            file.write(indent + indent + "}\n")
        else:
            file.write(indent + indent + "SetParam(output." + param[1] + ", *input.GetChild(\"" + param[1] + "\"));\n")

        file.write(indent + "}\n")
    file.write("}\n")
    file.write("std::unique_ptr<Network::IMessage> Deserialize"+ filename[0] + "(Utils::XmlReader& reader)\n")
    file.write("{\n")
    file.write(indent + "auto result = std::make_unique<"+ fileName[0] + ">();\n")
    file.write(indent + "auto msg = reader.Get(\""+ fileName[0] + "\");\n")
    file.write(indent + "if (not msg) return nullptr;\n")
    file.write(indent + "SetParam(*result, *msg);\n")
    file.write(indent + "return std::move(result);\n")
    file.write("}\n")
    EndNamespace(file)
    file.close()

def CreateMessageTypesFile(fileNames):
    file = open(resultPath + "MessageTypes.h","w")
    file.write("#pragma once\n")
    file.write("\n")
    StartNamespace(file)
    file.write("enum class MessageTypes\n")
    file.write("{\n")
    file.write(indent + "Any,\n")
    i=0
    for filename in fileNames:
        if i < len(fileNames) - 1:
            file.write(indent + filename + ",\n")
        else:
            file.write(indent + filename + "\n")
        i = i + 1
    file.write("};\n")
    EndNamespace(file)
    file.close()

if __name__ == "__main__":
    if not os.path.exists(resultPath):
        os.makedirs(resultPath)

    onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]

    templateFiles = []
    for fileNameWithExtension in onlyfiles:
        fileName=fileNameWithExtension.split('.')
        if fileName[1]=="template":
            params = ReadFile(fileNameWithExtension)
            print(fileName)
            print(params)
            templateFiles.append(fileName[0])
            CreateClassFile(fileName, params)
            CreateSerializationFile(fileName, params)
            CreateDeserializationFile(fileName, params)

    CreateMessageTypesFile(templateFiles)



