import tkinter as tk

def CreateVectorInput(rootFrame, label, startColumn, startRow, keyFunc):
    frame = tk.Frame(rootFrame, width=200, height=400)
    frame.grid(row=startRow, column=startColumn, padx=5, pady=0, sticky=(tk.W, tk.E))

    vframe = tk.LabelFrame(frame, text=label)
    vframe.grid(column=startColumn, row=startRow, padx=0, pady=0, ipadx=5,ipady=5, sticky=(tk.W), columnspan=1)

    tk.Label(vframe, text="X :").grid(row=0, column=0, padx=5, pady=0, ipadx=0)
    editBox_X = tk.Text(vframe, height=1, width=7)
    editBox_X.grid(column=1, row=0, sticky=(tk.W))
    editBox_X.bind('<Key>', keyFunc)

    tk.Label(vframe, text="Y :").grid(row=0, column=2, padx=5, pady=0, ipadx=0)
    editBox_Y = tk.Text(vframe, height=1, width=7)
    editBox_Y.grid(column=3, row=0, sticky=(tk.W))
    editBox_Y.bind('<Key>', keyFunc)

    tk.Label(vframe, text="Z :").grid(row=0, column=4, padx=5, pady=0, ipadx=0)
    editBox_Z = tk.Text(vframe, height=1, width=7)
    editBox_Z.grid(column=5, row=0, sticky=(tk.W))
    editBox_Z.bind('<Key>', keyFunc)

    return editBox_X, editBox_Y, editBox_Z



#pid=subprocess.Popen(["/home/bach/Projects/OpenGL_Engine/build/Editor"]).pid

#notes

#rightFrame.configure(background='black')

# componentsTree=ttk.Treeview(rightFrame, height=20)
# componentsTree.grid(columnspan=3, row=3, pady=20)
# componentsTree.column("#0", width=250, minwidth=100)
# componentsTree.heading("#0",text="Components")
# #componentsTree.configure(background='black')

# btn = Button(window, text="Connect", command=connect)
# btn.grid(column=0, row=0, sticky=(N, S, E, W))

#frameTransform = tk.Frame(rightFrame, width=200, height=400)
#frameTransform.grid(row=1, column=0, padx=5, pady=5, sticky=(tk.W, tk.N))
#rightFrame.configure(background='green')

#tk.Label(rightFrame, text="Transform").grid(column=0, row=0, padx=5, pady=3, ipadx=0)

# tk.Label(rightFrame, text="Rotation").grid(column=0, row=2, padx=5, pady=3, ipadx=0)
# rotX, rotY, rotZ = CreateVectorInput(frame=frameTransform, startColumn=0, startRow=3)

# tk.Label(rightFrame, text="Scale").grid(column=0, row=4, padx=5, pady=3, ipadx=0)
# scaleX, scaleY, scaleZ = CreateVectorInput(frame=frameTransform, startColumn=0, startRow=5)

# positionY = Text(window, height=1, width=8)
# positionY.grid(column=4, row=1)
# positionZ = Text(window, height=1, width=8)
# positionZ.grid(column=5, row=1)