import tkinter as tk

class InfoView:
    def __init__(self, networkClient, rootFrame):
        self.idTextVar = tk.StringVar()

        infoFrame = tk.LabelFrame(rootFrame, width=290, height=50, text="Info")
        infoFrame.pack(padx=5, pady=5, fill=tk.X)

        tk.Label(infoFrame, text="Id : ").grid(row=0, column=0, padx=5, pady=0, ipadx=0, sticky=(tk.W))
        tk.Label(infoFrame, textvariable = self.idTextVar).grid(row=0, column=1, padx=5, pady=0, ipadx=0, sticky=(tk.W))

        tk.Label(infoFrame, text="Name : ").grid(row=1, column=0, padx=5, pady=0, ipadx=0, sticky=(tk.W))
        self.nameTextWidget = tk.Text(infoFrame, height=1, width=28)
        self.nameTextWidget.grid(column=1, row=1, padx=7, pady=5)

        networkClient.SubscribeOnDisconnect(self.Clear)

    def Clear(self):
        self.idTextVar.set("")
        self.nameTextWidget.delete('1.0', tk.END)
        self.nameTextWidget.update()

    def GetObjectId(self):
        return self.idTextVar.get()