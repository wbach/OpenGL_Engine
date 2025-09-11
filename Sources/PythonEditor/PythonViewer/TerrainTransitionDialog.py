import tkinter as tk
from functools import partial
from CommonWidgetTools import AskAndTryConnect

class TerrainTransitionDialog:
    def __init__(self, root, networkClient):
        self.networkClient = networkClient
        self.idNotSetStr = "---";
        self.root = root
        self.transitionSize = tk.StringVar()
        self.transitionSize.set("2.0")
        self.gameObjectId1 = tk.StringVar()
        self.gameObjectId1.set(self.idNotSetStr)
        self.gameObjectId2 = tk.StringVar()
        self.gameObjectId2.set(self.idNotSetStr)

    def Show(self):
        if AskAndTryConnect(self.networkClient):
            dialog = tk.Toplevel(self.root)
            dialog.title("Terran generate view")

            self.createEntryWithLabel(dialog, "GameObject1 id", self.gameObjectId1)
            self.createEntryWithLabel(dialog, "GameObject2 id", self.gameObjectId2)
            self.createEntryWithLabel(dialog, "Transition Size", self.transitionSize)

            tk.Button(dialog, text="Create",
                      command=partial(self.SendCommand)) \
                .pack(fill=tk.X)

    def createEntryWithLabel(self, root, name, stringVarName):
            label = tk.LabelFrame(root, text=name)
            label.pack(fill=tk.X)
            entry = tk.Entry(label, textvariable=stringVarName)
            entry.pack(fill=tk.X, expand=1)


    def SendCommand(self):
        if self.networkClient.IsConnected():
            if (self.gameObjectId1.get() != self.idNotSetStr and self.gameObjectId2.get() != self.idNotSetStr):
                command="createTerrainTransition terrainGameObjectId1=" + self.gameObjectId1.get() + " terrainGameObjectId2=" + self.gameObjectId2.get() + " transitionSize=" + \
                        self.transitionSize.get()
                self.networkClient.SendCommand(command)