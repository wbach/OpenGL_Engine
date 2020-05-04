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
        if msg.get("type") == "HeightMap":
            self.HeightPainterDialog(msg)
        if msg.get("type") == "BlendMap":
            print("Terrain Texture painter enabled.")

    def HeightPainterDialog(self, msg):
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
        self.dialog.geometry(CalculateGeomentryCenterPosition(self.context, 400, 300))

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

        self.brushSize = tk.Scale(brushSizeLabel, from_=1, to=50, tickinterval=5, orient=tk.HORIZONTAL,
                                  command=self.SendBrushSize)
        self.brushSize.pack(fill=tk.X, expand=1)
        self.brushSize.set(int(msg.get("brushSize")))

        stengthLabel = tk.LabelFrame(self.dialog, text="Strength")
        stengthLabel.pack(fill=tk.X)

        self.strengthInput = tk.Entry(stengthLabel, textvariable=self.strengthStr)
        self.strengthInput.pack(fill=tk.X, expand=1)
        self.strengthStr.set(msg.get("strength"))
        self.strengthStr.trace("w", self.SendStrength)

        self.dialog.lift()
        self.dialog.attributes('-topmost', True)
        # dialog.update()
        self.dialog.attributes('-topmost', False)
        self.dialog.protocol("WM_DELETE_WINDOW", self.OnClose)

        tk.Button(self.dialog, text="Recalculate normals",
                  command=lambda: self.networkClient.SendCommand("recalculateTerrainNormals")).pack(fill=tk.X)
        tk.Button(self.dialog, text="Recalculate Y offset",
                  command=lambda: self.networkClient.SendCommand(
                      "recalculateTerrainYOffset")).pack(fill=tk.X)
        self.IsDialogVisible = True

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
        self.networkClient.SendCommand("disableTerrainHeightPainter")
        self.dialog.destroy()
        self.IsDialogVisible = False

    def OnDisconnect(self):
        if self.IsDialogVisible:
            self.dialog.destroy()
