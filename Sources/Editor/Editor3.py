from NetworkUtils import NetworkClient

import tkinter as tk
from tkinter import ttk
import sys
# from tkinter import filedialog
from tkinter import messagebox

from lxml import objectify

import json
import pathlib

import subprocess

TCP_IP = 'localhost'
TCP_PORT = 1991

if len(sys.argv) > 1:
    print("Connecting to {0}".format(sys.argv[1]))
    TCP_IP=sys.argv[1]

networkClient = NetworkClient(TCP_IP, TCP_PORT)
gameObjects={}

def DoNothing():
    return

def GetObjectList():
    networkClient.SendCommand("getObjectList")
    count = 0
    while(True):
        msg = networkClient.RecevieMsg()
        print("RecevieMsg:")
        print(msg)
        print("===============")

        main = objectify.fromstring(msg)
        if main.tag == "TextMessage":
            if main.get("text") == "end":
                break;
        if main.tag == "GameObject":
            print("insert")
            goId = int(main.get("id"))
            parentId = int(main.get("parentId"))
            name = main.get("name")

            if parentId == 0:
                hwnd = tree.insert("", "end", None, text=name, values=("", "","--"))
                gameObjects[goId] = parentId, name, hwnd
            else:
                parentId, parentName, hwnd = gameObjects[parentId]
                hwnd = tree.insert(hwnd, "end", None, text=name, values=("", "","--"))
                gameObjects[goId] = parentId, name, hwnd

            count = count + 1

    print("Objects count : {0}".format(count))

def AskAndTryConnect(msg, func):
    if not networkClient.IsConnected():
        while True:
            answer = messagebox.askyesno(title="Error", message= msg)
            if answer:
                if func():
                    return True
            else:
                return False
    return False

def Connect():
    if networkClient.Connect():
        GetObjectList()
        return True
    return False

def OpenFile():
    if not AskAndTryConnect("System not connected. Do you want connect?", Connect):
        return

    try:
        initDir="/"
        if pathlib.Path("h.tmp").exists():
            file = open("h.tmp","r") 
            initDir = file.readline()[:-1]
            print(initDir)
            file.close()

        filename = filedialog.askopenfilename(initialdir = initDir,title = "Select file",filetypes = (("scene files","*.xml"),("all files","*.*")))
        if filename:
            file = open("h.tmp","w")
            file.write(str(pathlib.Path(filename).parent) + "/") 
            file.close()
            SendCommand("openFile " + filename);
    except:
        messagebox.showerror(title="Error", message=sys.exc_info()[0])

def Close():
    s.close()

def CreateVectorInput(rootFrame, label, startColumn, startRow):
    frame = tk.Frame(rootFrame, width=200, height=400)
    frame.grid(row=startRow, column=startColumn, padx=5, pady=0, sticky=(tk.W, tk.E))

    vframe = tk.LabelFrame(frame, text=label)
    vframe.grid(column=startColumn, row=startRow, padx=0, pady=0, ipadx=5,ipady=5, sticky=(tk.W), columnspan=1)

    tk.Label(vframe, text="X :").grid(row=0, column=0, padx=5, pady=0, ipadx=0)
    positionX = tk.Text(vframe, height=1, width=7)
    positionX.grid(column=1, row=0, sticky=(tk.W),)

    tk.Label(vframe, text="Y :").grid(row=0, column=2, padx=5, pady=0, ipadx=0)
    positionY = tk.Text(vframe, height=1, width=7)
    positionY.grid(column=3, row=0, sticky=(tk.W),)

    tk.Label(vframe, text="Z :").grid(row=0, column=4, padx=5, pady=0, ipadx=0)
    positionZ = tk.Text(vframe, height=1, width=7)
    positionZ.grid(column=5, row=0, sticky=(tk.W),)

    return positionX, positionY, positionZ

def OnSelectGameObject(event):
    print("OnSelectGameObject : {0}".format(event.widget.selection()))
    curItem = tree.focus()
    print("Focus : {0}".format(tree.item(curItem)))
    nameText.delete('1.0', tk.END)
    nameText.insert(tk.END, tree.item(curItem)['text'])

window = tk.Tk()
window.title("Editor")
window.geometry('620x750')

