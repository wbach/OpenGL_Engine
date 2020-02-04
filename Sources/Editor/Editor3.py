from NetworkClient import NetworkClient
from GameObjectView import GameObjectView
from ComponentsView import ComponentsView
from TransformView import TransformView
from InfoView import InfoView
from Menu import Menu

import tkinter as tk
from tkinter import ttk
import sys
from tkinter import filedialog
from tkinter import messagebox

from lxml import objectify

import json
import pathlib

import subprocess

class Editor():
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("Editor")
        self.root.geometry('650x750')

        self.rightFrame = tk.Frame(self.root, width=325, height=400)
        self.rightFrame.grid(row=0, column=1, padx=5, pady=5, sticky=(tk.W, tk.N))

        self.networkClient  = NetworkClient(TCP_IP, TCP_PORT)
        self.infoView       = InfoView(self.networkClient, self.rightFrame)
        self.transformView  = TransformView(self.networkClient, self.rightFrame)
        self.componentsView = ComponentsView(self.networkClient, self.rightFrame)
        self.gameObjectView = GameObjectView(self.networkClient, self.root, self.infoView, self.transformView, self.componentsView)
        self.menu           = Menu(self.networkClient, self.root, self.gameObjectView)

        self.root.mainloop()


TCP_IP = 'localhost'
TCP_PORT = 1991

if len(sys.argv) > 1:
    print("Connecting to {0}".format(sys.argv[1]))
    TCP_IP=sys.argv[1]

Editor()


