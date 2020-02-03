from NetworkUtils import NetworkClient
from tkinter import *
from tkinter.ttk import *
from tkinter import filedialog
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
            tree.insert("", "end", None, text=main.get("name"), values=("", "","--"))
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
    networkClient.Connect()
    GetObjectList()

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

window = Tk()
window.title("Editor")
window.geometry('405x800')

tree=Treeview(window)
tree.grid(columnspan=3, row=1)
tree["columns"]=("one","two","three")
tree.column("#0", width=100, minwidth=100)
tree.column("one", width=100, minwidth=100)
tree.column("two", width=100, minwidth=100)
tree.column("three", width=100, minwidth=100)

tree.heading("#0",text="Name")
tree.heading("one", text="Date modified")
tree.heading("two", text="Type")
tree.heading("three", text="Size")

# Level 1
#folder1=tree.insert("", 1, None, text="Folder 1", values=("23-Jun-17 11:05","File folder",""))
# tree.insert("", 2, "", text="text_file.txt", values=("23-Jun-17 11:25","TXT file","1 KB"))
# # Level 2
#tree.insert(folder1, "end", None, text="photo1.png", values=("23-Jun-17 11:28","PNG file","2.6 KB"))

# tree.insert(folder1, "end", "", text="photo2.png", values=("23-Jun-17 11:29","PNG file","3.2 KB"))
# tree.insert(folder1, "end", "", text="photo3.png", values=("23-Jun-17 11:30","PNG file","3.1 KB"))

# btn = Button(window, text="Connect", command=connect) 
# btn.grid(column=0, row=0, sticky=(N, S, E, W))

menubar = Menu(window)
filemenu = Menu(menubar, tearoff=0)
filemenu.add_command(label="Connect", command=Connect)
filemenu.add_command(label="New", command=DoNothing)
filemenu.add_command(label="Open", command=OpenFile)
filemenu.add_command(label="Save", command=DoNothing)
filemenu.add_command(label="Save as...", command=DoNothing)
filemenu.add_command(label="Close", command=Close)
menubar.add_cascade(label="File", menu=filemenu)
filemenu.add_separator()
filemenu.add_command(label="Exit", command=window.quit)
window.config(menu=menubar)

#pid=subprocess.Popen(["/home/bach/Projects/OpenGL_Engine/build/Editor"]).pid

window.mainloop()
