#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Face.h"
#include "Transformation.h"
#include <iostream>
#include <Axis.h>
#include <vector>

class MeshModel
{
public:
	MeshModel::MeshModel();
	MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name);
	virtual ~MeshModel();
	const Face& GetFace(int index) const;
	int GetFacesCount() const;
	const std::string& GetModelName() const;
	glm::vec3& MeshModel::GetVertex(int i, int j);
	glm::vec3& MeshModel::GetNormal(int i, int j);

private:
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::string model_name;

public :
	vec3 MeshModelGridColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 MeshModelFillColor = vec3(1.0f, 0.0f, 0.0f);
	
	vec3 NormalsColor = vec3(1.0f,1.0f,1.0f);
	vec3 CenterOfFaceColor = vec3(1.0f,1.0f,1.0f);
	vec3 BoundingBoxColor = vec3(0.0f, 1.0f, 0.0f);
	vec3 WorldBoundingBoxColor = vec3(0.8f, 0.8f, 0.3f);
	vec3 FacesRectangleColor = vec3(1.0f, 1.0f, 1.0f);

	bool NormalsDraw = false, CenterOfFaceDraw = false, BoundingBoxDraw = false, OffGridDraw = false, Local_Axis = false, WorldBoundingBox = false, DrawFacesRectangles = false, FacesRectangles = false, TriangleFill = false;

	float NormalsLength = 50.0f;
	float CenterOfFaceLength = 50.0f;

	Transformation MeshModelTransformation = Transformation();
	Transformation WorldTransformation = Transformation();
	Axis MeshModelAxis = Axis();
	float AxisLength = 50.0f;

	int TriangleFillMode = 0;
	int ScanLineMode = 0;
	int WalkingEdgeMode = 0;

public:
	vec3 MinPoints;
	vec3 MaxPoints;
	vec3 CenterPoint = vec3(0.0f,0.0f,0.0f);
	vec3 UpdatedCenterPoint = vec3(0.0f,0.0f,0.0f);
	vec3 EdgePoints[8];
	
public:
	vec3 AmbientColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 DiffuseColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 SpecularColor = vec3(1.0f, 1.0f, 1.0f);

};
