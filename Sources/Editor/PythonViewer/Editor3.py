from GameObjectView import GameObjectView
from ComponentsView import ComponentsView
from SceneControlView import SceneControlView
from TransformView import TransformView
from InfoView import InfoView
from Menu import Menu
from Context import Context
from CommonWidgetTools import CalculateGeomentryCenterPosition
from tkinter import messagebox

import os
import tkinter as tk
import sys


class Editor:
    def __init__(self):
        self.root = tk.Tk()
        self.context = Context(self.root, TCP_IP, TCP_PORT)
        self.titleBase = "Editor"
        self.root.title(self.titleBase + " (disconnected)")

        self.root.geometry(CalculateGeomentryCenterPosition(self.context, 660, 750))

        self.rightFrame = tk.Frame(self.root, width=325, height=400)
        self.rightFrame.grid(row=0, column=1, padx=5, pady=5, sticky=(tk.W, tk.N))

        self.sceneControlView = SceneControlView(self.context.networkClient, self.rightFrame)
        self.infoView = InfoView(self.context.networkClient, self.rightFrame)
        self.transformView = TransformView(self.context.networkClient, self.rightFrame, self.infoView)
        self.componentsView = ComponentsView(self.context, self.rightFrame)

        self.gameObjectView = GameObjectView(self.context.networkClient, self.root, self.infoView, self.transformView,
                                             self.componentsView)
        self.menu = Menu(self.context.networkClient, self.root, self.gameObjectView)

        self.context.networkClient.SubscribeOnMessage("SceneFileMsg", self.OnSceneFileMsg)
        self.context.networkClient.SubscribeOnMessage("ErrorIndication", self.OnSceneFileMsg)
        self.context.networkClient.SubscribeOnDisconnect(self.OnDisconnect)
        self.context.networkClient.SubscribeOnConnect(self.OnConnect)

        self.root.mainloop()

    def OnErrorIndication(self, msg):
        messagebox.showerror(title=msg.get("title"), message=msg.get("message"))

    def OnDisconnect(self):
        self.root.title(self.titleBase + " (disconnected)")

    def OnConnect(self):
        self.root.title(self.titleBase + " (connected)")

    def OnSceneFileMsg(self, msg):
        self.root.title(self.titleBase + " (connected)" + os.path.basename(msg.get("filename")))

if __name__ == "__main__":
    TCP_IP = 'localhost'
    TCP_PORT = 1991

    if len(sys.argv) > 1:
        print("Connecting to {0}".format(sys.argv[1]))
        TCP_IP = sys.argv[1]

    Editor()
