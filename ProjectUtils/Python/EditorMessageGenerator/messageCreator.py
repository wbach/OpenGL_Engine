from os import listdir
from os.path import isfile, join, splitext
import os

dir_path = os.path.realpath('.')

mypath="."
resultPath=dir_path + "\\build\\"
print("------------")
print(resultPath)
print("------------")
namespace="GameEngine"
indent="    "

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
    file = open(resultPath + filename[0] + "Serilizer.h","w") 
    file.close()
    file = open(resultPath + filename[0] + "Serilizer.cpp","w") 
    file.close()

def CreateDeserializationFile(filename, params):
    file = open(resultPath + filename[0] + "Deserilizer.h","w") 
    file.close()
    file = open(resultPath + filename[0] + "Deserilizer.cpp","w") 
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


