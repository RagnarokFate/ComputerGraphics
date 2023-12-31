#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Face.h"
#include<iostream>
#include<iterator> // for iterators
#include<vector> // for vectors
#define VectorLength 200
using namespace std;

class MeshModel
{
public:
	  //decleration of float rotation degrees:
      float WorldAlphaX = 0, WorldAlphaY = 0, WorldAlphaZ = 0;

	  //declerations of matrixes of World
	  glm::mat4x4 World_matrix = glm::mat4x4(1);
	  glm::mat4x4 WorldTranslate = glm::mat4x4(1);
	  glm::mat4x4 WorldScale = glm::mat4x4(1);

	MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name);
	virtual ~MeshModel();
	const Face& GetFace(int index) const;
	int GetFacesCount() const;
	const std::string& GetModelName() const;
   
	//new functions for Mesh Model
	//getting Vector 3D from the mesh model (this) face i and vertex j
	glm::vec3& MeshModel::GetVertex(int i, int j);
	glm::vec3& MeshModel::GetVertex(int j);
	glm::vec3& MeshModel::GetNormal(int i, int j);
	void MeshModel::Add_To_XY(int a, int b);

	//rotations of mesh model
	void MeshModel::RotationX();
	void MeshModel::RotationY();
	void MeshModel::RotationZ();

	//rotations of World
	void MeshModel::WorldRotationX();
	void MeshModel::WorldRotationY();
	void MeshModel::WorldRotationZ();

	//reset Default values:
	void MeshModel::ResetCurrentMatrix();
	void MeshModel::ResetWorldRotation();
	void MeshModel::ResetMeshModelRotation();
	//update default values:
	void MeshModel::CalculateRotation();
	void MeshModel::CalculateWorldRotation();
	
	//appling the changes on the Mesh Model Main Matrix 4x4
	void MeshModel::ApplyMatrix();
	//update World matrix
	void MeshModel::ApplyWorldMatrix();

	//get functions of private Mesh Model parts
	glm::mat4x4 MeshModel::Get_CurrentMeshModelMatrix();
	glm::mat4x4 MeshModel::Get_TranslateMeshModelMatrix();
	glm::mat4x4 MeshModel::Get_ScaleMeshModelMatrix();
	//Alpha of Mesh Model Rotation :
	float& MeshModel::Get_AlphaXMeshModelMatrix();
	float& MeshModel::Get_AlphaYMeshModelMatrix();
	float& MeshModel::Get_AlphaZMeshModelMatrix();
    //Alpha of World Rotation:
	float& MeshModel::Get_WorldAlphaXMeshModelMatrix();
	float& MeshModel::Get_WorldAlphaYMeshModelMatrix();
	float& MeshModel::Get_WorldAlphaZMeshModelMatrix();
	//Get/Update Mesh Model Status:
	bool MeshModel::GetStatus();
	void MeshModel::Set_Status(bool input);

	//set functions of private mesh model parts

	void MeshModel::Set_AlphaXMeshModelMatrix(float input);
	void MeshModel::Set_AlphaYMeshModelMatrix(float input);
	void MeshModel::Set_AlphaZMeshModelMatrix(float input);
	void MeshModel::Set_TranslateMeshModelMatrix(glm::mat4x4 input);	
	void MeshModel::Set_ScaleMeshModelMatrix(glm::mat4x4 input);
	//Alpha Changes for World Rotation
	void MeshModel::Set_WorldAlphaXMeshModelMatrix(float input);
	void MeshModel::Set_WorldAlphaYMeshModelMatrix(float input);
	void MeshModel::Set_WorldAlphaZMeshModelMatrix(float input);
	//__________________________________________________________________________-
	//Rotations across world
	glm::mat4x4 WorldRotateX = glm::mat4x4(1);
	glm::mat4x4 WorldRotateY = glm::mat4x4(1);
	glm::mat4x4 WorldRotateZ = glm::mat4x4(1);
	glm::mat4x4 WorldRotateTotal = glm::mat4x4(1);
	//__________________________________________________________________________-
	//Center Point Properities
	glm::vec3 MeshModel::Get_ZeroPoint();
	void MeshModel::ExtractZeroPoint();
	void MeshModel::Set_ZeroPoint(glm::vec3& input);
	void MeshModel::Reset_ZeroPoint();
	//X-Y-Z AXIS of mesh modeL!
	void Adjust_Axis()
	{
		this->X_Axis.x = ZeroPoint.x + VectorLength;
		this->X_Axis.y = ZeroPoint.y;
		this->X_Axis.z = ZeroPoint.z;

		this->Y_Axis.x = ZeroPoint.x;
		this->Y_Axis.y = ZeroPoint.y + VectorLength;
		this->Y_Axis.z = ZeroPoint.z;

		this->Z_Axis.x = ZeroPoint.x;
		this->Z_Axis.y = ZeroPoint.y;
		this->Z_Axis.z = ZeroPoint.z + VectorLength;

	}

	glm::vec3& Get_XAxis()
	{
		return X_Axis;
	}
	void Set_XAxis(glm::vec3& input)
	{
		this->X_Axis = input;
	}
	glm::vec3& Get_YAxis()
	{
		return Y_Axis;
	}
	void Set_YAxis(glm::vec3& input)
	{
		this->Y_Axis = input;
	}
	glm::vec3& Get_ZAxis()
	{
		return Z_Axis;
	}
	void Set_ZAxis(glm::vec3& input)
	{
		this->Z_Axis = input;
	}
	glm::mat4x4 MeshModel::Get_RotateTotalMeshModel();


	////Lights

	void Ambient();
	void Diffuse();
	void Specular();
	void TotalEffect();

