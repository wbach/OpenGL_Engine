from lxml import objectify

from CommonWidgetTools import CreateVectorInput


class TransformView:
    def __init__(self, networkClient, frame, infoView):
        self.networkClient = networkClient
        self.infoView = infoView
        self.selfEntering = False

        self.positionX, self.positionY, self.positionZ = CreateVectorInput(rootFrame=frame, label="Position",
                                                                           startColumn=0, startRow=1,
                                                                           inputCallback=self.EnterPosition)
        self.rotX, self.rotY, self.rotZ = CreateVectorInput(rootFrame=frame, label="Rotation", startColumn=0,
                                                            startRow=2, inputCallback=self.EnterRotation)
        self.scaleX, self.scaleY, self.scaleZ = CreateVectorInput(rootFrame=frame, label="Scale", startColumn=0,
                                                                  startRow=3, inputCallback=self.EnterScale)

        self.networkClient.SubscribeOnMessage("Transform", self.RecevieTransform)
        self.networkClient.SubscribeOnDisconnect(self.Clear)

    def Clear(self):
        self.selfEntering = True
        self.positionX.set("")
        self.positionY.set("")
        self.positionZ.set("")
        self.rotX.set("")
        self.rotY.set("")
        self.rotZ.set("")
        self.scaleX.set("")
        self.scaleY.set("")
        self.scaleZ.set("")
        self.selfEntering = False

    def ReqAndFill(self, gameObjectId):
        self.SendGetTransformRequest(gameObjectId)

    def Fill(self, position, rotation, scale):
        self.selfEntering = True
        self.positionX.set(position[0])
        self.positionY.set(position[1])
        self.positionZ.set(position[2])
        self.rotX.set(rotation[0])
        self.rotY.set(rotation[1])
        self.rotZ.set(rotation[2])
        self.scaleX.set(scale[0])
        self.scaleY.set(scale[1])
        self.scaleZ.set(scale[2])
        self.selfEntering = False

    def SendGetTransformRequest(self, gameObjectId):
        self.networkClient.SendCommand("transformReq id=" + str(gameObjectId))

    def RecevieTransform(self, root):
        self.selfEntering = True
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
        self.selfEntering = False

    def EnterPosition(self, input):
        if self.selfEntering:
            return

        x = self.positionX.get()
        y = self.positionY.get()
        z = self.positionZ.get()

        if self.infoView.GetObjectId():
            self.networkClient.SendCommand(
                "setPosition id=" + self.infoView.GetObjectId() + " x=" + x + " y=" + y + " z=" + z)

    def EnterRotation(self, input):
        if self.selfEntering:
            return

        x = self.rotX.get()
        y = self.rotY.get()
        z = self.rotZ.get()

        if self.infoView.GetObjectId():
            self.networkClient.SendCommand(
                "setRotation id=" + self.infoView.GetObjectId() + " x=" + x + " y=" + y + " z=" + z)

    def EnterScale(self, input):
        if self.selfEntering:
            return

        x = self.scaleX.get()
        y = self.scaleY.get()
        z = self.scaleZ.get()

        if self.infoView.GetObjectId():
            self.networkClient.SendCommand(
                "setScale id=" + self.infoView.GetObjectId() + " x=" + x + " y=" + y + " z=" + z)
