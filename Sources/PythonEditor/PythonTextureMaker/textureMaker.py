import os
import tkinter as tk
import sys
import math
from PIL import Image, ImageTk

class TextureMaker:
    def __init__(self):
        self.root = tk.Tk()
        self.mainFrame = tk.Frame(self.root, width=320, height=240)
        self.mainFrame.grid(row=0, column=1, padx=5, pady=5, sticky=(tk.W, tk.N))
        btn = tk.Button(self.mainFrame, text="CreateColorsPalet", command=self.CreateColorsPalet, width=30)
        btn.pack(padx=5, pady=5)

        self.root.mainloop()

    def CreateColorsPalet(self):
        boxRange = 5
        boxCount = int(255 / boxRange)
        boxSize = 20

        totalColor = boxCount * boxCount * boxCount
        size = math.ceil(math.sqrt(totalColor))

        print(boxCount)
        print(totalColor)
        print(size)

        image = Image.new('RGBA', (size, size))
        pixels = image.load()

        currentBox = 0
        r = 0
        g = 0
        b = 0
        a = 255

        x = 0
        y = 0
        for ir in range(boxCount):
            for ig in range(boxCount):
                for ib in range(boxCount):
                    r = int(ir / boxSize) * boxRange
                    g = int(ig / boxSize) * boxRange
                    b = int(ib / boxSize) * boxRange
                    pixels[x,y] = (r, g, b, a)

                    x = x + 1
                    if x >= image.width:
                        y = y + 1
                        x = 0


        # for i in range(image.size[0]):
        #     for j in range(image.size[1]):
        #         floor(i / boxSize)

        #             if r < 255:
        #                 r = r + boxRange
        #             elif g < 255:
        #                 g = g + boxRange
        #             elif b < 255:
        #                 b = b + boxRange


        #           #  print(i, j)
        #             pixels[i,j] = (r, g, b, a)

        image.save("image.png", "PNG")



def CreateColorsPalet():
    boxRange = 50
    boxCount = int(250 / boxRange) + 1

    totalColor = boxCount * boxCount
    totalColor2 = boxCount * boxCount * boxCount
    size = math.ceil(math.sqrt(totalColor))

    print(boxCount)
    print(totalColor)
    print(size)

    image = Image.new('RGBA', (boxCount, boxCount * boxCount))
    pixels = image.load()

    colors = list()

    for ib in range(boxCount):
        for ig in range(boxCount):
            for ir in range(boxCount):
                r = ir * boxRange
                g = ig * boxRange
                b = ib * boxRange
                a = 255
                print((r, g, b, a))
                colors.append((r, g, b, a))
                pixels[ir, ig + boxCount*ib] = (r, g, b, a)

    # boxId = 0
    # for k in range(boxCount):
    #     for i in range(boxCount):
    #         for j in range(boxCount):
    #                 pixels[j, i + boxCount*k] = colors[boxId]
    #                 if boxId < totalColor2 - 1:
    #                     boxId = boxId + 1
        #return

    image.save("image.png", "PNG")

if __name__ == "__main__":
    CreateColorsPalet()
