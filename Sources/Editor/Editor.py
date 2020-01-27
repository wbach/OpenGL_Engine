#!/usr/bin/env python
import socket
import sys

TCP_IP = 'localhost'
TCP_PORT = 1991
BUFFER_SIZE = 512


#enum MessageTypes
Any = chr(251)
Ping = chr(252)
ConnectionMsg = chr(253)
Authentication = chr(254)
Text = chr(255)


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = (TCP_IP, TCP_PORT)

def SendMsg(msg, type):
    s.send(chr(4))
    s.send(type)
    s.send(msg + ';')

def RecevieMsg():
    msgFormat = s.recv(1)
    print "msgFormat data:", ord(msgFormat)
    msgType = s.recv(1)
    print "msgType data:", ord(msgType)
    msg = s.recv(BUFFER_SIZE)
    msg = msg.replace(';', '')
    msg = msg.replace('\x00', '')
    return msg

def RecevieConnectionMsg():
    print "msg data:", RecevieMsg()

def SendAuthenticationMessage():
    msg = "<AuthenticationMessage username=\"baszek\" password=\"haslo\"/>"
    SendMsg(msg, Authentication);

def SendOpenFileCommand(cmd):
    msg = "<TextMessage text=\"" + cmd + "\"/>"
    SendMsg(msg, Text);

try:
    s.connect(server_address)
    print "Connected."
    RecevieConnectionMsg();
    SendAuthenticationMessage();
    RecevieConnectionMsg();
    SendOpenFileCommand("openFile /home/baszek/test.jpg");
    s.close()
except:
    print("Unexpected error:", sys.exc_info()[0])