private:
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::string model_name;

	//declaring rotations across axis
    float AlphaX = 0, AlphaY = 0, AlphaZ = 0;
	glm::vec3 ZeroPoint = glm::vec3(0, 0, 0);
	glm::vec3 X_Axis, Y_Axis, Z_Axis;
	
	//effects
	public:
	int SubDivision = 1;
	bool Color_MeshModel_Triangles = false;
	bool FillColor = false;
	bool FillBlackGray = false;
	bool OffGrid = false;

	public:
	glm::vec3 WorldX_axis, WorldY_Axis, WorldZ_Axis;
	bool RectangleStatus = false;
	bool NormalStatus = false;
	bool NormalStatusPerFaces = false;
	bool AxisStatus = false;
	bool WorldAxisStatus = false;
	bool VerticesColorful = false;
	bool ApplyTexture = false;
	glm::vec3 Mesh_Model_Color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 Normals_Mesh_Model_Color = glm::vec3(0.0f, 0.0f, 0.0f);
	float Rectangle_X1 = 0.0f, Rectangle_X2 = 0.0f, Rectangle_Y1 = 0.0f, Rectangle_Y2 = 0.0f, Rectangle_Z1 = 0.0f, Rectangle_Z2 = 0.0f;

private:
	//Mesh Model Matrixes
	glm::mat4x4 current_matrix = glm::mat4x4(1);
	glm::mat4x4 Scale = glm::mat4x4(1);
	glm::mat4x4 Translate = glm::mat4x4(1);
	//Rotations across on Mesh Model
	glm::mat4x4 RotateX = glm::mat4x4(1);
	glm::mat4x4 RotateY = glm::mat4x4(1);
	glm::mat4x4 RotateZ = glm::mat4x4(1);
	glm::mat4x4 RotateTotal = glm::mat4x4(1);

	bool status = true;
	

	public:
	//Lights
	//Colors Features:
		float Ambient_Red = 1.0f;
		float Ambient_Green = 1.0f;
		float Ambient_Blue = 1.0f;

		float Diffuse_Red = 1.0f;
		float Diffuse_Green = 1.0f;
		float Diffuse_Blue = 1.0f;

		float Specular_Red = 1.0f;
		float Specular_Green = 1.0f;
		float Specular_Blue = 1.0f;

	//Color Vectors
	glm::vec3 Ambient_Color = glm::vec3(Ambient_Red, Ambient_Green, Ambient_Blue);
	glm::vec3 Diffuse_Color = glm::vec3(Diffuse_Red, Diffuse_Green, Diffuse_Blue);
	glm::vec3 Specular_Color = glm::vec3(Specular_Red, Specular_Green, Specular_Blue);
	//diffuse material reflection
	float Material = 40.0f;
	//Light Options
};