leftFrame = tk.LabelFrame(window, width=200, height=400, text="GameObjects")
leftFrame.grid(row=0, column=0, padx=5, pady=5)
rightFrame = tk.Frame(window, width=650, height=400)
rightFrame.grid(row=0, column=1, padx=5, pady=5, sticky=(tk.W, tk.N))

infoFrame = tk.LabelFrame(rightFrame, width=290, height=50, text="Info")
infoFrame.grid(row=0, column=0, padx=5, pady=5, sticky=(tk.W, tk.N))

tk.Label(infoFrame, text="Id : ").grid(row=0, column=0, padx=5, pady=0, ipadx=0, sticky=(tk.W))
tk.Label(infoFrame, text="-").grid(row=0, column=1, padx=5, pady=0, ipadx=0, sticky=(tk.W))
tk.Label(infoFrame, text="Name : ").grid(row=1, column=0, padx=5, pady=0, ipadx=0, sticky=(tk.W))
nameText = tk.Text(infoFrame, height=1, width=25)
nameText.grid(column=1, row=1, padx=7, pady=5)

positionX, positionY, positionZ = CreateVectorInput(rootFrame=rightFrame, label="Transform", startColumn=0, startRow=1)
rotX, rotY, rotZ = CreateVectorInput(rootFrame=rightFrame, label="Rotation", startColumn=0, startRow=2)
scaleX, scaleY, scaleZ = CreateVectorInput(rootFrame=rightFrame, label="Scale", startColumn=0, startRow=3)

componentsFrame = tk.LabelFrame(rightFrame, text="Components", width=250, height=550)
componentsFrame.grid(column=0, row=4, padx=0, pady=0, ipady=5)

btn = tk.Button(componentsFrame, text="Test componnent 1", command=DoNothing,width=36) 
btn.grid(column=0, row=0, padx=5, pady=0)
btn = tk.Button(componentsFrame, text="Test componnent 2", command=DoNothing,width=36) 
btn.grid(column=0, row=1, padx=5, pady=0)
btn = tk.Button(componentsFrame, text="Test componnent 3", command=DoNothing,width=36) 
btn.grid(column=0, row=2, padx=5, pady=0)
btn = tk.Button(componentsFrame, text="Test componnent 4", command=DoNothing,width=36) 
btn.grid(column=0, row=3, padx=5, pady=0)

btn = tk.Button(rightFrame, text="Add componnent", command=DoNothing,width=36) 
btn.grid(column=0, row=5, padx=5, pady=5)

# componentsTree=ttk.Treeview(rightFrame, height=20)
# componentsTree.grid(columnspan=3, row=3, pady=20)
# componentsTree.column("#0", width=250, minwidth=100)
# componentsTree.heading("#0",text="Components")
# #componentsTree.configure(background='black')

# btn = Button(window, text="Connect", command=connect) 
# btn.grid(column=0, row=0, sticky=(N, S, E, W))


tree=ttk.Treeview(leftFrame, height=35, show="tree")
tree.grid(columnspan=3, row=1, rowspan=37, padx=5, pady=5)
tree.column("#0", width=300, minwidth=100)
tree.bind('<<TreeviewSelect>>', OnSelectGameObject)

menubar = tk.Menu(window)
filemenu = tk.Menu(menubar, tearoff=0)
filemenu.add_command(label="Connect", command=Connect)
filemenu.add_command(label="New", command=DoNothing)
filemenu.add_command(label="Open", command=OpenFile)
filemenu.add_command(label="Save", command=DoNothing)
filemenu.add_command(label="Save as...", command=DoNothing)
filemenu.add_command(label="Close", command=Close)
menubar.add_cascade(label="File", menu=filemenu)
filemenu.add_separator()
filemenu.add_command(label="Exit", command=window.quit)

createMenu = tk.Menu(menubar, tearoff=0)
createMenu.add_command(label="Add game object", command=DoNothing)
menubar.add_cascade(label="Scene", menu=createMenu)

window.config(menu=menubar)

#pid=subprocess.Popen(["/home/bach/Projects/OpenGL_Engine/build/Editor"]).pid

window.mainloop()



#notes

#rightFrame.configure(background='black')


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