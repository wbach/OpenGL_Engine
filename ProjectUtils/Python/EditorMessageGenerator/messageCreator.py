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

def CreateStructFile(filename, params):
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

    file.write("struct " + fileName[0] + " : public Network::IMessage\n")
    file.write("{\n")
    file.write(indent + fileName[0] + "()\n")
    # default constructor
    file.write(indent + ": IMessage(Convert(MessageTypes::" +  fileName[0] + "), Network::MessageTarget::All)\n")
    file.write(indent + "{\n")
    file.write(indent + "}\n")
    # end of default constructor

    # custom constructor
    if params:
        file.write(indent + fileName[0] + "(\n");
        i = 0
        for param in params:
            if param[0] in basicTypes and not (param[0] == "std::string" or param[0] == "vec2" or param[0] == "vec3"):
                file.write(indent + indent + indent + param[0] + " " + param[1])
            else:
                file.write(indent + indent + indent + "const " + param[0] + "& " + param[1])
            if i < len(params) - 1:
                file.write(",\n")
            i = i + 1

        file.write(")\n")
        file.write(indent + ": IMessage(Convert(MessageTypes::" +  fileName[0] + "), Network::MessageTarget::All)\n")

        for param in params:
            file.write(indent + ", ")
            file.write(param[1] + "{" + param[1] + "}\n")


        file.write(indent + "{\n")
        file.write(indent + "}\n")
        file.write("\n")
    # end of custom constructor

    # create members
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

def CreateConverter(fileNames):
    file = open(resultPath + "XmlMessageConverter.h","w")
    file.write("#pragma once\n")
    file.write("#include <UtilsNetwork/IMessageConverter.h>\n")
    file.write("\n")
    StartNamespace(file)
    file.write("class XmlMessageConverter : public Network::IMessageConverter\n")
    file.write("{\n")
    file.write("public:\n")
    file.write(indent + "virtual bool IsValid(Network::IMessageFormat, Network::IMessageType) const override;\n")
    file.write(indent + "virtual std::unique_ptr<Network::IMessage> Convert(Network::IMessageType, const Network::IMessageData&) override;\n")
    file.write(indent + "virtual Network::IMessageData Convert(const Network::IMessage&) override;\n")
    file.write("};\n")
    EndNamespace(file)
    file.close()

    file = open(resultPath + "XmlMessageConverter.cpp","w")
    file.write("#include \"XmlMessageConverter.h\"\n")
    file.write("#include <UtilsNetwork/MessageFormat.h>\n")
    file.write("#include <UtilsNetwork/Messages/XmlConverterUtils.h>\n")
    file.write("#include \"MessageTypes.h\"\n")
    for filename in fileNames:
        file.write("#include \"" + filename + "XmlSerializer.h\"\n")
        file.write("#include \"" + filename + "XmlDeserializer.h\"\n")
    file.write("\n")
    StartNamespace(file)
    file.write("bool XmlMessageConverter::IsValid(Network::IMessageFormat format, Network::IMessageType type) const\n")
    file.write("{\n")
    file.write(indent + "return format == Network::ConvertFormat(Network::MessageFormat::Xml) and\n")
    file.write(indent + indent + "type >= " + CreateNamespaceRangeString() + "MIN_VALUE and\n")
    file.write(indent + indent + "type <= " + CreateNamespaceRangeString() + "MAX_VALUE;\n")
    file.write("}\n")
    file.write("std::unique_ptr<Network::IMessage> XmlMessageConverter::Convert(Network::IMessageType messageType, const Network::IMessageData& message)\n")
    file.write("{\n")
    file.write(indent + "DEBUG_LOG(Network::Convert(message));\n")
    file.write(indent + "Utils::XmlReader reader;\n")
    file.write(indent + "std::string convertedMessage = Network::Convert(message);\n")
    file.write(indent + "if (not reader.ReadXml(convertedMessage)) return nullptr;\n")
    file.write(indent + "auto type = DebugNetworkInterface::Convert(messageType);\n")
    file.write(indent + "if (not type) return nullptr;\n")
    file.write(indent + "switch (*type)\n")
    file.write(indent + "{\n")
    for filename in fileNames:
        file.write(indent + "case MessageTypes::" + filename + ": ")
        file.write("return Deserialize" + filename + "(reader);\n")
    file.write(indent + "}\n")
    file.write(indent + "return nullptr;\n")
    file.write("}\n")
    file.write("Network::IMessageData XmlMessageConverter::Convert(const Network::IMessage& message)\n")
    file.write("{\n")
    file.write(indent + "auto type = DebugNetworkInterface::Convert(message.GetType());\n")
    file.write(indent + "if (not type) return {};\n")
    file.write(indent + "switch (*type)\n")
    file.write(indent + "{\n")
    for filename in fileNames:
        file.write(indent + "case " +  "MessageTypes::" + filename + ":\n")
        file.write(indent + "{\n")
        file.write(indent + indent + "auto msg = Network::castMessageAs<" + filename + ">(message);\n")
        file.write(indent + indent + "return Serialize(*msg);\n")
        file.write(indent + "}\n")
    file.write(indent + "}\n")
    file.write(indent + "return {};\n")
    file.write("}\n")
    EndNamespace(file)
    file.close()

