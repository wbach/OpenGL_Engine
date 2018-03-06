#pragma once
#include "Types.h"
#include <vector>
#include "../Types/Common/Source.h"
#include "../Types/Common/Input.h"

namespace rapidxml
{
	template <class T>
	class xml_node;
	template <class T>
	class xml_document;
} // rapidxml

typedef rapidxml::xml_node<char> XMLNode;

typedef std::vector<mat4> Mat4Vector;
typedef std::vector<vec2> Vec2Vector;
typedef std::vector<vec3> Vec3Vector;
typedef std::vector<float> FloatVector;
typedef std::vector<uint32> Uint32Vector;
typedef std::vector<std::string> StrVector;

namespace GameEngine
{
	namespace Collada
	{
		enum DataTypes
		{
			VEC3,
			VEC2,
			FLOAT,
			FLOAT4x4
		};

		enum ObjectType
		{
			UNKONOWN,
			LIBRARY_CAMERAS,
			LIBRARY_IMAGES,
			LIBRARY_EFFECTS,
			LIBRARY_MATERIALS,
			LIBRARY_GOMETRIES,
			LIBRARY_ANIMATIONS,
			LIBRARY_CONTROLLERS,
			LIBRARY_VISUAL_SCENES
		};
		
		void ProcessTechnique(XMLNode* root, TechniqueCommon& technique);
		void ProcessAccessor(XMLNode* root, Accessor& accessor);
		void ProcessParam(XMLNode* root, Param& param);
		void ProcessArray(XMLNode* root, Array& dataArray);
		void ProcessSource(XMLNode* root, Source& source);
		void ProcessInput(XMLNode* root, Input& input);
		vec4 GetVector4FromString(const std::string& str);
		mat4 GetMatrixFromString(const std::string& str);
		Mat4Vector GetMatrixesFromString(const std::string& str);
		Vec2Vector GetVectors2dFromString(const std::string& str);
		vec3 GetVector3(const vec4& v);
		void GetVectors2dFromString(const std::string& str, Vec2Vector& v);
		Vec3Vector GetVectors3dFromString(const std::string& str);
		void GetVectors3dFromString(const std::string& str, Vec3Vector& vert);
		FloatVector GetFloatsFromString(const std::string& str);
		void GetFloatsFromString(const std::string& str, FloatVector& v);
		Uint32Vector GetIntsFromString(const std::string& str);
		StrVector GetStringsFromString(const std::string& str);
		std::string GetSource(const std::string& str);
	}// Collada
} // GameEngine
