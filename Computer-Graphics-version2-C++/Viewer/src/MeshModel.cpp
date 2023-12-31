#include "MeshModel.h"
MeshModel::MeshModel()
{
}

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
	faces(faces),
	vertices(vertices),
	normals(normals)
{
	this->WorldTransformation.ResetTransformation();
	float Maximum = -1 * static_cast<float>(INFINITY);
	float Minimum = static_cast<float>(INFINITY);

	vec3 max = vec3(Maximum, Maximum, Maximum), min = vec3(Minimum, Minimum, Minimum);;
	for (int i = 0; i < vertices.size(); i++)
	{
		max.x = std::max(max.x, vertices[i].x);
		max.y = std::max(max.y, vertices[i].y);
		max.z = std::max(max.z, vertices[i].z);

		min.x = std::min(min.x, vertices[i].x);
		min.y = std::min(min.y, vertices[i].y);
		min.z = std::min(min.z, vertices[i].z);

	}
	this->MaxPoints = max;
	this->MinPoints = min;
	WorldTransformation.ResetTransformation();

	{
		vec3 point1, point2, point3, point4, point5, point6, point7, point8;
		point1 = vec3(MinPoints.x, MinPoints.y, MinPoints.z);
		point2 = vec3(MinPoints.x, MinPoints.y, MaxPoints.z);
		point3 = vec3(MinPoints.x, MaxPoints.y, MinPoints.z);
		point4 = vec3(MinPoints.x, MaxPoints.y, MaxPoints.z);

		point5 = vec3(MaxPoints.x, MinPoints.y, MinPoints.z);
		point6 = vec3(MaxPoints.x, MinPoints.y, MaxPoints.z);
		point7 = vec3(MaxPoints.x, MaxPoints.y, MinPoints.z);
		point8 = vec3(MaxPoints.x, MaxPoints.y, MaxPoints.z);

		EdgePoints[0] = point1;
		EdgePoints[1] = point2;
		EdgePoints[2] = point3;
		EdgePoints[3] = point4;
		EdgePoints[4] = point5;
		EdgePoints[5] = point6;
		EdgePoints[6] = point7;
		EdgePoints[7] = point8;
	}

	this->CenterPoint = vec3((max.x + min.x) / 2, (max.y + min.y) / 2, (max.z + min.z) / 2);
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

glm::vec3& MeshModel::GetVertex(int i, int j)
{
	return vertices[this->GetFace(i).GetVertexIndex(j) - float(1)];
}

glm::vec3& MeshModel::GetNormal(int i, int j)
{
	return normals[this->GetFace(i).GetNormalIndex(j) - float(1)];
}