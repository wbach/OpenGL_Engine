import tkinter as tk
from tkinter import messagebox


class ComponentsView:
    def __init__(self, networkClient, rootFrame):
        self.networkClient = networkClient
        self.size          = 0

        self.componentsFrame = tk.LabelFrame(rootFrame, text="Components", width=270, height=25)
        self.componentsFrame.pack(padx=5, pady=5, fill=tk.X)

        btn = tk.Button(rootFrame, text="Add componnent", command=self.AddComponent, width=30)
        btn.pack(padx=5, pady=5,)

        self.networkClient.SubscribeOnMessage("NewComponentMsgInd", self.OnComponentIndMsg)
        self.networkClient.SubscribeOnDisconnect(self.ClearComponents)

    def ClearComponents(self):
        self.size = 0
        componentList = self.componentsFrame.grid_slaves()
        for l in componentList:
            l.destroy()

    def Fill(self, gameObjectId):
        self.ClearComponents()
        self.SendGetGameObjectComponentsReq(gameObjectId)

    def AddComponent(self):
        print("Not supported yet.")
        messagebox.showinfo(title="Info", message="Not supported yet.")

    def SendGetGameObjectComponentsReq(self, gameObjectId):
        self.networkClient.SendCommand("getGameObjectComponentsListReq id=" + str(gameObjectId))

    def OnComponentIndMsg(self, msg):
        print("OnComponentIndMsg, Message : \"{0}\"".format(msg.tag))
        # checkVar = tk.BooleanVar()
        checkVar = False
        if msg.get("isActive") == "true":
            checkVar = True

        checkBox = tk.Checkbutton(self.componentsFrame, variable=checkVar, command=self.ChangeComponentState,
                                  onvalue=True, offvalue=False)
        checkBox.grid(column=0, row=self.size, padx=5, pady=0)

        btn = tk.Button(self.componentsFrame, text=msg.get("name"), command=self.ShowEditComponentDialog, width=30)
        btn.grid(column=1, row=self.size, padx=5, pady=0)

        self.size = self.size + 1

    def ShowEditComponentDialog(self):
        messagebox.showinfo(title="Info", message="ShowEditComponentDialog no supported.")

    def ChangeComponentState(self):
        messagebox.showinfo(title="Info", message="ChangeComponentState no supported.")