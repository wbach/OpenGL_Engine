import tkinter as tk
from tkinter import ttk
from lxml import objectify

from InfoView import InfoView

class GameObjectView:
    def __init__(self, networkClient, root, infoView, transformView, componentsView):
        self.networkClient  = networkClient
        self.root           = root
        self.nameWidget     = infoView.nameTextWidget
        self.idTextVar      = infoView.idTextVar
        self.transformView  = transformView
        self.componentsView = componentsView
        self.gameObjects    = {}

        frame = tk.LabelFrame(root, width=200, height=400, text="GameObjects")
        frame.grid(row=0, column=0, padx=5, pady=5)

        self.tree = ttk.Treeview(frame, height=35, show="tree")
        self.tree.grid(columnspan=3, row=1, rowspan=37, padx=5, pady=5)
        self.tree.column("#0", width=300, minwidth=100)
        self.tree.bind('<<TreeviewSelect>>', self.OnSelectGameObject)

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
        count = 0
        while(True):
            msg = self.networkClient.RecevieMsg()
            self.networkClient.PrintMsg(msg)

            main = objectify.fromstring(msg)
            if main.tag == "TextMessage":
                if main.get("text") == "end":
                    break;
            if main.tag == "GameObject":
                print("insert")
                goId = int(main.get("id"))
                parentId = int(main.get("parentId"))
                name = main.get("name")

                if parentId == 0:
                    hwnd = self.tree.insert("", "end", None, text=name, values=(goId))
                    self.gameObjects[goId] = parentId, name, hwnd
                else:
                    parentId, parentName, parentHwnd = self.gameObjects[parentId]
                    hwnd = self.tree.insert(parentHwnd, "end", None, text=name, values=(goId))
                    self.gameObjects[goId] = parentId, name, hwnd

                count = count + 1

        print("Objects count : {0}".format(count))
