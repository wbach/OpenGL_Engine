import numpy as np
import matplotlib.pyplot as plt

import sys
import random

import cv2

HEIGHT_FACTOR = 800.0

def getHeight(img, x, y):
    px = img[x, y]
    #heightFactor = (int(px[0])+int(px[1])+int(px[2])) / 3
    height = float(px[0]) / 255.0 * HEIGHT_FACTOR
    height = height - (HEIGHT_FACTOR / 2.0)
    height = height + 19.0
    return height

# specify params
n = 500
shape = np.array([4096, 4096])
sensitivity = 0.8 # 0 means no movement, 1 means max distance is init_dist

# compute grid shape based on number of points
width_ratio = shape[1] / shape[0]
num_y = np.int32(np.sqrt(n / width_ratio)) + 1
num_x = np.int32(n / num_y) + 1

# create regularly spaced neurons
x = np.linspace(0., shape[1]-1, num_x, dtype=np.float32)
y = np.linspace(0., shape[0]-1, num_y, dtype=np.float32)
coords = np.stack(np.meshgrid(x, y), -1).reshape(-1,2)

# compute spacing
init_dist = np.min((x[1]-x[0], y[1]-y[0]))
min_dist = init_dist * (1 - sensitivity)

assert init_dist >= min_dist
print(min_dist)

# perturb points
max_movement = (init_dist - min_dist)/2
noise = np.random.uniform(
    low=-max_movement,
    high=max_movement,
    size=(len(coords), 2))
coords += noise

print(len(coords))

# plot
#plt.figure(figsize=(10*width_ratio,10))
#plt.scatter(coords[:,0], coords[:,1], s=3)
#plt.show()

input=sys.argv[1]
img = cv2.imread(input)

f = open(sys.argv[2], "w")

f.write('<prefab>\n')
f.write('   <worldTransform>\n')
f.write('   <position x="0.000000" y="0.000000" z="10.000000"/>\n')
f.write('   <rotation x="0.000000" y="0.000000" z="0.000000"/>\n')
f.write('   <scale x="1.800000" y="1.800000" z="1.800000"/>\n')
f.write('   <isDynamic>false</isDynamic>\n')
f.write('   </worldTransform>\n')
f.write('   <components count="1">\n')
f.write('       <component type="12">\n')
f.write('           <textureFileName>Textures/Plants/G3_Nature_Plant_Grass_06_Diffuse_01.png</textureFileName>\n')
f.write('           <positions>\n')
for y in range(len(coords)):
    for x in range(len(coords)):
        f.write(str(coords[x, 0]))
        f.write(' ')
        #f.write('0')
        f.write(str(getHeight(img, x, y)))
        f.write(' ')
        f.write(str(coords[y, 1]))
        f.write(' ')
        # f.write('               <v>' + str(coords[x, 0]) + '</v>\n')
        # f.write('               <v>' + str(0) + '</v>\n')
        # f.write('               <v>' + str(coords[y, 1]) + '</v>\n')
f.write('           </positions>\n')
f.write('       </components>\n')
f.write('   </component>\n')
f.write('</prefab>')
f.close()
