import tkinter as tk
from functools import partial
from CommonWidgetTools import AskAndTryConnect

class TerrainGeneratorDialog:
    def __init__(self, root, networkClient):
        self.networkClient = networkClient
        self.root = root
        self.bias = tk.StringVar()
        self.bias.set("2.0")
        self.octaves = tk.StringVar()
        self.octaves.set("9")
        self.width = tk.StringVar()
        self.width.set("512")
        self.gameObjectId = tk.StringVar()
        self.gameObjectId.set("---")

    def Show(self, gameObjectId="---"):
        if AskAndTryConnect(self.networkClient):
            self.gameObjectId.set(gameObjectId)

            dialog = tk.Toplevel(self.root)
            dialog.title("Terran generate view")

            gameObjectIdLabel = tk.LabelFrame(dialog, text="GameObjectId")
            gameObjectIdLabel.pack(fill=tk.X)
            gameObjectIdInput = tk.Entry(gameObjectIdLabel, textvariable=self.gameObjectId)
            gameObjectIdInput.pack(fill=tk.X, expand=1)

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

            tk.Button(dialog, text="Generate with the same seed",
                      command=partial(self.SendGenerateTerrain, "false")) \
                .pack(fill=tk.X)

            tk.Button(dialog, text="Generate terrain with new seed",
                      command=partial(self.SendGenerateTerrain, "true")) \
                .pack(fill=tk.X)


    def SendGenerateTerrain(self, updateNoiseSeed):
        if self.networkClient.IsConnected():
            command="generateTerrains width=" + self.width.get() + " height=" + self.width.get() + " octaves=" + \
                    self.octaves.get() + " bias=" + self.bias.get() +\
                    " updateNoiseSeed=" + updateNoiseSeed

            if self.gameObjectId.get() != "---":
                command = command + " gameObjectId=" + self.gameObjectId.get()

            self.networkClient.SendCommand(command)