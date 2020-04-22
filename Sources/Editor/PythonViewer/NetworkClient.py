from collections import defaultdict
from tkinter import messagebox
import socket
import _thread
import time

import tkinter as tk
from lxml import objectify

# enum MessageTypes
Any = chr(251)
Ping = chr(252)
ConnectionMsg = chr(253)
Authentication = chr(254)
Text = chr(255)


class NetworkClient:
    def __init__(self, tcpIp, port):
        self.encodeFormat_ = 'iso-8859-15'
        self.connect_ = False
        self.isRunning = True
        self.debugPrinting = True
        self.serverAddress = (tcpIp, port)
        self.disconnectSubscribers = []
        self.connectSubscribers = []
        self.messageSubscribers = defaultdict(list)

    def SubscribeOnMessage(self, msgType, callback):
        self.messageSubscribers[msgType].append(callback)

    def SubscribeOnDisconnect(self, callback):
        self.disconnectSubscribers.append(callback)

    def SubscribeOnConnect(self, callback):
        self.connectSubscribers.append(callback)

    def Disconnect(self):
        if self.connect_:
            print("Disconnect.")
            self.isRunning = False
            self.connect_ = False
            self.socket_.close()
            for sub in self.disconnectSubscribers:
                sub()
        else:
            messagebox.showinfo(title="Info", message="Not connected")

    def RecevieThread(self):
        while self.isRunning:
            try:
                msg = self.RecevieMsg()
            except:
                self.Disconnect()
                break

            self.PrintMsg(msg)
            try:
                main = objectify.fromstring(msg)
                print("Parsing fromstring complete.")
                msgType = main.tag
                if msgType in self.messageSubscribers:
                    for subscriber in self.messageSubscribers[msgType]:
                        subscriber(main)
                else:
                    self.Print("Msg \"{0}\" handler not found".format(msgType))
            except:
                print("Parsing msg failed.")

    def SendMsg(self, msg, type):
        self.socket_.send(chr(4).encode(self.encodeFormat_))
        self.socket_.send(type.encode(self.encodeFormat_))
        self.Print("Sent msg size : {0}".format(len(msg)))
        self.socket_.send(len(msg).to_bytes(4, byteorder="little"))
        self.socket_.send(msg.encode(self.encodeFormat_))

    def RecevieMsg(self):
        msgFormat = self.socket_.recv(1)
        self.Print("msgFormat : {0}".format(ord(msgFormat)))
        msgType = self.socket_.recv(1)
        self.Print("msgType : {0}".format(ord(msgType)))
        msgSizeBytes = self.socket_.recv(4)
        msgSize = int.from_bytes(msgSizeBytes, "little")
        self.Print("msgSize : {0}".format(msgSize))
        return self.socket_.recv(msgSize).decode(self.encodeFormat_)

    def RecevieConnectionMsg(self):
        self.Print("msg data: {0}".format(self.RecevieMsg()))

    def SendAuthenticationMessage(self):
        msg = "<AuthenticationMessage username=\"baszek\" password=\"haslo\"/>"
        self.SendMsg(msg, Authentication)

    def SendCommand(self, cmd):
        msg = "<TextMessage text=\"" + cmd + "\"/>"
        self.SendMsg(msg, Text)
        self.Print(msg)

    def Connect(self):
        try:
            self.socket_ = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket_.connect(self.serverAddress)
            self.connect_ = True
            self.isRunning = True
            print("Connected")
            self.Print("Start authentication process.")
            self.RecevieConnectionMsg()
            self.SendAuthenticationMessage()
            self.RecevieConnectionMsg()
            self.thread_ = _thread.start_new_thread(self.RecevieThread, ())
            self.Print("Connected. Authentication process end.")
            for sub in self.connectSubscribers:
                sub()
            return True
        except socket.error as exc:
            # print("Connecting error: {0}".format(exc))
            tk.messagebox.showerror(title="Error", message=exc)
        except:
            # print("Unexpected error:", syself.socket_.exc_info()[0])
            tk.messagebox.showerror(title="Error", message=syself.socket_.exc_info())
        return False

    def IsConnected(self):
        return self.connect_

    def PrintMsg(self, msg):
        self.Print("RecevieMsg:")
        self.Print(msg)
        self.Print("===========")

    def Print(self,  str):
        if self.debugPrinting:
            print(str)
