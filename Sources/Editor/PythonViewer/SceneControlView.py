import tkinter as tk

class SceneControlView:
    def __init__(self, networkClient, rootFrame):
        self.networkClient = networkClient

        infoFrame = tk.Frame(rootFrame)
        infoFrame.pack(padx=5, pady=5)

        btn = tk.Button(infoFrame, text="Start", command=self.StartScene)
        btn.grid(column=0, row=0, sticky=(tk.N, tk.S, tk.E, tk.W))

        btn = tk.Button(infoFrame, text="Stop", command=self.StopScene)
        btn.grid(column=1, row=0, sticky=(tk.N, tk.S, tk.E, tk.W))

        self.statusStr = tk.StringVar()
        self.statusStr.set("Status : stopped")

        status = tk.Label(infoFrame, textvariable=self.statusStr)
        status.grid(column=0, row=1, sticky=(tk.N, tk.S, tk.E, tk.W), columnspan=2)

    def StartScene(self):
        self.networkClient.SendCommand("startScene")
        self.statusStr.set("Status : started")

    def StopScene(self):
        self.networkClient.SendCommand("stopScene")
        self.statusStr.set("Status : stopped")