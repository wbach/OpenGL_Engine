import os

import bpy
from mathutils import Matrix, Vector, Color
from bpy_extras import io_utils, node_shader_utils

from bpy_extras.wm_utils.progress_report import (
    ProgressReport,
    ProgressReportSubstep,
)

def mesh_triangulate(me):
    import bmesh
    bm = bmesh.new()
    bm.from_mesh(me)
    bmesh.ops.triangulate(bm, faces=bm.faces)
    bm.to_mesh(me)
    bm.free()

def export(context, keywords):
    print("export")

    filepath = keywords['filepath']
    with ProgressReport(context.window_manager) as progress:
        #progress.enter_substeps(100)
        with ProgressReportSubstep(progress, 2, "Export path: %r" % filepath, "Export Finished") as subprogress1:
            with open(filepath, "w", encoding="utf8", newline="\n") as f:
                fw = f.write
                objects = context.scene.objects
                fw("<model info=\"Blender exporter\">")
                for i, ob_main in enumerate(objects):
                    print(type(ob_main.type))
                    if ob_main.type == 'MESH':
                        mesh = ob_main.to_mesh()
                        mesh_triangulate(mesh)
                        fw("<mesh name=\"" + ob_main.name + "\">")
                        fw("<v>")
                        for v in mesh.vertices[:]:
                            fw('%.6f %.6f %.6f ' % v.co[:])
                        fw("</v>")

                        fw("<i>")
                        for poly in mesh.polygons:
                            fw(str(poly.vertices[0]) + " " + str(poly.vertices[1]) + " " + str(poly.vertices[2]) + " ")
                        fw("</i>")
                        fw("</mesh>")
                    else:
                        print(ob_main.type)


                fw("</model>")

    return {'FINISHED'}