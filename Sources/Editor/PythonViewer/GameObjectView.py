import tkinter as tk
from tkinter import ttk


class GameObjectView:
    def __init__(self, networkClient, root, infoView, transformView, componentsView):
        self.networkClient    = networkClient
        self.root             = root
        self.nameWidget       = infoView.nameTextWidget
        self.idTextVar        = infoView.idTextVar
        self.transformView    = transformView
        self.componentsView   = componentsView
        self.gameObjects      = {}
        self.gameObjectsCount = 0

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

        self.networkClient.SubscribeOnMessage("NewGameObjectInd", self.OnGameObjectMsg)
        networkClient.SubscribeOnDisconnect(self.Clear)

    def Clear(self):
        for i in self.tree.get_children():
            self.tree.delete(i)
        self.gameObjectsCountStr.set("Game objects count : 0")

    def OnGameObjectMsg(self, msg):
        print("OnGameObjectMsg, Message : \"{0}\"".format(msg.tag))
        goId = int(msg.get("id"))
        parentId = int(msg.get("parentId"))
        name = msg.get("name")

        if parentId == 0:
            hwnd = self.tree.insert("", "end", None, text=name, values=(goId))
            self.gameObjects[goId] = parentId, name, hwnd
        else:
            parentId, parentName, parentHwnd = self.gameObjects[parentId]
            hwnd = self.tree.insert(parentHwnd, "end", None, text=name, values=(goId))
            self.gameObjects[goId] = parentId, name, hwnd

        self.gameObjectsCount = self.gameObjectsCount + 1
        #self.frame.setvar()
        print("Objects count : {0}".format(self.gameObjectsCount))
        self.gameObjectsCountStr.set("Game objects count : {0}".format(self.gameObjectsCount))

    def UpdateInfoWidget(self, name, id):
        self.nameWidget.delete('1.0', tk.END)
        self.nameWidget.insert(tk.END, name)
        self.idTextVar.set(id)

    def GetGameObjectNameAndId(self):
        curItem = self.tree.focus()
        item    = self.tree.item(curItem)
        gameObjectId = item['values'][0]
        return item['text'], gameObjectId

    def OnSelectGameObject(self, event):
        name, gameObjectId = self.GetGameObjectNameAndId()
        self.UpdateInfoWidget(name, gameObjectId)
        self.transformView.Fill(gameObjectId)
        self.componentsView.Fill(gameObjectId)

    def GetObjectList(self):
        self.networkClient.SendCommand("getObjectList")
