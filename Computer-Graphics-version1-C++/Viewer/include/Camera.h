#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Camera
{
public:
	Camera();
	virtual ~Camera();
	//LookAt Function
	void SetCameraLookAt()
	{
		lookat = glm::lookAt(Position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	void SetCameraLookAt(glm::vec3& eye, glm::vec3& at, glm::vec3& up)
	{
		this->lookat =  glm::lookAt(eye, at, up);
	}
	//Get Private parts of Camera Matrixes via functions 
	bool GetStatus();
	void SetStatus(bool input);

	glm::mat4x4& GetTranslation();
	void SetAlphaX(float& input);
	void SetAlphaY(float& input);
	void SetAlphaZ(float& input);
	void SetTranslation(glm::mat4x4& input);
	float& GetAlphaX();
	float& GetAlphaY();
	float& GetAlphaZ();

	void Camera::RotationX();
	void Camera::RotationY();
	void Camera::RotationZ();
	glm::mat4x4& GetCameraTotalTransform();
	void SetCameraTotalTransform(glm::mat4x4& input);

	//Calculating Total Rotation and Total Transformation
	void CalculateTotalRotation();
	void CalculateTotalTransform();
	void CalcualteWorldTransformation();
	void World_RotationX();
	void World_RotationY();
	void World_RotationZ();
	void CalculateWorldTotalRotation();

	//Orthographic OR Perspective
	glm::mat4x4& GetCameraPerspectiveMatrix();
	glm::mat4x4& GetCameraOrthographicMatrix();
	void SetPerspectiveView();
	void SetOrthographicView();
	void SetFrustumMatrix();

	//CONVERT 4D TO 3D VECTOR
	glm::vec3 Camera::ConvertTo3D(glm::vec4 input)
	{
		return glm::vec3(input.x / input.z, input.y / input.z, 0);
	}
	
private:
	bool status = true;
	float AlphaX = 0, AlphaY = 0, AlphaZ = 0;
	glm::mat4x4 Translate = glm::mat4x4(1);
	glm::mat4x4 RotateX = glm::mat4x4(1);
	glm::mat4x4 RotateY = glm::mat4x4(1);
	glm::mat4x4 RotateZ = glm::mat4x4(1);
	glm::mat4x4 RotateTotal = glm::mat4x4(1);
	//total calculation of transformation for camera
public:
	//World Transformation
	float World_AlphaX = 0, World_AlphaY = 0, World_AlphaZ = 0;
	glm::mat4x4 World_Translate = glm::mat4x4(1);
	glm::mat4x4 World_RotateX = glm::mat4x4(1);
	glm::mat4x4 World_RotateY = glm::mat4x4(1);
	glm::mat4x4 World_RotateZ = glm::mat4x4(1);
	glm::mat4x4 World_RotateTotal = glm::mat4x4(1);
	glm::mat4x4 World_TotalTransform = glm::mat4x4(1);
	glm::mat4x4 TotalTransform = glm::mat4x4(1);

	//type of Camera View Matrix
	glm::mat4x4 FrustumMatrix = glm::mat4x4(1);
	glm::mat4x4 OrthographicMatrix = glm::mat4x4(1);
	glm::mat4x4 PerspectiveMatrix = glm::mat4x4(1);
	glm::mat4x4 lookat = glm::mat4x4(1);

	//boolean indicators
	bool Orthograhpic_View = true;
	bool Perspective_View = false;
	bool FrustumView = false;
	//Orthographic Features:
	public: float left = -1, right = 1, up = 1, down = -1;

	//Perspective Features:
	float near_cam = -0.1f;
	float far_cam = 1.0f;
	float FOV = 45.0f;

	//Frustum Features:
	float distance = 1;
	float viewport_width_ = float(1920);
	float viewport_height_ = float(1280);
	float aspect = viewport_width_ / viewport_height_;

	//Camera Location Features:
	public:
		glm::vec3 Position = glm::vec3(0.0f, 0.0f, 3.0f);
		glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 Up_vector = glm::vec3(0.0f, 1.0f, 0.0f);

};
