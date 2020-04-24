import tkinter as tk
from tkinter import messagebox
from tkinter import filedialog
from tkinter import simpledialog

import os
import sys
import pathlib

from CommonWidgetTools import AskAndTryConnect

class FileManager:
    def __init__(self, networkClient):
        self.networkClient = networkClient
        self.sceneFileName = ""
        self.sceneFileName = ""
        self.histTmpFile   = "history.autosave"

        self.networkClient.SubscribeOnMessage("SceneFileMsg", self.OnSceneFileMsg)

    def OnSceneFileMsg(self, msg):
        self.sceneFileName = msg.get("filename")
        if self.sceneFileName:
            file = open(self.histTmpFile, "w")
            file.write(self.sceneFileName)
            file.close()

    def GetInitFilename(self, default):
        if self.sceneFileName:
            return os.path.basename(self.sceneFileName)
        return default

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

    def WriteToHistFile(self, filename):
        file = open(self.histTmpFile, "w")
        file.write(os.path.dirname(filename) + "/")
        file.close()

    def OpenFile(self, intialFileName, fileTypes):
        if not AskAndTryConnect(self.networkClient):
            return ""

        try:
            initDir = self.GetInitDir()
            filename = filedialog.askopenfilename(initialdir=initDir, initialfile=intialFileName, title="Select file",
                                                  filetypes=fileTypes)
            if filename:
                self.WriteToHistFile(filename)
            return filename
        except:
            messagebox.showerror(title="Error", message=sys.exc_info())
        return ""

    def OpenAllTypesFile(self):
        return self.OpenFile(self.GetInitFilename(""), (("3DModel files","*.obj"), ("3DModel files","*.fbx"), ("3DModel files","*.dae"), ("3DModel files","*.terrain")))

    def OpenSceneFile(self):
        filename = self.OpenFile(self.GetInitFilename(""), (("scene files","*.xml"),("all files","*.*")))
        if filename:
            self.networkClient.SendCommand("openFile filename=" + filename)

    def OpenModelFile(self, parentId=-1):
        if not AskAndTryConnect(self.networkClient):
            return ""
        return self.OpenFile("", (("3DModel files","*.obj"), ("3DModel files","*.fbx"), ("3DModel files","*.dae"), ("3DModel files","*.terrain")))

    def OpenPrefabFile(self):
        return self.OpenFile("", (("prefab files","*.xml"),("all files","*.*")))

    def SaveFile(self, initialFile, fileTypes):
        if not AskAndTryConnect(self.networkClient):
            return ""
        try:
            initDir = self.GetInitDir()
            filename = filedialog.asksaveasfilename(initialdir=initDir, initialfile=initialFile, title="Select file", filetypes=fileTypes)
            if filename:
                self.WriteToHistFile(filename)
                return filename
        except:
            messagebox.showerror(title="Error", message=sys.exc_info())

        return ""

    def SaveSceneFile(self):
        filename = self.SaveFile(self.GetInitFilename("newScene.xml"), (("scene files", "*.xml"), ("all files", "*.*")))
        if filename:
            self.networkClient.SendCommand("saveToFile filename=" + filename)
