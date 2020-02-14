import tkinter as tk
from tkinter import messagebox
from tkinter import filedialog

import sys
import pathlib

from CommonWidgetTools import AskAndTryConnect

class Menu:
    def __init__(self, networkClient, root, gameObjectView):
        self.root           = root
        self.networkClient  = networkClient
        self.gameObjectView = gameObjectView

        menubar = tk.Menu(root)
        filemenu = tk.Menu(menubar, tearoff=0)
        filemenu.add_command(label="Connect", command=self.Connect)
        filemenu.add_command(label="Disconnect", command=self.networkClient.Disconnect)
        filemenu.add_command(label="New", command=self.DoNothing)
        filemenu.add_command(label="Open", command=self.OpenFile)
        filemenu.add_command(label="Save", command=self.DoNothing)
        filemenu.add_command(label="Save as...", command=self.DoNothing)
        menubar.add_cascade(label="File", menu=filemenu)
        filemenu.add_separator()
        filemenu.add_command(label="Exit", command=self.root.quit)

        createMenu = tk.Menu(menubar, tearoff=0)
        createMenu.add_command(label="Add game object", command=self.DoNothing)
        menubar.add_cascade(label="Scene", menu=createMenu)

        root.config(menu=menubar)

    def DoNothing(self):
        return

    def Listen(self):
        print("Listen")
        self.root.after(100, self.Listen)

    def Connect(self):
        if self.networkClient.Connect():
            self.gameObjectView.GetObjectList()
            self.networkClient.SendCommand("getComponentList")
            #window.after(1000, Listen)
            return True
        return False

    def OpenFile(self):
        if not AskAndTryConnect(self.networkClient, "System not connected. Do you want connect?", self.Connect):
            return

        try:
            initDir="/"
            if pathlib.Path("h.tmp").exists():
                file = open("h.tmp","r")
                initDir = file.readline()[:-1]
                print(initDir)
                file.close()

            filename = filedialog.askopenfilename(initialdir=initDir, title="Select file", filetypes=(("scene files","*.xml"),("all files","*.*")))
            if filename:
                file = open("h.tmp","w")
                file.write(str(pathlib.Path(filename).parent) + "/")
                file.close()
                self.networkClient.SendCommand("openFile " + filename);
        except:
            messagebox.showerror(title="Error", message=sys.exc_info())
