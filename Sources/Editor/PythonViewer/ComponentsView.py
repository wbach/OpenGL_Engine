import tkinter as tk
import tkinter.ttk as ttk
import traceback
from tkinter import messagebox
from functools import partial
from PIL import Image, ImageTk
from os import path
import sys
class ComponentsView:
    def __init__(self, context, rootFrame, fileManager):
        self.size = 0
        self.context = context
        self.networkClient = context.networkClient
        self.availableComponents = set()
        self.rootFrame = rootFrame
        self.addComponentCB = tk.StringVar()
        self.gameObjectId = 0
        self.isDialogVisible = False
        self.params=[]
        self.requestedParams = dict() #defaultdict(list)
        self.seletedCompoentName=""
        self.fileManager = fileManager

        self.componentsFrame = tk.LabelFrame(rootFrame, text="Components", width=270, height=25)
        self.componentsFrame.pack(padx=5, pady=5, fill=tk.X)

        btn = tk.Button(rootFrame, text="Add componnent", command=self.AddComponent, width=30)
        btn.pack(padx=5, pady=5)

        self.networkClient.SubscribeOnMessage("NewComponentMsgInd", self.OnComponentIndMsg)
        self.networkClient.SubscribeOnMessage("AvailableComponentMsgInd", self.AvailableComponentMsgInd)
        self.networkClient.SubscribeOnMessage("ComponentDataMessage", self.HandleComponentDataMessage)
        self.networkClient.SubscribeOnDisconnect(self.ClearComponents)

        # load = Image.open("/media/baszek/Drive/DriveWBachProjects/GameData/Data/Textures/Terrain/Ground/oreon/Ground_17_DIF.jpg")
        # render = ImageTk.PhotoImage(load)
        # img = tk.Label(self.rootFrame, image=render)
        # img.image = render
        # img.grid(row=0, column=4, padx=5, pady=0)

    def HandleComponentDataMessage(self, msg):
        self.CreateDialog(msg.get("name"), 600, 400)
        frame = tk.LabelFrame(self.dialog, text="Component parameters", width=270, height=25)
        frame.grid(row=0, column=0, padx=5, pady=5, sticky=tk.W)
        self.seletedCompoentName = msg.get("name")

        for child in msg.getchildren():
            self.params = []
            self.requestedParams = dict()
            if child.tag == "params":
                i = 0
                for param in child.getchildren():
                    self.requestedParams[param.get("name")] = param.get("value")
                    tk.Label(frame, text=param.get("name")).grid(row=i, column=0, padx=5, pady=5, sticky=(tk.W, tk.E))
                    tk.Label(frame, text=param.get("type")).grid(row=i, column=1, padx=5, pady=5, sticky=(tk.W, tk.E))

                    varType = param.get("type")
                    if varType == "vector":
                        print("unsupported")
                    else:
                        text = tk.StringVar()
                        text.set(param.get("value"))

                        columnSpan = 1
                        if varType != "file":
                            columnSpan = 3
                        tk.Entry(frame, textvariable=text, width=50).grid(row=i, column=2, padx=5, pady=5, sticky=tk.W, columnspan=columnSpan)
                        self.params.append([param.get("name"), text])

                        if varType == "file" or varType == "imageFile" or varType == "modelFile":
                            btn = tk.Button(frame, text="Choose file", command=partial(self.OpenFile, text, varType))
                            btn.grid(row=i, column=3, padx=5, pady=0)

                        if varType == "imageFile":
                            try:
                                load = Image.open(param.get("value"))
                                load = load.resize((32, 32), Image.ANTIALIAS)
                                render = ImageTk.PhotoImage(load)
                                img = tk.Label(frame, image=render)
                                img.image = render
                                img.grid(row=i, column=4, padx=5, pady=0)
                                text.trace("w", partial(self.ParamValueChange, varType, text, img))
                                #img.place(x=0, y=0)
                            except OSError as err:
                                print("OS error: {0}".format(err))
                            except:
                                print("Exception in user code.")
                                print('-' * 60)
                                print(traceback.print_exc(file=sys.stdout))
                                print('-' * 60)

                    i = i + 1

        buttonFrame = tk.Frame(self.dialog, width=270, height=25)
        buttonFrame.grid(row=i, column=0, padx=5, pady=0)

        btn = tk.Button(buttonFrame, text="Confirm", command=self.SendModifyComponentReqAndCloseDialog)
        btn.grid(row=0, column=0, padx=5, pady=10)
        btn = tk.Button(buttonFrame, text="Cancel", command=self.CloseDialog)
        btn.grid(row=0, column=1, padx=5, pady=10)

        #CenterPosition(self.dialog, self.context)

    def ParamValueChange(self, varType, text, img, *args):
        #print(varType + " is changed. " + text.get())
        if text.get().lower().endswith(('.png', '.jpg', '.jpeg', '.bmp')):
            if path.exists(text.get()):
                load = Image.open(text.get())
                load = load.resize((32, 32), Image.ANTIALIAS)
                render = ImageTk.PhotoImage(load)
                img.configure(image=render)
                img.image = render


    def OpenFile(self, text, varType):
        filename = ""
        if varType == "imageFile":
            filename = self.fileManager.OpenImageFile()
        elif varType == "modelFile":
            filename = self.fileManager.OpenModelFile()
        else:
            filename = self.fileManager.OpenAllTypesFile()

        if filename:
            text.set(filename)

    def SendModifyComponentReqAndCloseDialog(self):
        paramList = ""
        #print(self.params)
        #print(self.requestedParams)
        for param in self.params:
            if self.requestedParams[param[0]] != param[1].get():
                value=param[1].get().replace(" ", "%")
                paramList = paramList + param[0] + "=" + value + " "

        if paramList:
            self.networkClient.SendCommand("modifyComponentReq gameObjectId=" + str(self.gameObjectId) + " componentName=" + self.seletedCompoentName + " " + paramList)
        else:
            print("no param to change")
        self.CloseDialog()

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
        if self.networkClient.IsConnected():
            if len(self.availableComponents) > 0:
                self.CreateDialog("Add component", 400, 50)
                combobox = ttk.Combobox(self.dialog, textvariable=self.addComponentCB)
                combobox['values'] = list(self.availableComponents)
                combobox.grid(row=0, column=0, padx=5, pady=5, sticky=(tk.W, tk.N))
                combobox.current(0)
                combobox.bind("<<ComboboxSelected>>", self.OnSelectedNewComponent)

                btn = tk.Button(self.dialog, text="Add", command=self.SendAddComponentReq, width=30)
                btn.grid(row=0, column=1, padx=5, pady=0)
            else:
                messagebox.showinfo(title="Info", message="Components are not available")
        else:
            messagebox.showinfo(title="Info", message="Not connected")

    def SendAddComponentReq(self):
        if self.networkClient.IsConnected():
            self.networkClient.SendCommand("addComponent id=" + str(self.gameObjectId) + " name=" + self.addComponentCB.get())
            self.CloseDialog()
        else:
            messagebox.showinfo(title="Info", message="Not connected")

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

        btn = tk.Button(self.componentsFrame, text=msg.get("name"), command=lambda: self.SendGetComponentParamRequest(msg.get("name")), width=30)
        btn.grid(column=1, row=self.size, padx=5, pady=0)
        self.size = self.size + 1

        if msg.get("name") == "Grass":
            btn = tk.Button(self.componentsFrame, text="Enable grass painter",
                            command=lambda: self.networkClient.SendCommand("enablePlantPainter gameObjectId=" + str(self.gameObjectId)), width=30)
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
        #self.dialog.attributes('-topmost', 'true')
        self.isDialogVisible = True

    def CloseDialog(self):
        if (self.isDialogVisible):
            self.dialog.destroy()
            self.isDialogVisible = False