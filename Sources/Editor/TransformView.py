import tkinter as tk
from lxml import objectify

from CommonWidgetTools import CreateVectorInput

class TransformView:
    def __init__(self, networkClient, frame):
        self.networkClient = networkClient

        self.positionX, self.positionY, self.positionZ = CreateVectorInput(rootFrame=frame, label="Position", startColumn=0, startRow=1, keyFunc=self.EnterPosition)
        self.rotX, self.rotY, self.rotZ                = CreateVectorInput(rootFrame=frame, label="Rotation", startColumn=0, startRow=2, keyFunc=self.EnterRotation)
        self.scaleX, self.scaleY, self.scaleZ          = CreateVectorInput(rootFrame=frame, label="Scale", startColumn=0, startRow=3, keyFunc=self.EnterScale)

    def Fill(self, gameObjectId):
        self.SendGetTransformRequest(gameObjectId)
        self.RecevieTransform()

    def SendGetTransformRequest(self, gameObjectId):
        self.networkClient.SendCommand("transformReq id=" + str(gameObjectId));

    def RecevieTransform(self):
        msg = self.networkClient.RecevieMsg()
        root = objectify.fromstring(msg)

        if root.tag != "Transform":
            return

        for e in root.getchildren():
            if e.tag == "position":
                self.EditBoxSetText(self.positionX, e.get("x"))
                self.EditBoxSetText(self.positionY, e.get("y"))
                self.EditBoxSetText(self.positionZ, e.get("z"))
            if e.tag == "rotation":
                self.EditBoxSetText(self.rotX, e.get("x"))
                self.EditBoxSetText(self.rotY, e.get("y"))
                self.EditBoxSetText(self.rotZ, e.get("z"))
            if e.tag == "scale":
                self.EditBoxSetText(self.scaleX, e.get("x"))
                self.EditBoxSetText(self.scaleY, e.get("y"))
                self.EditBoxSetText(self.scaleZ, e.get("z"))

    def EditBoxSetText(self, editBox, value):
        editBox.delete('1.0', tk.END)
        editBox.insert(tk.END, value)

    def EnterPosition(self, input):
        #networkClient.SendCommand("setPosition ")
        print("Enter : {0}".format(input))
    def EnterRotation(self, input):
        #networkClient.SendCommand("setPosition ")
        print("Enter : {0}".format(input))
    def EnterScale(self, input):
        #networkClient.SendCommand("setPosition ")
        print("Enter : {0}".format(input))