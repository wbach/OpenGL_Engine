import tkinter as tk
from tkinter import messagebox
from tkinter import filedialog

import os
import sys
import pathlib

from CommonWidgetTools import AskAndTryConnect

class Menu:
    def __init__(self, networkClient, root, gameObjectView):
        self.root           = root
        self.networkClient  = networkClient
        self.gameObjectView = gameObjectView
        self.sceneFileName = ""
        self.histTmpFile = "history.autosave"

        menubar = tk.Menu(root)
        filemenu = tk.Menu(menubar, tearoff=0)
        filemenu.add_command(label="Connect", command=self.Connect)
        filemenu.add_command(label="Disconnect", command=self.networkClient.Disconnect)
        filemenu.add_command(label="New", command=self.DoNothing)
        filemenu.add_command(label="Open", command=self.OpenFile)
        filemenu.add_command(label="Save", command=self.SaveFile)
        menubar.add_cascade(label="File", menu=filemenu)
        filemenu.add_separator()
        filemenu.add_command(label="Exit", command=self.root.quit)

        createMenu = tk.Menu(menubar, tearoff=0)
        createMenu.add_command(label="Add game object", command=self.DoNothing)
        menubar.add_cascade(label="Scene", menu=createMenu)
        root.config(menu=menubar)

        self.networkClient.SubscribeOnMessage("SceneFileMsg", self.OnSceneFileMsg)

    def OnSceneFileMsg(self, msg):
        self.sceneFileName = msg.get("filename")
        if filename:
            file = open(self.histTmpFile, "w")
            file.write(self.sceneFileName)
            file.close()
            self.networkClient.SendCommand("saveFile " + filename)

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

    def GetInitDir(self):
        if self.sceneFileName:
            return os.path.dirname(self.sceneFileName)
        if pathlib.Path(self.histTmpFile).exists():
            file = open(self.histTmpFile, "r")
            dir = file.readline()[:-1]
            file.close()
            return dir
        else:
            return "/"

    def GetInitFilename(self, default):
        if self.sceneFileName:
            return os.path.basename(self.sceneFileName)
        return default

    def WriteToHistFile(self, filename):
            file = open(self.histTmpFile, "w")
            file.write(os.path.dirname(filename) + "/")
            file.close()

    def OpenFile(self):
        if not AskAndTryConnect(self.networkClient, "System not connected. Do you want connect?", self.Connect):
            return

        try:
            initDir = self.GetInitDir()
            filename = filedialog.askopenfilename(initialdir=initDir, initialfile=self.GetInitFilename(""), title="Select file", filetypes=(("scene files","*.xml"),("all files","*.*")))
            if filename:
                self.networkClient.SendCommand("openFile filename=" + filename)
                self.WriteToHistFile(filename)
        except:
            messagebox.showerror(title="Error", message=sys.exc_info())

    def SaveFile(self):
        if not AskAndTryConnect(self.networkClient, "System not connected. Do you want connect?", self.Connect):
            return

        try:
            initDir = self.GetInitDir()
            filename = filedialog.asksaveasfilename(initialdir=initDir, initialfile=self.GetInitFilename("newScene.xml"), title="Select file", filetypes=(("scene files","*.xml"),("all files","*.*")))
            if filename:
                self.networkClient.SendCommand("saveToFile filename=" + filename)
                self.WriteToHistFile(filename)
        except:
            messagebox.showerror(title="Error", message=sys.exc_info())