import numpy as np
from sklearn import preprocessing
# def normalize(value):
#     return value

def texture(heightmap, texCoord):
    #print texCoord
    px=heightmap[texCoord[0], texCoord[1]]
    result=(float(px[0])+float(px[1])+float(px[2])) / 3
    return result

def distance(v1, v2):
    return numpy.linalg.norm(v1-v2)

def sum(textCoord, offset):
    textCoord[0]=textCoord[0] + offset[0]
    textCoord[1]=textCoord[1] + offset[1]
    return textCoord

def calculateNormal(heightmap, texCoord):

    # z0 -- z1 -- z2
    # |     |     |
    # z3 -- h  -- z4
    # |     |     |
    # z5 -- z6 -- z7
    HEIGHT_MAP_SIZE=513
    texelSize=1

    h=texture(heightmap, texCoord)
    z0 = texture(heightmap, sum(texCoord, [-texelSize,-texelSize]))
    z1 = texture(heightmap, sum(texCoord, [0,-texelSize]))
    z2 = texture(heightmap, sum(texCoord, [texelSize,-texelSize]))
    z3 = texture(heightmap, sum(texCoord, [-texelSize, 0]))
    z4 = texture(heightmap, sum(texCoord, [texelSize, 0]))
    z5 = texture(heightmap, sum(texCoord, [-texelSize,texelSize]))
    z6 = texture(heightmap, sum(texCoord, [0,texelSize]))
    z7 = texture(heightmap, sum(texCoord, [texelSize,texelSize]))

    if texCoord[0]==0:
        z3=h

    if texCoord[1]==0:
        z1=h

    if texCoord[0]==0 and texCoord[1]==0:
        z0=h

    normal=[0.0,0.0,0.0]
    normalStrength=4
    yoffset=0.0
    #Sobel Filter
    # normal[0]=z0 + 2*z3 + z5 - z2 - 2*z4 - z7
    # normal[1]=z0 + 2*z1 + z2 -z5 - 2*z6 - z7 + yoffset
    # normal[2]=1.0/normalStrength

    # normal[0]=(2*(z4-z3))/4
    # normal[1]=(2*(z6-z1))/4
    # normal[2]=-4

    #glm::vec3 normal(heightL - heightR, 2.0f, heightD - heightU);

    normal[0]=z3-z4
    normal[1]=2
    normal[2]=z6-z1

    #return (normalize(normal)+1.0)/2.0
    X = np.asarray([normal], dtype=np.float)
    normalizedNormal=preprocessing.normalize(X, norm='l2')[0] #normalize(normal)
    # normalizedNormal[0]=(normalizedNormal[0]+1)/2
    # normalizedNormal[1]=(normalizedNormal[1]+1)/2
    # normalizedNormal[2]=(normalizedNormal[2]+1)/2
    return normalizedNormal


  