import tkinter as tk
import tkinter.ttk as ttk
from tkinter import messagebox
from CommonWidgetTools import CalculateGeomentryCenterPosition

class ComponentsView:
    def __init__(self, context, rootFrame):
        self.size = 0
        self.context = context
        self.networkClient = context.networkClient
        self.availableComponents = set()
        self.rootFrame = rootFrame
        self.addComponentCB = tk.StringVar()
        self.gameObjectId = 0

        self.componentsFrame = tk.LabelFrame(rootFrame, text="Components", width=270, height=25)
        self.componentsFrame.pack(padx=5, pady=5, fill=tk.X)

        btn = tk.Button(rootFrame, text="Add componnent", command=self.AddComponent, width=30)
        btn.pack(padx=5, pady=5,)

        self.networkClient.SubscribeOnMessage("NewComponentMsgInd", self.OnComponentIndMsg)
        self.networkClient.SubscribeOnMessage("AvailableComponentMsgInd", self.AvailableComponentMsgInd)
        self.networkClient.SubscribeOnDisconnect(self.ClearComponents)

    def AvailableComponentMsgInd(self, msg):
        self.availableComponents.add(msg.get("name"))
        print(self.availableComponents)

    def ClearComponents(self):
        self.size = 0
        componentList = self.componentsFrame.grid_slaves()
        for l in componentList:
            l.destroy()

    def Fill(self, gameObjectId):
        self.gameObjectId = gameObjectId
        self.ClearComponents()
        self.SendGetGameObjectComponentsReq(gameObjectId)

    def AddComponent(self):
        if len(self.availableComponents)  > 0:
            dialog = tk.Toplevel(self.rootFrame)
            dialog.title("Add component")
            dialog.geometry(CalculateGeomentryCenterPosition(self.context, 400, 50))
            dialog.attributes('-topmost', 'true')

            combobox = ttk.Combobox(dialog, textvariable=self.addComponentCB)
            combobox['values'] = list(self.availableComponents)
            combobox.grid(row=0, column=0, padx=5, pady=5, sticky=(tk.W, tk.N))
            combobox.current(0)
            combobox.bind("<<ComboboxSelected>>", self.OnSelectedNewComponent)

            btn = tk.Button(dialog, text="Add", command=self.SendAddComponentReq, width=30)
            btn.grid(row=0, column=1, padx=5, pady=0)

    def SendAddComponentReq(self):
        self.networkClient.SendCommand("addComponent id=" + str(self.gameObjectId) + " name=" + self.addComponentCB.get())

    def OnSelectedNewComponent(self, event):
        print("selcted {0}".format(self.addComponentCB.get()))

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