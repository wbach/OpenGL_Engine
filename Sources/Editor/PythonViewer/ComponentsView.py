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
        self.isDialogVisible = False
        self.params=[]
        self.seletedCompoentName=""

        self.componentsFrame = tk.LabelFrame(rootFrame, text="Components", width=270, height=25)
        self.componentsFrame.pack(padx=5, pady=5, fill=tk.X)

        btn = tk.Button(rootFrame, text="Add componnent", command=self.AddComponent, width=30)
        btn.pack(padx=5, pady=5)

        self.networkClient.SubscribeOnMessage("NewComponentMsgInd", self.OnComponentIndMsg)
        self.networkClient.SubscribeOnMessage("AvailableComponentMsgInd", self.AvailableComponentMsgInd)
        self.networkClient.SubscribeOnMessage("ComponentDataMessage", self.HandleComponentDataMessage)
        self.networkClient.SubscribeOnDisconnect(self.ClearComponents)

    def HandleComponentDataMessage(self, msg):
        self.CreateDialog(msg.get("name"), 280, 400)
        frame = tk.LabelFrame(self.dialog, text="Component parameters", width=270, height=25)
        frame.grid(row=0, column=0, padx=5, pady=5)
        self.seletedCompoentName = msg.get("name")

        # variableFrame = tk.Frame(frame, width=270, height=25)
        # variableFrame.grid(row=0, column=0, padx=5, pady=5)

        # valueFrame = tk.Frame(frame, width=270, height=25)
        # valueFrame.grid(row=0, column=1, padx=5, pady=5)

        for child in msg.getchildren():
            self.params = []
            if child.tag == "params":
                i = 0
                for param in child.getchildren():
                    variableFrame = tk.Frame(frame, width=270, height=25)
                    variableFrame.pack(padx=5, pady=5)
                    tk.Label(variableFrame, text=param.get("name")).grid(row=0, column=0, padx=5, pady=5)
                    tk.Label(variableFrame, text=param.get("type")).grid(row=0, column=1, padx=5, pady=5)

                    varType = param.get("type")
                    if varType == "vector":
                        print("unsupported")
                    else:
                        text = tk.StringVar()
                        text.set(param.get("value"))
                        tk.Entry(variableFrame, textvariable=text, width=15).grid(row=0, column=2, padx=5, pady=5)
                        self.params.append([param.get("name"), text])
                        i = i + 1

        buttonFrame = tk.Frame(self.dialog, width=270, height=25)
        buttonFrame.grid(row=1, column=0, padx=5, pady=0)

        btn = tk.Button(buttonFrame, text="Confirm", command=self.SendModifyComponentReq)
        btn.grid(row=0, column=0, padx=5, pady=0)
        btn = tk.Button(buttonFrame, text="Cancel", command=lambda : dialog.destroy())
        btn.grid(row=0, column=1, padx=5, pady=0)

    def SendModifyComponentReq(self):
        print(self.params)
        paramList = ""
        for param in self.params:
            print(param)
            print(param[0])
            print(param[1].get())
            value=param[1].get().replace(" ", "%")
            paramList = paramList + param[0] + "=" + value + " "

        print(paramList)
        self.networkClient.SendCommand("modifyComponentReq gameObjectId=" + str(self.gameObjectId) + " componentName=" + self.seletedCompoentName + " " + paramList)

    def ComponentParamChange(self, input):
        print("Param " + input.get() + " is changed")

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
            self.CreateDialog("Add component", 400, 50)
            combobox = ttk.Combobox(self.dialog, textvariable=self.addComponentCB)
            combobox['values'] = list(self.availableComponents)
            combobox.grid(row=0, column=0, padx=5, pady=5, sticky=(tk.W, tk.N))
            combobox.current(0)
            combobox.bind("<<ComboboxSelected>>", self.OnSelectedNewComponent)

            btn = tk.Button(self.dialog, text="Add", command=self.SendAddComponentReq, width=30)
            btn.grid(row=0, column=1, padx=5, pady=0)

    def SendAddComponentReq(self):
        self.networkClient.SendCommand("addComponent id=" + str(self.gameObjectId) + " name=" + self.addComponentCB.get())
        self.CloseDialog()

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

        id = self.size
        btn = tk.Button(self.componentsFrame, text=msg.get("name"), command=lambda: self.SendGetComponentParamRequest(msg.get("name")), width=30)
        btn.grid(column=1, row=self.size, padx=5, pady=0)

        self.size = self.size + 1

    def SendGetComponentParamRequest(self, name):
        self.networkClient.SendCommand("getComponentParams gameObjectId=" + str(self.gameObjectId) + " name=" + name);

    def ChangeComponentState(self):
        messagebox.showinfo(title="Info", message="ChangeComponentState no supported.")

    def CreateDialog(self, title, sizeX, sizeY):
        if (self.isDialogVisible):
            self.dialog.destroy()
        self.dialog = tk.Toplevel(self.rootFrame)
        self.dialog.title(title)
        self.dialog.geometry(CalculateGeomentryCenterPosition(self.context, sizeX, sizeY))
        self.dialog.attributes('-topmost', 'true')
        self.isDialogVisible = True

    def CloseDialog(self):
        if (self.isDialogVisible):
            self.dialog.destroy()
            self.isDialogVisible = False