def CreateNamespaceRangeString():
    result=""
    for name in namespace:
        result = result + name + "::"
    return result

def CreateMessageTypesFile(fileNames):
    file = open(resultPath + "MessageTypes.h","w")
    file.write("#pragma once\n")
    file.write("#include <Types.h>\n")
    file.write("#include <optional>\n")
    file.write("#include <UtilsNetwork/IMessage.h>\n")
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
    file.write("const uint8 MIN_VALUE{static_cast<uint8>(MessageTypes::Any)};\n")
    file.write("const uint8 MAX_VALUE{static_cast<uint8>(MessageTypes::" + fileNames[-1] +")};\n")
    file.write("Network::IMessageType Convert(MessageTypes);\n")
    file.write("std::optional<MessageTypes> Convert(Network::IMessageType);\n")
    file.write(CreateNamespaceRangeString() + "MessageTypes CreateFromString(const std::string&);\n")
    EndNamespace(file)
    file.write("namespace std\n")
    file.write("{\n")
    file.write("string to_string(" + CreateNamespaceRangeString() + "MessageTypes);\n")
    file.write("} // namespace std\n")
    file.close()

    file = open(resultPath + "MessageTypes.cpp","w")
    file.write("#include \"MessageTypes.h\"\n")
    file.write("\n")

    StartNamespace(file)
    file.write("Network::IMessageType Convert(MessageTypes type)\n")
    file.write("{\n")
    file.write(indent + "return static_cast<Network::IMessageType>(type);\n")
    file.write("}\n")
    file.write("std::optional<MessageTypes> Convert(Network::IMessageType type)\n")
    file.write("{\n")
    file.write(indent + "return (type >= MIN_VALUE and MIN_VALUE <= MAX_VALUE) ? static_cast<MessageTypes>(type) : std::optional<MessageTypes>();\n")
    file.write("}\n")
    file.write(CreateNamespaceRangeString() + "MessageTypes CreateFromString(const std::string& type)\n")
    file.write("{\n")
    for filename in fileNames:
        file.write(indent + "if (type == \"" + filename + "\") return " + CreateNamespaceRangeString() + "MessageTypes::" + filename + ";\n")
    file.write(indent + "return " + CreateNamespaceRangeString() + "MessageTypes::Any;\n")
    file.write("}\n")
    EndNamespace(file)

    file.write("namespace std\n")
    file.write("{\n")
    file.write("string to_string(" + CreateNamespaceRangeString() + "MessageTypes type)\n")
    file.write("{\n")
    for filename in fileNames:
        file.write(indent + "if (type == " + CreateNamespaceRangeString() + "MessageTypes::"+ filename + ") return \"" + filename + "\";\n")
    file.write(indent + "return \"Unknown type\";\n")
    file.write("}\n")
    file.write("} // namespace std\n")
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
            CreateStructFile(fileName, params)
            CreateSerializationFile(fileName, params)
            CreateDeserializationFile(fileName, params)

    CreateMessageTypesFile(templateFiles)
    CreateConverter(templateFiles)



