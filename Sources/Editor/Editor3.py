from tkinter import *
from tkinter.ttk import *

import socket
import json
from tkinter import filedialog

TCP_IP = 'localhost'
TCP_PORT = 1991
BUFFER_SIZE = 512

if len(sys.argv) > 1:
    print("Connecting to {0}".format(sys.argv[1]))
    TCP_IP=sys.argv[1]

#enum MessageTypes
Any = chr(251)
Ping = chr(252)
ConnectionMsg = chr(253)
Authentication = chr(254)
Text = chr(255)
EncodeFormat = 'iso-8859-15'

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = (TCP_IP, TCP_PORT)

def SendMsg(msg, type):
    s.send(chr(4).encode(EncodeFormat))
    s.send(type.encode(EncodeFormat))
    s.send((msg + ';').encode(EncodeFormat))

def RecevieMsg():
    msgFormat = s.recv(1).decode(EncodeFormat)
    print("msgFormat data: {0}".format(ord(msgFormat)))
    msgType = s.recv(1).decode(EncodeFormat)
    print("msgType data: {0}".format(ord(msgType)))
    msg = s.recv(BUFFER_SIZE).decode(EncodeFormat)
    msg = msg.replace(';', '')
    msg = msg.replace('\x00', '')
    return msg

def RecevieConnectionMsg():
    print("msg data: {0}".format(RecevieMsg()));

def SendAuthenticationMessage():
    msg = "<AuthenticationMessage username=\"baszek\" password=\"haslo\"/>"
    SendMsg(msg, Authentication);

def SendOpenFileCommand(cmd):
    msg = "<TextMessage text=\"" + cmd + "\"/>"
    SendMsg(msg, Text);

def connect():
    try:
        s.connect(server_address)
        print("Connected.")
        RecevieConnectionMsg();
        SendAuthenticationMessage();
        RecevieConnectionMsg();
    except socket.error as exc:
        print("Connecting error: {0}".format(exc))
        return
    except:
        print("Unexpected error:", sys.exc_info()[0])

def OpenFile():
    filename = filedialog.askopenfilename(initialdir = "/",title = "Select file",filetypes = (("scene files","*.xml"),("all files","*.*")))
    if filename:
        SendOpenFileCommand("openFile " + filename);

def Close():
    s.close()

window = Tk()
window.title("Editor")
window.geometry('405x800')

# btn = Button(window, text="Connect", command=connect) 
# btn.grid(column=0, row=0, sticky=(N, S, E, W))

tree=Treeview(window)
tree.grid(columnspan=3, row=1)
tree["columns"]=("one","two","three")
tree.column("#0", width=100, minwidth=100)
tree.column("one", width=100, minwidth=100)
tree.column("two", width=100, minwidth=100)
tree.column("three", width=100, minwidth=100)

tree.heading("#0",text="Name")
tree.heading("one", text="Date modified")
tree.heading("two", text="Type")
tree.heading("three", text="Size")

# Level 1
folder1=tree.insert("", 1, None, text="Folder 1", values=("23-Jun-17 11:05","File folder",""))
# tree.insert("", 2, "", text="text_file.txt", values=("23-Jun-17 11:25","TXT file","1 KB"))
# # Level 2
tree.insert(folder1, "end", None, text="photo1.png", values=("23-Jun-17 11:28","PNG file","2.6 KB"))

# tree.insert(folder1, "end", "", text="photo2.png", values=("23-Jun-17 11:29","PNG file","3.2 KB"))
# tree.insert(folder1, "end", "", text="photo3.png", values=("23-Jun-17 11:30","PNG file","3.1 KB"))

def donothing():
    return

menubar = Menu(window)
filemenu = Menu(menubar, tearoff=0)
filemenu.add_command(label="Connect", command=connect)
filemenu.add_command(label="New", command=donothing)
filemenu.add_command(label="Open", command=OpenFile)
filemenu.add_command(label="Save", command=donothing)
filemenu.add_command(label="Save as...", command=donothing)
filemenu.add_command(label="Close", command=Close)
menubar.add_cascade(label="File", menu=filemenu)
filemenu.add_separator()
filemenu.add_command(label="Exit", command=window.quit)
window.config(menu=menubar)

window.mainloop()
