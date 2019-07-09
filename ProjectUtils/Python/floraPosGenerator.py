import numpy as np
import matplotlib.pyplot as plt

import sys
import random

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
plt.figure(figsize=(10*width_ratio,10))
plt.scatter(coords[:,0], coords[:,1], s=3)
plt.show()



# f = open(sys.argv[1], "a")
# sizeX=sys.argv[2]
# sizeY=sys.argv[3]

# f.write("Now the file has more content!")


# for y in range(506):
#     for x in range(506):
#         f.write('x : ' + str(x) + ' y : ' + str(y) + ' v : ' + str(coords[x, y])  + '\n')


# f.close()
# print("Random float number is ", random.random())