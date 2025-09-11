import tkinter as tk

class FileExplorer:
    def __init__(self, context, rootFrame):
        explorerWindow = tk.Toplevel(rootFrame)
        explorerWindow.geometry("200x200")
        explorerWindow.title("File Explorer")