import tkinter as tk
from tkinter import messagebox

class SceneControlView:
    def __init__(self, networkClient, rootFrame):
        self.networkClient = networkClient

        infoFrame = tk.Frame(rootFrame)
        infoFrame.pack(padx=5, pady=5)

        btn = tk.Button(infoFrame, text="Start", command=self.StartScene)
        btn.grid(column=0, row=0, sticky=(tk.N, tk.S, tk.E, tk.W))

        btn = tk.Button(infoFrame, text="Stop", command=self.StopScene)
        btn.grid(column=1, row=0, sticky=(tk.N, tk.S, tk.E, tk.W))

        self.physicsVisualization = tk.BooleanVar()
        checkbox = tk.Checkbutton(infoFrame, text="Physics Visualization", variable=self.physicsVisualization, command=self.SetPhysicsVisualization)
        checkbox.grid(column=0, row=2, sticky=(tk.N, tk.S, tk.E, tk.W), columnspan=3)

        self.statusStr = tk.StringVar()
        self.statusStr.set("Status : stopped")

        status = tk.Label(infoFrame, textvariable=self.statusStr)
        status.grid(column=0, row=1, sticky=(tk.N, tk.S, tk.E, tk.W), columnspan=3)

        self.networkClient.SubscribeOnDisconnect(self.OnDisconnect)
        self.networkClient.SubscribeOnConnect(self.OnConnect)
        self.networkClient.SubscribeOnMessage("SceneStartedNotifMsg", self.OnStartSceneMsg)
        self.networkClient.SubscribeOnMessage("SceneStopedNotifMsg", self.OnStopedMsg)

    def SetPhysicsVisualization(self):
        if self.networkClient.IsConnected():
            if self.physicsVisualization.get():
                self.networkClient.SendCommand("setPhysicsVisualization enabled=true")
            else:
                self.networkClient.SendCommand("setPhysicsVisualization enabled=false")
        else:
            messagebox.showinfo(title="Info", message="Not connected")
            self.physicsVisualization.set(False)

    def OnDisconnect(self):
        self.statusStr.set("Status : disconnected")

    def OnConnect(self):
        self.statusStr.set("Status : stopped")

    def OnStopedMsg(self, msg):
        self.statusStr.set("Status : stopped")

    def OnStartSceneMsg(self, msg):
        self.statusStr.set("Status : started")

    def StartScene(self):
        if self.networkClient.IsConnected():
            self.networkClient.SendCommand("startScene")
            self.statusStr.set("Status : started")
        else:
            messagebox.showinfo(title="Info", message="Not connected")

    def StopScene(self):
        if self.networkClient.IsConnected():
            self.networkClient.SendCommand("stopScene")
            self.statusStr.set("Status : stopped")
        else:
            messagebox.showinfo(title="Info", message="Not connected")