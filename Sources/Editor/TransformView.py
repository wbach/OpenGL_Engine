import tkinter as tk
from lxml import objectify

from CommonWidgetTools import CreateVectorInput

class TransformView:
    def __init__(self, networkClient, frame, infoView):
        self.networkClient = networkClient
        self.infoView = infoView

        self.positionX, self.positionY, self.positionZ = CreateVectorInput(rootFrame=frame, label="Position", startColumn=0, startRow=1, inputCallback=self.EnterPosition)
        self.rotX, self.rotY, self.rotZ                = CreateVectorInput(rootFrame=frame, label="Rotation", startColumn=0, startRow=2, inputCallback=self.EnterRotation)
        self.scaleX, self.scaleY, self.scaleZ          = CreateVectorInput(rootFrame=frame, label="Scale", startColumn=0, startRow=3, inputCallback=self.EnterScale)

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
                self.positionX.set(e.get("x"))
                self.positionY.set(e.get("y"))
                self.positionZ.set(e.get("z"))
            if e.tag == "rotation":
                self.rotX.set(e.get("x"))
                self.rotY.set(e.get("y"))
                self.rotZ.set(e.get("z"))
            if e.tag == "scale":
                self.scaleX.set(e.get("x"))
                self.scaleY.set(e.get("y"))
                self.scaleZ.set(e.get("z"))

    def EnterPosition(self, input):
        x = self.positionX.get()
        y = self.positionY.get()
        z = self.positionZ.get()

        if self.infoView.GetObjectId():
            self.networkClient.SendCommand("setPosition id=" + self.infoView.GetObjectId() + " x=" + x + " y=" + y + " z=" + z)

    def EnterRotation(self, input):
        #networkClient.SendCommand("setPosition ")
        print(self.rotX.get())
        print(self.rotY.get())
        print(self.rotZ.get())

    def EnterScale(self, input):
        #networkClient.SendCommand("setPosition ")
        print(self.scaleX.get())
        print(self.scaleY.get())
        print(self.scaleZ.get())