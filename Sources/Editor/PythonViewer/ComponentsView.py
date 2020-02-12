import tkinter as tk
from tkinter import messagebox

from lxml import objectify


class ComponentsView:
    def __init__(self, networkClient, rootFrame):
        self.networkClient = networkClient
        self.size          = 0

        self.componentsFrame = tk.LabelFrame(rootFrame, text="Components", width=250, height=25)
        self.componentsFrame.grid(column=0, row=4, padx=0, pady=0, ipady=5)

        btn = tk.Button(rootFrame, text="Add componnent", command=self.AddComponent, width=30)
        btn.grid(column=0, row=5, padx=5, pady=5)

    def Fill(self, gameObjectId):
        self.SendGetGameObjectComponentsReq(gameObjectId)
        self.RecevieComponents()

    def AddComponent(self):
        print("Not supported yet.")
        messagebox.showinfo(title="Info", message="Not supported yet.")

    def SendGetGameObjectComponentsReq(self, gameObjectId):
        self.networkClient.SendCommand("getGameObjectComponentsListReq id=" + str(gameObjectId))

    def RecevieComponents(self):
        componentList = self.componentsFrame.grid_slaves()
        for l in componentList:
            l.destroy()

        self.size = 0

        while True:
            msg = self.networkClient.RecevieMsg()
            self.networkClient.PrintMsg(msg)

            main = objectify.fromstring(msg)
            if main.tag == "TextMessage":
                value=main.get("text")
                if value == "end":
                    break;

                checkVar = tk.BooleanVar()
                checkVar.set(False)
                checkBox = tk.Checkbutton(self.componentsFrame, variable=checkVar, command=self.ChangeComponentState, onvalue=True, offvalue=False)
                checkBox.grid(column=0, row=self.size, padx=5, pady=0)
                checkBox.select()

                btn = tk.Button(self.componentsFrame, text=value, command=self.ShowEditComponentDialog, width=30)
                btn.grid(column=1, row=self.size, padx=5, pady=0)

                self.size = self.size + 1

    def ShowEditComponentDialog(self):
        messagebox.showinfo(title="Info", message="ShowEditComponentDialog no supported.")

    def ChangeComponentState(self):
        messagebox.showinfo(title="Info", message="ChangeComponentState no supported.")