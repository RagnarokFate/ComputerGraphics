#include "MeshModel.h"

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
	faces(faces),
	vertices(vertices),
	normals(normals)
{
	//PrintVertexList();
	//PrintNormalsList();
	//PrintFacesList();
}
MeshModel::~MeshModel()
{
}
const Face& MeshModel::GetFace(int index) const
{
	return faces[index];
}
int MeshModel::GetFacesCount() const
{
	return faces.size();
}
const std::string& MeshModel::GetModelName() const
{
	return model_name;
}

//rotations of Mesh Model Object:
void MeshModel::RotationX()
{
	float Alpha = glm::radians(AlphaX);
	RotateX[1][1] = cos(Alpha);
	RotateX[2][1] = -sin(Alpha);
	RotateX[1][2] = sin(Alpha);
	RotateX[2][2] = cos(Alpha);

}
void MeshModel::RotationY()
{
	float Alpha = glm::radians(AlphaY);
	RotateY[0][0] = cos(Alpha);
	RotateY[2][0] = -sin(Alpha);
	RotateY[0][2] = sin(Alpha);
	RotateY[2][2] = cos(Alpha);

}
void MeshModel::RotationZ()
{
	float Alpha = glm::radians(AlphaZ);
	RotateZ[0][0] = cos(Alpha);
	RotateZ[1][0] = -sin(Alpha);
	RotateZ[0][1] = sin(Alpha);
	RotateZ[1][1] = cos(Alpha);
}

//rotations of World:
void MeshModel::WorldRotationX()
{
	float Alpha = glm::radians(WorldAlphaX);
	WorldRotateX[1][1] = cos(Alpha);
	WorldRotateX[2][1] = -sin(Alpha);
	WorldRotateX[1][2] = sin(Alpha);
	WorldRotateX[2][2] = cos(Alpha);

}
void MeshModel::WorldRotationY()
{
	float Alpha = glm::radians(WorldAlphaY);
	WorldRotateY[0][0] = cos(Alpha);
	WorldRotateY[2][0] = -sin(Alpha);
	WorldRotateY[0][2] = sin(Alpha);
	WorldRotateY[2][2] = cos(Alpha);

}
void MeshModel::WorldRotationZ()
{
	float Alpha = glm::radians(WorldAlphaZ);
	WorldRotateZ[0][0] = cos(Alpha);
	WorldRotateZ[1][0] = -sin(Alpha);
	WorldRotateZ[0][1] = sin(Alpha);
	WorldRotateZ[1][1] = cos(Alpha);
}

//move point (x,y) = (a,b)
void MeshModel::Add_To_XY(int a, int b)
{
	for (int i = 0; i < this->faces.size(); i++)
	{
		this->GetVertex(i, 0).x += a;

		this->GetVertex(i, 1).x += a;

		this->GetVertex(i, 2).x += a;

		this->GetVertex(i, 0).y += b;

		this->GetVertex(i, 1).y += b;

		this->GetVertex(i, 2).y += b;
	}
}
//instantly get vector 3D from Mesh Model Object of I Face
glm::vec3& MeshModel::GetVertex(int i, int j)
{
	return vertices[this->GetFace(i).GetVertexIndex(j) - 1];
}
glm::vec3& MeshModel::GetNormal(int i, int j)
{
	return normals[this->GetFace(i).GetNormalIndex(j) - 1];
}
glm::vec3& MeshModel::GetVertex(int j)
{
	return vertices[j];
}

//reseting values 
void MeshModel::ResetWorldRotation()
{
	WorldRotateX = glm::mat4x4(1);
	WorldRotateY = glm::mat4x4(1);
	WorldRotateZ = glm::mat4x4(1);
	CalculateWorldRotation();
}
void MeshModel::ResetMeshModelRotation()
{
	RotateX = glm::mat4x4(1);
	RotateY = glm::mat4x4(1);
	RotateZ = glm::mat4x4(1);
	CalculateRotation();

}
void MeshModel::ResetCurrentMatrix()
{
	current_matrix = glm::mat4x4(1);
}

//updating total rotation of mesh model object and world
void MeshModel::CalculateRotation()
{
	RotateTotal = RotateX * RotateY * RotateZ;
}
void MeshModel::CalculateWorldRotation()
{
	WorldRotateTotal = (WorldRotateX * WorldRotateY * WorldRotateZ);
}

//update current mesh model matrix
void MeshModel::ApplyMatrix()
{
	ApplyWorldMatrix();
	current_matrix = WorldScale * WorldTranslate * WorldRotateTotal * Translate * RotateTotal * Scale;

}
//update World matrix
void MeshModel::ApplyWorldMatrix()
{
	World_matrix = (WorldScale * WorldTranslate * WorldRotateTotal);
}

