import socket
import tkinter as tk

#enum MessageTypes
Any = chr(251)
Ping = chr(252)
ConnectionMsg = chr(253)
Authentication = chr(254)
Text = chr(255)

class NetworkClient:
    def __init__(self, tcpIp, port):
        self.encodeFormat_ = 'iso-8859-15'
        self.connect_ = False
        self.serverAddress = (tcpIp, port)

    def SendMsg(self, msg, type):
        self.socket_.send(chr(4).encode(self.encodeFormat_))
        self.socket_.send(type.encode(self.encodeFormat_))
        print("Sent msg size : {0}".format(len(msg)))
        self.socket_.send(len(msg).to_bytes(4, byteorder="little"))
        self.socket_.send(msg.encode(self.encodeFormat_))

    def RecevieMsg(self):
        msgFormat = self.socket_.recv(1)
        print("msgFormat : {0}".format(ord(msgFormat)))
        msgType = self.socket_.recv(1)
        print("msgType : {0}".format(ord(msgType)))
        msgSizeBytes = self.socket_.recv(4)
        msgSize = int.from_bytes(msgSizeBytes, "little")
        print("msgSize : {0}".format(msgSize))
        return self.socket_.recv(msgSize).decode(self.encodeFormat_)

    def RecevieConnectionMsg(self):
        print("msg data: {0}".format(self.RecevieMsg()));

    def SendAuthenticationMessage(self):
        msg = "<AuthenticationMessage username=\"baszek\" password=\"haslo\"/>"
        self.SendMsg(msg, Authentication);

    def SendCommand(self, cmd):
        msg = "<TextMessage text=\"" + cmd + "\"/>"
        self.SendMsg(msg, Text);
        print(msg)

    def Connect(self):
        try:
            self.socket_ = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket_.connect(self.serverAddress)
            self.connect_ = True
            print("Connected. Start authentication process.")
            self.RecevieConnectionMsg()
            self.SendAuthenticationMessage()
            self.RecevieConnectionMsg()
            print("Connected. Authentication process end.")
            return True
        except socket.error as exc:
            #print("Connecting error: {0}".format(exc))
            tk.messagebox.showerror(title="Error", message=exc)
        except:
            #print("Unexpected error:", syself.socket_.exc_info()[0])
            tk.messagebox.showerror(title="Error", message=syself.socket_.exc_info())
        return False

    def IsConnected(self):
        return self.connect_

    def PrintMsg(self, msg):
        print("RecevieMsg:")
        print(msg)
        print("===========")