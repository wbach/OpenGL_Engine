import tkinter as tk
from tkinter import ttk
from tkinter import simpledialog

class GameObjectView:
    def __init__(self, networkClient, root, infoView, transformView, componentsView):
        self.networkClient = networkClient
        self.root = root
        self.infoView = infoView
        self.transformView = transformView
        self.componentsView = componentsView

        self.InitVariables()

        self.frame = tk.LabelFrame(root, width=200, height=400, text="GameObjects")
        self.frame.grid(row=0, column=0, padx=5, pady=5)

        self.tree = ttk.Treeview(self.frame, height=34, show="tree")
        self.tree.grid(columnspan=3, row=1, rowspan=37, padx=5, pady=5)
        self.tree.column("#0", width=300, minwidth=100)
        self.tree.bind('<<TreeviewSelect>>', self.OnSelectGameObject)

        self.gameObjectsCountStr = tk.StringVar()
        self.gameObjectsCountStr.set("Game objects count : 0")

        gameObjectsCountLabel = ttk.Label(root, textvariable=self.gameObjectsCountStr)
        gameObjectsCountLabel.grid(row=1, column=0, padx=5, pady=5, sticky=(tk.W))

        self.popupMenuSelection = 0
        self.popupMenu = tk.Menu(self.tree, tearoff=0)
        self.popupMenu.add_command(label="Create child", command=self.CreateChild)
        self.popupMenu.add_command(label="Create child with model", command=self.CreateChildWithModel)
        self.popupMenu.add_command(label="Rename gameObject", command=self.RenameObject)
        self.popupMenu.add_command(label="Delete gameObject", command=self.DeleteSelected)
        self.tree.bind("<Button-3>", self.Popup)

        self.networkClient.SubscribeOnMessage("ReloadScene", self.Clear)
        self.networkClient.SubscribeOnMessage("GameObjectRenamed", self.OnGameObjectRenamed)
        self.networkClient.SubscribeOnMessage("GameObjectDeleted", self.OnGameObjectDeleted)
        self.networkClient.SubscribeOnMessage("NewGameObjectInd", self.OnGameObjectMsg)
        self.networkClient.SubscribeOnMessage("CameraMsg", self.OnCameraMsg)
        self.networkClient.SubscribeOnMessage("SelectedObjectChanged", self.OnSelectedObjectChanged)
        networkClient.SubscribeOnDisconnect(self.Clear)

    def CreateChild(self):
        curItem = self.tree.focus()
        item = self.tree.item(curItem)
        gameObjectId = item['values'][0]
        answer = simpledialog.askstring("Input", "Game object name", parent=self.root, initialvalue="GameObject")
        self.networkClient.SendCommand("createGameObject parentGameObjectId=" + str(gameObjectId) + " name=" + answer)

    def CreateChildWithModel(self):
        curItem = self.tree.focus()
        item = self.tree.item(curItem)
        gameObjectId = item['values'][0]
        self.networkClient.SendCommand("createGameObject parentGameObjectId=" + str(gameObjectId))

    def Popup(self, event):
        self.popupMenuSelection = self.tree.identify_row(event.y)
        if self.popupMenuSelection:
            self.tree.focus(self.popupMenuSelection)
            self.tree.selection_set(self.popupMenuSelection)
            self.popupMenu.post(event.x_root, event.y_root)

    def DeleteSelected(self):
        curItem = self.tree.focus()
        item = self.tree.item(curItem)
        gameObjectId = item['values'][0]
        if self.popupMenuSelection:
           # self.tree.delete(self.popupMenuSelection)
            self.networkClient.SendCommand("deleteGameObject gameObjectId=" + str(gameObjectId))

    def OnGameObjectDeleted(self, msg):
        gameObjectId = int(msg.get("gameObjectId"))
        parentId, name, hwnd = self.gameObjects[gameObjectId]
        self.tree.delete(hwnd)
        del self.gameObjects[gameObjectId]
        self.UpdateGameObjectCount()

    def RenameObject(self):
        curItem = self.tree.focus()
        item = self.tree.item(curItem)
        gameObjectId = item['values'][0]
        if self.popupMenuSelection:
            answer = simpledialog.askstring("Input", "New game object name",
                                            parent=self.root)
            self.networkClient.SendCommand("renameGameObject gameObjectId=" + str(gameObjectId) + " name=" + answer)

    def OnGameObjectRenamed(self, msg):
        gameObjectId = int(msg.get("gameObjectId"))
        newName = msg.get("newName")
        parentId, oldName, hwnd = self.gameObjects[gameObjectId]
        self.tree.item(hwnd, text=newName)
        #rename tuple
        lst = list(self.gameObjects[gameObjectId])
        lst[1] = newName
        self.gameObjects[gameObjectId] = tuple(lst)

    def InitVariables(self):
        self.gameObjects = {}
        self.gameObjectsCount = 0
        self.goType = "go"
        self.cameraType = "camera"
        self.cameraInfo = []

    def OnCameraMsg(self, msg):
        if not self.cameraType in self.gameObjects:
            hwnd = self.tree.insert("", "end", None, text=self.cameraType, values=(0, self.cameraType))
            self.gameObjects[self.cameraType] = 0, self.cameraType, hwnd

        position = []
        rotation = []
        for e in msg.getchildren():
            if e.tag == "position":
                position = [e.get("x"), e.get("y"), e.get("z")]
            if e.tag == "rotation":
                rotation = [e.get("x"), e.get("y"), e.get("z")]

        self.cameraInfo = [position, rotation]

        if self.infoView.GetViewName() == self.cameraType:
            self.ShowCameraInView()

    def Clear(self):
        for i in self.tree.get_children():
            self.tree.delete(i)

        self.gameObjects.clear()
        self.InitVariables()
        self.UpdateGameObjectCount()

    def OnGameObjectMsg(self, msg):
        print("OnGameObjectMsg, Message : \"{0}\"".format(msg.tag))
        goId = int(msg.get("id"))
        parentId = int(msg.get("parentId"))
        name = msg.get("name")

        if parentId == 0:
            hwnd = self.tree.insert("", "end", None, text=name, values=(goId, self.goType))
            self.gameObjects[goId] = parentId, name, hwnd
        else:
            parentId, parentName, parentHwnd = self.gameObjects[parentId]
            hwnd = self.tree.insert(parentHwnd, "end", None, text=name, values=(goId, self.goType))
            self.gameObjects[goId] = parentId, name, hwnd

        self.gameObjectsCount = self.gameObjectsCount + 1
        # self.frame.setvar()
        print("Objects count : {0}".format(self.gameObjectsCount))
        self.UpdateGameObjectCount()

    def GetGameObjectNameAndId(self):
        curItem = self.tree.focus()
        item = self.tree.item(curItem)
        gameObjectId = item['values'][0]
        type = item['values'][1]
        return item['text'], gameObjectId, type

    def OnSelectGameObject(self, event):
        name, gameObjectId, type = self.GetGameObjectNameAndId()

        if type == self.goType:
            self.infoView.UpdateInfoWidget(name, gameObjectId)
            self.transformView.ReqAndFill(gameObjectId)
            self.componentsView.Fill(gameObjectId)
            self.networkClient.SendCommand("selectGameObject gameObjectId=" + str(gameObjectId))
        elif type == self.cameraType:
            self.ShowCameraInView()
            self.networkClient.SendCommand("getCamera")

    def OnSelectedObjectChanged(self, msg):
        gameObjectId = int(msg.get("id"))
        id, name, hwnd = self.gameObjects[gameObjectId]
        self.infoView.UpdateInfoWidget(name, gameObjectId)
        self.transformView.ReqAndFill(gameObjectId)
        self.componentsView.Fill(gameObjectId)
        #self.tree.focus(hwnd)
        self.tree.selection_set(hwnd)

    def GetObjectList(self):
        self.Clear()
        self.networkClient.SendCommand("getObjectList")
        self.networkClient.SendCommand("getCamera")

    def ShowCameraInView(self):
        self.infoView.UpdateInfoWidget(self.cameraType, "camera")
        self.transformView.Fill(self.cameraInfo[0], self.cameraInfo[1], ["---", "---", "---"])

    def UpdateGameObjectCount(self):
        self.gameObjectsCountStr.set("Game objects count : {0}".format(len(self.gameObjects)))