glm::mat4x4 MeshModel::Get_CurrentMeshModelMatrix()
{
	return this->current_matrix;
}
glm::mat4x4 MeshModel::Get_RotateTotalMeshModel()
{
	return this->RotateTotal;
}
glm::mat4x4 MeshModel::Get_TranslateMeshModelMatrix()
{
	return Translate;
}
glm::mat4x4 MeshModel::Get_ScaleMeshModelMatrix()
{
	return Scale;
}
float& MeshModel::Get_AlphaXMeshModelMatrix()
{
	return AlphaX;
}
float& MeshModel::Get_AlphaYMeshModelMatrix()
{
	return AlphaY;
}
float& MeshModel::Get_AlphaZMeshModelMatrix()
{
	return AlphaZ;
}
float& MeshModel::Get_WorldAlphaXMeshModelMatrix()
{
	return WorldAlphaX;
}
float& MeshModel::Get_WorldAlphaYMeshModelMatrix()
{
	return WorldAlphaY;
}
float& MeshModel::Get_WorldAlphaZMeshModelMatrix()
{
	return WorldAlphaZ;
}
bool MeshModel::GetStatus()
{
	return status;
}

//set functions of private mesh model parts
void MeshModel::Set_AlphaXMeshModelMatrix(float input)
{
	this->AlphaX = input;
}
void MeshModel::Set_AlphaYMeshModelMatrix(float input)
{
	this->AlphaY = input;
}
void MeshModel::Set_AlphaZMeshModelMatrix(float input)
{
	this->AlphaZ = input;
}
void MeshModel::Set_TranslateMeshModelMatrix(glm::mat4x4 input)
{
	this->Translate = input;
}
void MeshModel::Set_Status(bool input)
{
	this->status = input;
}
void MeshModel::Set_ScaleMeshModelMatrix(glm::mat4x4 input)
{
	this->Scale = input;
}

void MeshModel::Set_WorldAlphaXMeshModelMatrix(float input)
{
	this->AlphaX = input;
}
void MeshModel::Set_WorldAlphaYMeshModelMatrix(float input)
{
	this->AlphaY = input;
}
void MeshModel::Set_WorldAlphaZMeshModelMatrix(float input)
{
	this->AlphaZ = input;
}

glm::vec3 MeshModel::Get_ZeroPoint()
{
	return ZeroPoint;
}
void MeshModel::ExtractZeroPoint()
{
	glm::vec3 Center_Point;
	float Min_X, Max_X;
	float Min_Y, Max_Y;
	float Min_Z, Max_Z;

	//initializing of paramters

	Min_X = vertices[0].x;
	Max_X = vertices[0].x;
	Min_Y = vertices[0].y;
	Max_Y = vertices[0].y;
	Min_Z = vertices[0].z;
	Max_Z = vertices[0].z;
	//____________________________________________

	for (int i = 0; i < vertices.size(); i++)
	{
		if (Max_X < vertices[i].x)
		{
			Max_X = vertices[i].x;
		}
		if (Min_X > vertices[i].x)
		{
			Min_X = vertices[i].x;
		}

		if (Max_Y < vertices[i].y)
		{
			Max_Y = vertices[i].y;
		}
		if (Min_Y > vertices[i].y)
		{
			Min_Y = vertices[i].y;
		}

		if (Max_Z < vertices[i].z)
		{
			Max_Z = vertices[i].z;
		}
		if (Min_Z > vertices[i].z)
		{
			Min_Z = vertices[i].z;
		}
	}
	this->Rectangle_X2 = Max_X, this->Rectangle_X1 = Min_X, this->Rectangle_Y2 = Max_Y, this->Rectangle_Y1 = Min_Y, this->Rectangle_Z2 = Max_Z, this->Rectangle_Z1 = Min_Z;
	Center_Point = glm::vec3((Max_X + Min_X) / 2, (Max_Y + Min_Y) / 2, (Max_Z + Min_Z) / 2);

	this->ZeroPoint = Center_Point;
}
void MeshModel::Reset_ZeroPoint()
{
	ExtractZeroPoint();
}
void MeshModel::Set_ZeroPoint(glm::vec3& input)
{
	this->ZeroPoint = input;
}

//X-Y-Z AXIS of mesh modeL!

//X-Y-Z AXIS of World!

//Lights
void MeshModel::Ambient()
{
	this->Ambient_Color = glm::vec3(Ambient_Red, Ambient_Green, Ambient_Blue);
}
void MeshModel::Diffuse()
{
	this->Diffuse_Color = glm::vec3(Diffuse_Red, Diffuse_Green, Diffuse_Blue);
}
void MeshModel::Specular()
{
	this->Specular_Color = glm::vec3(Specular_Red, Specular_Green, Specular_Blue);
}





