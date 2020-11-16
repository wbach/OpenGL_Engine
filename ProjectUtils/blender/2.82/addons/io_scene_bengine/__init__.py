bl_info = {
    "name": "BEngine format",
    "author": "Wojciech Bach",
    "version": (3, 8, 0),
    "blender": (2, 81, 6),
    "location": "File > Import-Export",
    "description": "Import-Export bengine models",
    "warning": "",
    "doc_url": "",
    "support": 'OFFICIAL',
    "category": "Import-Export",
}

import bpy
from bpy.props import (
        BoolProperty,
        FloatProperty,
        StringProperty,
        EnumProperty,
        )
from bpy_extras.io_utils import (
        ImportHelper,
        ExportHelper,
        orientation_helper,
        path_reference_mode,
        axis_conversion,
        )

if "bpy" in locals():
    import importlib
    if "bexporter" in locals():
        importlib.reload(bexporter)

@orientation_helper(axis_forward='-Z', axis_up='Y')
class ExportBengine(bpy.types.Operator, ExportHelper):
    bl_idname = "export_scene.bengine"
    bl_label = 'Export bengine'
    bl_options = {'PRESET'}
    filename_ext = ".bengine"

    def execute(self, context):
        from . import bexporter
        print("execute")
        keywords = self.as_keywords(ignore=("axis_forward",
                                    "axis_up",
                                    "global_scale",
                                    "check_existing",
                                    "filter_glob",
                                    ))
        print(keywords['filepath'])
        return bexporter.export(context, keywords)

    def draw(self, context):
        pass

@orientation_helper(axis_forward='-Z', axis_up='Y')
class ImportBengine(bpy.types.Operator, ImportHelper):
    bl_idname = "import_scene.bengine"
    bl_label = "Import bengine"
    bl_options = {'PRESET', 'UNDO'}

    filename_ext = ".bengine"
    filter_glob: StringProperty(
            default="*.bengine",
            options={'HIDDEN'},
            )

    def execute(self, context):
        # print("Selected: " + context.active_object.name)
        return False 

    def draw(self, context):
        pass

classes = (
    ImportBengine,
    ExportBengine
)

def menu_func_import(self, context):
    self.layout.operator(ImportBengine.bl_idname, text="BEngine (.bengine)")


def menu_func_export(self, context):
    self.layout.operator(ExportBengine.bl_idname, text="BEngine (.bengine)")

def register():
    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

    for cls in classes:
        bpy.utils.unregister_class(cls)


if __name__ == "__main__":
    register()