bl_info = {
    "name": "XML Animation Exporter",
    "author": "Gemini & Peer",
    "version": (1, 0),
    "blender": (3, 0, 0),
    "location": "File > Export",
    "description": "Eksportuje akcje armatury do plików XML (OpenGL Y-up)",
    "category": "Import-Export",
}

import bpy
import os
import xml.etree.ElementTree as ET
from xml.dom import minidom
from mathutils import Matrix
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty

class ExportAnimationToXML(bpy.types.Operator, ExportHelper):
    """Eksportuj wszystkie akcje Armatury do osobnych plików XML (OpenGL Y-up)"""
    bl_idname = "export_anim.xml_folder"
    bl_label = "Eksportuj Animacje (XML)"
    
    filename_ext = ""
    use_filter_folder = True
    
    directory: StringProperty(
        name="Folder docelowy",
        description="Wybierz folder, w którym zostaną zapisane pliki XML",
        subtype='DIR_PATH'
    )

    def execute(self, context):
        obj = context.active_object
        if not obj or obj.type != 'ARMATURE':
            self.report({'ERROR'}, "Zaznacz armaturę przed eksportem!")
            return {'CANCELLED'}

        global_matrix = Matrix(((1, 0, 0, 0),
                                (0, 0, 1, 0),
                                (0, -1, 0, 0),
                                (0, 0, 0, 1)))

        scene = context.scene
        fps = scene.render.fps
        original_action = obj.animation_data.action if obj.animation_data else None

        for action in bpy.data.actions:
            obj.animation_data.action = action
            start_frame = int(action.frame_range[0])
            end_frame = int(action.frame_range[1])
            duration = (end_frame - start_frame) / fps

            root = ET.Element("AnimationClip", name=action.name, length=f"{duration:.6f}")
            keyframes_xml = ET.SubElement(root, "KeyFrames", count=str(end_frame - start_frame + 1))

            for f in range(start_frame, end_frame + 1):
                scene.frame_set(f)
                kf_xml = ET.SubElement(keyframes_xml, "KeyFrame")
                ET.SubElement(kf_xml, "timeStamp").text = f"{(f - start_frame) / fps:.6f}"
                transforms_xml = ET.SubElement(kf_xml, "Transforms")
                
                for p_bone in obj.pose.bones:
                    world_matrix = obj.matrix_world @ p_bone.matrix
                    if p_bone.parent:
                        parent_world_matrix = obj.matrix_world @ p_bone.parent.matrix
                        local_matrix = parent_world_matrix.inverted() @ world_matrix
                    else:
                        local_matrix = global_matrix @ world_matrix

                    loc, rot, sca = local_matrix.decompose()

                    t_xml = ET.SubElement(transforms_xml, "Transform", jointName=p_bone.name)
                    ET.SubElement(t_xml, "Position", x=f"{loc.x:.6f}", y=f"{loc.y:.6f}", z=f"{loc.z:.6f}")
                    ET.SubElement(t_xml, "Rotation", x=f"{rot.x:.6f}", y=f"{rot.y:.6f}", z=f"{rot.z:.6f}", w=f"{rot.w:.6f}")
                    ET.SubElement(t_xml, "Scale", x=f"{sca.x:.6f}", y=f"{sca.y:.6f}", z=f"{sca.z:.6f}")

            file_name = "".join([c for c in action.name if c.isalnum() or c in (' ', '.', '_')]).rstrip()
            filepath = os.path.join(self.directory, f"{file_name}.xml")
            
            xml_str = minidom.parseString(ET.tostring(root)).toprettyxml(indent="   ")
            with open(filepath, "w") as f:
                f.write(xml_str)

        obj.animation_data.action = original_action
        self.report({'INFO'}, f"Wyeksportowano {len(bpy.data.actions)} akcji.")
        return {'FINISHED'}

def menu_func_export(self, context):
    self.layout.operator(ExportAnimationToXML.bl_idname, text="XML Animations Folder (.xml)")

def register():
    bpy.utils.register_class(ExportAnimationToXML)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

def unregister():
    bpy.utils.unregister_class(ExportAnimationToXML)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register()