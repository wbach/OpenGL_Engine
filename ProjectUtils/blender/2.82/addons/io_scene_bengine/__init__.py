@orientation_helper(axis_forward='-Z', axis_up='Y')
class ExportOBJ(bpy.types.Operator, ExportHelper):
	bl_idname = "export_scene.bengine"
    bl_label = 'Export bengine'
    bl_options = {'PRESET'}
    filename_ext = ".bengine"

    def execute(self, context):
        from . import export_obj

        from mathutils import Matrix
        keywords = self.as_keywords(ignore=("axis_forward",
                                            "axis_up",
                                            "global_scale",
                                            "check_existing",
                                            "filter_glob",
                                            ))

        global_matrix = (Matrix.Scale(self.global_scale, 4) @
                         axis_conversion(to_forward=self.axis_forward,
                                         to_up=self.axis_up,
                                         ).to_4x4())

        keywords["global_matrix"] = global_matrix
        return export_obj.save(context, **keywords)

    def draw(self, context):
        pass

classes = (
    ImportOBJ,
    OBJ_PT_import_include,
    OBJ_PT_import_transform,
    OBJ_PT_import_geometry,
    ExportOBJ,
    OBJ_PT_export_include,
    OBJ_PT_export_transform,
    OBJ_PT_export_geometry,
)

def menu_func_import(self, context):
    self.layout.operator(ImportOBJ.bl_idname, text="BEngine (.bengine)")


def menu_func_export(self, context):
    self.layout.operator(ExportOBJ.bl_idname, text="BEngine (.bengine)")

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