import tkinter as tk
import tkinter.ttk as ttk
from CommonWidgetTools import CalculateGeomentryCenterPosition


class TerrainPainterView:
    def __init__(self, context, rootFrame):
        self.context = context
        self.networkClient = context.networkClient
        self.rootFrame = rootFrame
        self.brushTypeStr = tk.StringVar()
        self.stepInterpolationStr = tk.StringVar()
        self.strengthStr = tk.StringVar()
        self.brushSizeStr = tk.StringVar()
        self.IsDialogVisible = False

        self.dialog = 0
        self.strengthInput = 0
        self.brushSize = 0
        self.stepInterpolaton = 0
        self.brushTypes = 0
        self.networkClient.SubscribeOnMessage("TerrainPainterEnabled", self.OnTerrainPainterEnabled)
        self.context.networkClient.SubscribeOnDisconnect(self.OnDisconnect)

    def OnTerrainPainterEnabled(self, msg):
        self.OnClose()

        if msg.get("type") == "HeightMap":
            self.PainterBaseDialog(msg)
            self.AddToDialogHeightPostProcessFunctions()
        elif msg.get("type") == "BlendMap":
            self.PainterBaseDialog(msg)
            self.AddToTexturesToDialog()
        elif msg.get("type") == "Plant":
            self.PainterBaseDialog(msg)

    def PainterBaseDialog(self, msg):
        inputBrushTypes = []
        inputStepInterpolation = []
        for child in msg.getchildren():
            if child.tag == "brushTypes":
                for v in child.getchildren():
                    inputBrushTypes.append(v)
            if child.tag == "stepInterpolations":
                for v in child.getchildren():
                    inputStepInterpolation.append(v)

        self.dialog = tk.Toplevel(self.rootFrame)
        self.dialog.title("Terran painter view")
        #self.dialog.geometry(CalculateGeomentryCenterPosition(self.context, 400, 300))

        brushTypeLabel = tk.LabelFrame(self.dialog, text="Brush")
        brushTypeLabel.pack(fill=tk.X)

        self.brushTypes = ttk.Combobox(brushTypeLabel, textvariable=self.brushTypeStr)
        self.brushTypes['values'] = list(inputBrushTypes)
        self.brushTypes.pack(fill=tk.X, expand=1)
        currentBrushType = msg.get("stepInterpolation")
        if currentBrushType in inputBrushTypes:
            self.brushTypes.current(inputBrushTypes.index(currentBrushType))
        else:
            self.brushTypes.current(0)
        self.brushTypes.bind("<<ComboboxSelected>>", self.SendBrushType)

        stepInterpolatonLabel = tk.LabelFrame(self.dialog, text="Step interpolation")
        stepInterpolatonLabel.pack(fill=tk.X)
        self.stepInterpolaton = ttk.Combobox(stepInterpolatonLabel, textvariable=self.stepInterpolationStr)
        self.stepInterpolaton['values'] = list(inputStepInterpolation)
        self.stepInterpolaton.pack(fill=tk.X, expand=1)
        currentStepInterpolation = msg.get("stepInterpolation")
        if currentStepInterpolation in inputStepInterpolation:
            self.stepInterpolaton.current(inputStepInterpolation.index(currentStepInterpolation))
        else:
            self.stepInterpolaton.current(0)
        self.stepInterpolaton.bind("<<ComboboxSelected>>", self.SendStepInterpolation)

        brushSizeLabel = tk.LabelFrame(self.dialog, text="Brush size")
        brushSizeLabel.pack(fill=tk.X)

        self.brushSize = tk.Scale(brushSizeLabel, from_=1, to=50, tickinterval=10, orient=tk.HORIZONTAL,
                                  command=self.SendBrushSize)
        self.brushSize.pack(fill=tk.X, expand=1)
        self.brushSize.set(int(msg.get("brushSize")))

        stengthLabel = tk.LabelFrame(self.dialog, text="Strength")
        stengthLabel.pack(fill=tk.X)

        self.strengthInput = tk.Entry(stengthLabel, textvariable=self.strengthStr)
        self.strengthInput.pack(fill=tk.X, expand=1)
        self.strengthStr.set(msg.get("strength"))
        self.strengthStr.trace("w", self.SendStrength)

        self.dialog.protocol("WM_DELETE_WINDOW", self.OnClose)
        self.IsDialogVisible = True

    def AddToDialogHeightPostProcessFunctions(self):
        tk.Button(self.dialog, text="Recalculate normals",
                  command=lambda: self.networkClient.SendCommand("recalculateTerrainNormals")).pack(fill=tk.X)

    def AddToTexturesToDialog(self):
        texturesLabel = tk.LabelFrame(self.dialog, text="Textures")
        texturesLabel.pack(fill=tk.X)

        tk.Button(texturesLabel, text="Clear blend map", command=lambda: self.networkClient.SendCommand(
                      "clearTerrainsBlendMap")).pack(fill=tk.X)
        tk.Button(texturesLabel, text="Texture D", command=lambda: self.networkClient.SendCommand(
                      "updateTerrainPainterParam color=0000")).pack(fill=tk.X)
        tk.Button(texturesLabel, text="Texture R", command=lambda: self.networkClient.SendCommand(
                      "updateTerrainPainterParam color=1000")).pack(fill=tk.X)
        tk.Button(texturesLabel, text="Texture G", command=lambda: self.networkClient.SendCommand(
                      "updateTerrainPainterParam color=0100")).pack(fill=tk.X)
        tk.Button(texturesLabel, text="Texture B", command=lambda: self.networkClient.SendCommand(
                      "updateTerrainPainterParam color=0010")).pack(fill=tk.X)
        tk.Button(texturesLabel, text="Texture A", command=lambda: self.networkClient.SendCommand(
                      "updateTerrainPainterParam color=0001")).pack(fill=tk.X)

    def SendStrength(self, *args):
        value = self.strengthStr.get()
        if value:
            self.networkClient.SendCommand("updateTerrainPainterParam strength=" + value)

    def SendBrushSize(self, *args):
        self.networkClient.SendCommand("updateTerrainPainterParam brushSize=" + str(self.brushSize.get()))

    def SendStepInterpolation(self, *args):
        self.networkClient.SendCommand(
            "updateTerrainPainterParam stepInterpolation=" + str(self.stepInterpolaton.get()))

    def SendBrushType(self, *args):
        self.networkClient.SendCommand("updateTerrainPainterParam brushType=" + str(self.brushTypes.get()))

    def OnClose(self, *args):
        if self.IsDialogVisible:
            self.networkClient.SendCommand("disablePainter")
            self.CloseDialog()

    def OnDisconnect(self):
        self.CloseDialog()

    def CloseDialog(self):
        if self.IsDialogVisible:
            self.dialog.destroy()
            self.IsDialogVisible = False