#include "Camera.h"
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

Camera::Camera()
{
	
}

Camera::~Camera()
{
	
}

//Get Private parts of Camera Matrixes via functions 
//status of camera
bool Camera::GetStatus()
{
	return this->status;
}
void Camera::SetStatus(bool input)
{
	this->status = input;
}
//LocalTranslation
glm::mat4x4& Camera::GetTranslation()
{
	return this->Translate;
}
void Camera::SetTranslation(glm::mat4x4& input)
{
	this->Translate = input;
}
//Local Rotation across x
float& Camera::GetAlphaX()
{
	return this->AlphaX;
}
void Camera::SetAlphaX(float& input)
{
	this->AlphaX = input;
}
//Local Rotation across y
float& Camera::GetAlphaY()
{
	return this->AlphaY;
}
void Camera::SetAlphaY(float& input)
{
	this->AlphaY = input;
}
//Local Rotation across z
float& Camera::GetAlphaZ()
{
	return this->AlphaZ;
}
void Camera::SetAlphaZ(float& input)
{
	this->AlphaZ = input;
}
//Local Rotations matrixes
void Camera::RotationX()
{
	float Alpha = glm::radians(AlphaX);
	RotateX[1][1] = cos(Alpha);
	RotateX[2][1] = -sin(Alpha);
	RotateX[1][2] = sin(Alpha);
	RotateX[2][2] = cos(Alpha);

}
void Camera::RotationY()
{
	float Alpha = glm::radians(AlphaY);
	RotateX[0][0] = cos(Alpha);
	RotateX[2][0] = -sin(Alpha);
	RotateX[0][2] = sin(Alpha);
	RotateX[2][2] = cos(Alpha);

}
void Camera::RotationZ()
{
	float Alpha = glm::radians(AlphaZ);
	RotateX[0][0] = cos(Alpha);
	RotateX[1][0] = -sin(Alpha);
	RotateX[0][1] = sin(Alpha);
	RotateX[1][1] = cos(Alpha);
}
//World Rotations matrixes
void Camera::World_RotationX()
{
	float Alpha = glm::radians(World_AlphaX);
	World_RotateX[1][1] = cos(Alpha);
	World_RotateX[2][1] = -sin(Alpha);
	World_RotateX[1][2] = sin(Alpha);
	World_RotateX[2][2] = cos(Alpha);

}
void Camera::World_RotationY()
{
	float Alpha = glm::radians(World_AlphaY);
	World_RotateY[0][0] = cos(Alpha);
	World_RotateY[2][0] = -sin(Alpha);
	World_RotateY[0][2] = sin(Alpha);
	World_RotateY[2][2] = cos(Alpha);

}
void Camera::World_RotationZ()
{
	float Alpha = glm::radians(World_AlphaZ);
	World_RotateZ[0][0] = cos(Alpha);
	World_RotateZ[1][0] = -sin(Alpha);
	World_RotateZ[0][1] = sin(Alpha);
	World_RotateZ[1][1] = cos(Alpha);
}


glm::mat4x4& Camera::GetCameraTotalTransform()
{
	return this->TotalTransform;
}
glm::mat4x4& Camera::GetCameraPerspectiveMatrix()
{
	return this->PerspectiveMatrix;
}
glm::mat4x4& Camera::GetCameraOrthographicMatrix()
{
	return this->OrthographicMatrix;
}

void Camera::SetCameraTotalTransform(glm::mat4x4& input)
{
	this->TotalTransform = input;
}
void Camera::SetPerspectiveView()
{
	this->PerspectiveMatrix = glm::perspective(glm::radians(FOV), aspect, near_cam, far_cam);
}
void Camera::SetOrthographicView()
{
	this->OrthographicMatrix = glm::ortho(left, right, down, up, near_cam, far_cam);
}
void Camera::SetFrustumMatrix()
{
	this->FrustumMatrix = glm::frustum(left, right, down, up,-near_cam,-far_cam);
}


//Calculating Total Rotation and Total Transformation
void Camera::CalculateTotalRotation()
{
	this->RotateTotal = RotateX * RotateY * RotateZ;
}

void Camera::CalculateWorldTotalRotation()
{
	this->World_RotateTotal = World_RotateX * World_RotateY * World_RotateZ;
}

void Camera::CalculateTotalTransform()
{
	
	TotalTransform = World_Translate * World_RotateTotal *Translate * RotateTotal;
}

void Camera::CalcualteWorldTransformation()
{
	World_TotalTransform =  World_Translate * World_RotateTotal;
}


