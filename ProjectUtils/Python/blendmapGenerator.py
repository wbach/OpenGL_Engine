import cv2
import time
import numpy as np
import sys
from Utils import calculateNormal

import time
from tqdm import tqdm


input=sys.argv[1] #'/home/baszek/tmp/World_ref.png'
print( "Input arg: " + input)
output=sys.argv[2]#'/home/baszek/tmp/BlendMap.png'

img = cv2.imread(input)

upVector=[0,1,0]

def heightBase(img):
    height, width, channels = img.shape

    heightFactor=0
    maxValue=0
    minValue=255
    for y in range(height):
        for x in range(width):
            px = img[x, y]
            heightFactor=(int(px[0])+int(px[1])+int(px[2])) / 3
            if heightFactor > maxValue:
                maxValue=heightFactor
            if heightFactor < minValue:
                minValue = heightFactor

    print('Max value :' + str(maxValue))
    print('Min value :' + str(minValue))

    step1 = maxValue / 3
    step2 = 2 * step1

    #resultImg=img
    resultImg = np.zeros((height,width,channels), np.uint8)

    # x = 0
    # y = 0
    # total = height * width
    # for i in tqdm(range(total - 1)):
    # # for y in range(height):
    # #     for x in range(width):
    #         x = x + 1

    #         if x > width - 1:
    #             x = 0
    #             y = y + 1
            
    #         if (y < height / 2):
    #             resultImg[x, y] = 2*y
    #         else:
    #             resultImg[x, y] = height - (2*y)

    # cv2.imwrite(output, resultImg)
    # return 0

    x = 0
    y = 0
    total = height * width
    for i in tqdm(range(total - 1)):
    # for y in range(height):
    #     for x in range(width):
            x = x + 1

            if x > width - 1:
                x = 0
                y = y + 1
            
            px = img[x, y]
            heightFactor=(int(px[0])+int(px[1])+int(px[2])) / 3

            normal = calculateNormal(img, [x, y])
            product= abs(np.dot(normal, upVector))

            #product2=np.dot(upVector, normal)
#            print normal
            #print [product, product2]

            # colorNormal = [0,0,0]
            # colorNormal[0]=normal[0]*255
            # colorNormal[1]=normal[1]*255
            # colorNormal[2]=normal[2]*255

            value=int(255.0 * (1.0 - product))
            resultImg[x, y] = [0, value , 0]

            # if product < 0.30:
            #     resultImg[x, y] = [255, 0, 0]
            # elif product < 0.60:
            #     resultImg[x, y] = [0, 255, 0]
            # else:
            #     resultImg[x, y] = [0, 0, 255]


            # if heightFactor < step1:
            #     resultImg[x, y] = [255, 0, 0]
            # elif heightFactor < step2:
            #     resultImg[x, y] = [0, 255, 0]
            # else :
            #     resultImg[x, y] = [0, 0, 255]

    kernel = np.ones((5,5),np.float32)/25
    dst = cv2.filter2D(resultImg,-1,kernel)
    cv2.imwrite(output, dst)

heightBase(img)