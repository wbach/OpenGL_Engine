import tkinter as tk
from tkinter import messagebox
from tkinter import filedialog
from tkinter import simpledialog

import os
import sys
import pathlib

class Menu:
    def __init__(self, networkClient, root, fileManger):
        self.root           = root
        self.networkClient  = networkClient
        self.fileManger = fileManger
        self.exitRequested  = False

        menubar = tk.Menu(root)
        filemenu = tk.Menu(menubar, tearoff=0)
        filemenu.add_command(label="Connect", command=self.networkClient.Connect)
        filemenu.add_command(label="Disconnect", command=self.networkClient.Disconnect)
        filemenu.add_command(label="New", command=self.DoNothing)
        filemenu.add_command(label="Open", command=self.fileManger.OpenSceneFile)
        filemenu.add_command(label="Save", command=self.fileManger.SaveSceneFile)
        menubar.add_cascade(label="File", menu=filemenu)
        filemenu.add_separator()
        filemenu.add_command(label="Exit", command=self.Exit)

        createMenu = tk.Menu(menubar, tearoff=0)
        createMenu.add_command(label="Add game object", command=self.AddGameObjct)
        createMenu.add_command(label="Add object with model", command=self.AddModel)
        createMenu.add_command(label="Load prefab", command=self.LoadPrefab)
        createMenu.add_command(label="Reload scene", command=self.ReloadScene)
        createMenu.add_command(label="Clear all game objects", command=self.ClearAllGameObjects)
        createMenu.add_command(label="Clear all", command=self.ClearAll)
        menubar.add_cascade(label="Scene", menu=createMenu)

        toolsMenu = tk.Menu(menubar, tearoff=0)
        toolsMenu.add_command(label="Terrain height painter", command=self.EnableTerrainHeightPainter)
        toolsMenu.add_command(label="Terrain texture painter", command=self.EnableTerrainTexturePainter)
        menubar.add_cascade(label="Tools", menu=toolsMenu)

        rendererMenu = tk.Menu(menubar, tearoff=0)
        rendererMenu.add_command(label="Reload shaders", command=self.ReloadShaders)
        rendererMenu.add_command(label="Take snapshot", command=self.TakeSnapshot)
        #rendererMenu.add_command(label="Change renderer", command=self.EnableTerrainTexturePainter)
        menubar.add_cascade(label="Renderer", menu=rendererMenu)

        root.config(menu=menubar)

        self.networkClient.SubscribeOnDisconnect(self.OnDisconnect)

    def TakeSnapshot(self):
        self.networkClient.SendCommand("takeSnapshot")

    def ReloadShaders(self):
        self.networkClient.SendCommand("reloadShaders")

    def EnableTerrainHeightPainter(self):
        self.networkClient.SendCommand("enableTerrainHeightPainter")

    def EnableTerrainTexturePainter(self):
        self.networkClient.SendCommand("enableTerrainTexturePainter")

    def ReloadScene(self):
        answer = messagebox.askyesno(title="Info", message="Do you really want reload scene?")
        if answer:
            self.networkClient.SendCommand("reloadScene")

    def ClearAllGameObjects(self):
        answer = messagebox.askyesno(title="Info", message="Do you really want delete all game objects in scene?")
        if answer:
            self.networkClient.SendCommand("clearAllGameObjects")

    def ClearAll(self):
        answer = messagebox.askyesno(title="Info", message="Do you really want clear all?")
        if answer:
            self.networkClient.SendCommand("clearAll")

    def Exit(self):
        if self.networkClient.IsConnected():
            self.networkClient.SendCommand("exit")
            self.exitRequested=True
        else:
            self.root.quit()

    def OnDisconnect(self):
        if self.exitRequested:
            self.root.quit()

    def AddGameObjct(self):
        answer = simpledialog.askstring("Input", "Game object name", parent=self.root, initialvalue="GameObject")
        self.networkClient.SendCommand("createGameObject name=" + answer)

    def AddModel(self):
        filename = self.fileManger.OpenModelFile()
        if filename:
            self.networkClient.SendCommand("createGameObjectWithModel filename=" + filename + " frontCamera=5")

    def LoadPrefab(self):
        filename = self.fileManger.OpenPrefabFile()
        if filename:
            self.networkClient.SendCommand("loadPrefab filename=" + filename)

    def DoNothing(self):
        return

    def Listen(self):
        print("Listen")
        self.root.after(100, self.Listen)