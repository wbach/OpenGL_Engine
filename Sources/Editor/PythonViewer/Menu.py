import tkinter as tk
from functools import partial
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
        self.currentSceneFileName = ""

        menubar = tk.Menu(root)
        filemenu = tk.Menu(menubar, tearoff=0)
        filemenu.add_command(label="Connect", command=self.networkClient.Connect)
        filemenu.add_command(label="Disconnect", command=self.networkClient.Disconnect)
        filemenu.add_command(label="New", command=self.DoNothing)
        filemenu.add_command(label="Open", command=self.fileManger.OpenSceneFile)
        filemenu.add_command(label="Save", command=self.SaveSceneFile)
        filemenu.add_command(label="SaveAs", command=self.fileManger.SaveSceneFile)
        menubar.add_cascade(label="File", menu=filemenu)
        filemenu.add_separator()
        filemenu.add_command(label="Exit", command=self.Exit)

        createMenu = tk.Menu(menubar, tearoff=0)
        createMenu.add_command(label="Add game object", command=self.AddGameObjct)
        createMenu.add_command(label="[TEST] PreviewGameObject", command=self.PreviewGameObject)
        createMenu.add_command(label="Add object with model", command=self.AddModel)
        createMenu.add_command(label="Load prefab", command=self.LoadPrefab)
        createMenu.add_command(label="Reload scene", command=self.ReloadScene)
        createMenu.add_command(label="Clear all game objects", command=self.ClearAllGameObjects)
        createMenu.add_command(label="Clear all", command=self.ClearAll)
        menubar.add_cascade(label="Scene", menu=createMenu)

        toolsMenu = tk.Menu(menubar, tearoff=0)
        toolsMenu.add_command(label="Generate terrain", command=self.GenerateTerrain)
        toolsMenu.add_command(label="Terrain height painter", command=self.EnableTerrainHeightPainter)
        toolsMenu.add_command(label="Terrain texture painter", command=self.EnableTerrainTexturePainter)
        menubar.add_cascade(label="Tools", menu=toolsMenu)

        rendererMenu = tk.Menu(menubar, tearoff=0)
        rendererMenu.add_command(label="Reload shaders", command=self.ReloadShaders)
        rendererMenu.add_command(label="Take snapshot", command=self.TakeSnapshot)
        #rendererMenu.add_command(label="Change renderer", command=self.EnableTerrainTexturePainter)
        menubar.add_cascade(label="Renderer", menu=rendererMenu)

        self.physicsVisual = tk.BooleanVar()
        self.physicsVisual.set(False)
        self.physicsVisual.trace("w", self.PhysicsVisualChange)
        self.physicsVisualAllObjects = tk.BooleanVar()
        self.physicsVisualAllObjects.set(False)
        self.physicsVisualAllObjects.trace("w", self.PhysicsVisualAllObjectsChange)
        self.normalsVisual = tk.BooleanVar()
        self.normalsVisual.set(False)
        self.normalsVisual.trace("w", self.NormalsVisualChange)

        debugMenu = tk.Menu(menubar, tearoff=0)
        debugMenu.add_command(label="Swap Line render mode", command=lambda: self.networkClient.SendCommand("setLineRenderMode"))
        debugMenu.add_checkbutton(label="Physics Visualization", onvalue=1, offvalue=0, variable=self.physicsVisual)
        debugMenu.add_checkbutton(label="Physics Visualization all objects", onvalue=1, offvalue=0, variable=self.physicsVisualAllObjects)
        debugMenu.add_checkbutton(label="Normals Visualization", onvalue=1, offvalue=0, variable=self.normalsVisual)
        menubar.add_cascade(label="Debug", menu=debugMenu)

        self.texturesControlDiffuse = tk.BooleanVar()
        self.texturesControlDiffuse.set(True)
        self.texturesControlDiffuse.trace("w", self.TexturesControlDiffuseChange)
        self.texturesControlNormals = tk.BooleanVar()
        self.texturesControlNormals.set(True)
        self.texturesControlNormals.trace("w", self.TexturesControlNormalsChange)
        self.texturesControlSpecular = tk.BooleanVar()
        self.texturesControlSpecular.set(True)
        self.texturesControlSpecular.trace("w", self.TexturesControlSpecularChange)
        self.texturesControlDisplacement = tk.BooleanVar()
        self.texturesControlDisplacement.set(True)
        self.texturesControlDisplacement.trace("w", self.TexturesControlDisplacementChange)

        texturesControl = tk.Menu(menubar, tearoff=0)
        texturesControl.add_checkbutton(label="Diffuse", onvalue=1, offvalue=0, variable=self.texturesControlDiffuse)
        texturesControl.add_checkbutton(label="Normals", onvalue=1, offvalue=0, variable=self.texturesControlNormals)
        texturesControl.add_checkbutton(label="Specular", onvalue=1, offvalue=0, variable=self.texturesControlSpecular)
        texturesControl.add_checkbutton(label="Displacement", onvalue=1, offvalue=0, variable=self.texturesControlDisplacement)
        debugMenu.add_cascade(label="Textures", menu=texturesControl)

        root.config(menu=menubar)

        self.networkClient.SubscribeOnDisconnect(self.OnDisconnect)
        self.networkClient.SubscribeOnMessage("SceneFileMsg", self.OnSceneFileMsg)

        self.bias = tk.StringVar()
        self.bias.set("2.0")
        self.octaves = tk.StringVar()
        self.octaves.set("9")
        self.width = tk.StringVar()
        self.width.set("512")
        self.scale = tk.StringVar()
        self.scale.set("10.0")
        self.heightFactor = tk.StringVar()
        self.heightFactor.set("20.0")

    def GenerateTerrain(self):
        if self.networkClient.IsConnected():
            dialog = tk.Toplevel(self.root)
            dialog.title("Terran generate view")

            biasLabel = tk.LabelFrame(dialog, text="Bias")
            biasLabel.pack(fill=tk.X)
            biasInput = tk.Entry(biasLabel, textvariable=self.bias)
            biasInput.pack(fill=tk.X, expand=1)

            octavesLabel = tk.LabelFrame(dialog, text="Octaves")
            octavesLabel.pack(fill=tk.X)
            octavesLabel = tk.Entry(octavesLabel, textvariable=self.octaves)
            octavesLabel.pack(fill=tk.X, expand=1)

            widthLabel = tk.LabelFrame(dialog, text="Width")
            widthLabel.pack(fill=tk.X)
            widthLabel = tk.Entry(widthLabel, textvariable=self.width)
            widthLabel.pack(fill=tk.X, expand=1)

            tk.Button(dialog, text="Generate with the same seed", command=partial(self.SendGenerateTerrain, "false"))\
                .pack(fill=tk.X)
            tk.Button(dialog, text="Generate terrain with new seed", command=partial(self.SendGenerateTerrain, "true"))\
                .pack(fill=tk.X)

    def SendGenerateTerrain(self, updateNoiseSeed):
        if self.networkClient.IsConnected():
            self.networkClient.SendCommand("generateTerrains width=" + self.width.get() + " height=" +
                                           self.width.get() + " octaves=" + self.octaves.get() + " bias=" +
                                           self.bias.get() + " updateNoiseSeed=" + updateNoiseSeed)

    def TexturesControlDiffuseChange(self, *args):
        if self.networkClient.IsConnected():
            if self.texturesControlDiffuse.get():
                self.networkClient.SendCommand("controlTextureUsage enabled=true textureType=diffuse")
            else:
                self.networkClient.SendCommand("controlTextureUsage enabled=false textureType=diffuse")

    def TexturesControlNormalsChange(self, *args):
        if self.networkClient.IsConnected():
            if self.texturesControlNormals.get():
                self.networkClient.SendCommand("controlTextureUsage enabled=true textureType=normal")
            else:
                self.networkClient.SendCommand("controlTextureUsage enabled=false textureType=normal")
    def TexturesControlSpecularChange(self, *args):
        if self.networkClient.IsConnected():
            if self.texturesControlSpecular.get():
                self.networkClient.SendCommand("controlTextureUsage enabled=true textureType=specular")
            else:
                self.networkClient.SendCommand("controlTextureUsage enabled=false textureType=specular")
    def TexturesControlDisplacementChange(self, *args):
        if self.networkClient.IsConnected():
            if self.texturesControlDisplacement.get():
                self.networkClient.SendCommand("controlTextureUsage enabled=true textureType=displacement")
            else:
                self.networkClient.SendCommand("controlTextureUsage enabled=false textureType=displacement")
    def PhysicsVisualChange(self, *args):
        if self.networkClient.IsConnected():
            if self.physicsVisual.get():
                self.networkClient.SendCommand("setPhysicsVisualization enabled=true")
            else:
                self.networkClient.SendCommand("setPhysicsVisualization enabled=false")

    def PhysicsVisualAllObjectsChange(self, *args):
        if self.networkClient.IsConnected():
            if not self.physicsVisual:
                self.physicsVisual.set(True)
            if self.physicsVisualAllObjects.get():
                self.networkClient.SendCommand("setPhysicsVisualizationAllObjects enabled=true")
            else:
                self.networkClient.SendCommand("setPhysicsVisualizationAllObjects enabled=false")

    def NormalsVisualChange(self, *args):
        if self.networkClient.IsConnected():
            if self.normalsVisual.get():
                self.networkClient.SendCommand("setNormalsVisualization enabled=true")
            else:
                self.networkClient.SendCommand("setNormalsVisualization enabled=false")

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
        self.physicsVisual.set(False)
        self.normalsVisual.set(False)
        if self.exitRequested:
            self.root.quit()

    def AddGameObjct(self):
        answer = simpledialog.askstring("Input", "Game object name", parent=self.root, initialvalue="GameObject")
        self.networkClient.SendCommand("createGameObject name=" + answer)

    def PreviewGameObject(self):
        self.networkClient.SendCommand("modelPreviewRequest input=Meshes/Gothic/G3_Myrtana_Castle_Mainhouse/G3_Myrtana_Castle_Mainhouse.obj output=D:/Tmp/castle.png")

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

    def OnSceneFileMsg(self, msg):
        self.currentSceneFileName = msg.get("filename")
        print(self.currentSceneFileName)

    def SaveSceneFile(self):
        if self.currentSceneFileName:
            self.networkClient.SendCommand("saveToFile filename=" + self.currentSceneFileName)
        else:
            self.fileManger.SaveSceneFile()