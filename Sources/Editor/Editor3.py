from tkinter import *
import socket
import json

TCP_IP = 'localhost'
TCP_PORT = 1991
BUFFER_SIZE = 512

if len(sys.argv) > 0:
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
        SendOpenFileCommand("openFile /home/baszek/test.jpg");
        s.close()
    except socket.error as exc:
        print("Connecting error: {0}".format(exc))
        return
    except:
        print("Unexpected error:", sys.exc_info()[0])

window = Tk()
window.title("Editor")
window.geometry('350x200')

btn = Button(window, text="Connect", command=connect) 
btn.grid(column=1, row=0)

window.mainloop()
