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
        filemenu.add_command(label="Open", command=self.OpenSceneFile)
        filemenu.add_command(label="Save", command=self.SaveFile)
        menubar.add_cascade(label="File", menu=filemenu)
        filemenu.add_separator()
        filemenu.add_command(label="Exit", command=self.Exit)

        createMenu = tk.Menu(menubar, tearoff=0)
        createMenu.add_command(label="Add game object", command=self.AddGameObjct)
        createMenu.add_command(label="Add object with model", command=self.AddModel)
        createMenu.add_command(label="Load prefab", command=self.LoadPrefab)
        menubar.add_cascade(label="Scene", menu=createMenu)
        root.config(menu=menubar)

        self.networkClient.SubscribeOnMessage("SceneFileMsg", self.OnSceneFileMsg)

    def Exit(self):
        self.networkClient.SendCommand("exit")
        self.root.quit()

    def OnSceneFileMsg(self, msg):
        self.sceneFileName = msg.get("filename")
        if filename:
            file = open(self.histTmpFile, "w")
            file.write(self.sceneFileName)
            file.close()
            self.networkClient.SendCommand("saveFile " + filename)

    def AddGameObjct(self):
        self.networkClient.SendCommand("createGameObject")
        return

    def AddModel(self):
        filename = self.OpenFile("", (("3DModel files","*.obj"), ("3DModel files","*.fbx"), ("3DModel files","*.dae"), ("3DModel files","*.terrain")))
        if filename:
            self.networkClient.SendCommand("createGameObjectWithModel filename=" + filename + " frontCamera=5")

    def LoadPrefab(self):
        filename = self.OpenFile("", (("prefab files","*.xml"),("all files","*.*")))
        if filename:
            self.networkClient.SendCommand("loadPrefab filename=" + filename)

    def DoNothing(self):
        return

    def Listen(self):
        print("Listen")
        self.root.after(100, self.Listen)

    def Connect(self):
        if self.networkClient.Connect():
            self.gameObjectView.GetObjectList()
            self.networkClient.SendCommand("getComponentList")
            self.networkClient.SendCommand("getRunningStatus")
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

    def OpenSceneFile(self):
        filename = self.OpenFile(self.GetInitFilename(""), (("scene files","*.xml"),("all files","*.*")))
        if filename:
            self.networkClient.SendCommand("openFile filename=" + filename)

    def OpenFile(self, intialFileName, fileTypes):
        if not AskAndTryConnect(self.networkClient, "System not connected. Do you want connect?", self.Connect):
            return

        try:
            initDir = self.GetInitDir()
            filename = filedialog.askopenfilename(initialdir=initDir, initialfile=intialFileName, title="Select file", filetypes=fileTypes)
            if filename:
                self.WriteToHistFile(filename)
            return filename
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