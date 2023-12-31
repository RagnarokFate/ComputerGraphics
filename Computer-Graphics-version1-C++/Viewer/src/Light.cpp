#include "Light.h"


Light::Light()
{
}

Light::~Light()
{
}

void Light::Ambient()
{
	this->Ambient_Color = glm::vec3(Ambient_Red, Ambient_Green, Ambient_Blue);
}
void Light::Diffuse()
{
	this->Diffuse_Color = glm::vec3(Diffuse_Red, Diffuse_Green, Diffuse_Blue);
}
void Light::Specular()
{
	this->Specular_Color = glm::vec3(Specular_Red, Specular_Green, Specular_Blue);
}
void Light::ResetTotalTransform()
{
	TotalTransformation = glm::mat4x4(1);
}
void Light::CalculateTransformation()
{
	TotalTransformation = WorldTranslate * Translate;

}
void Light::CalculateWorldTransformation()
{
	WorldTotalTransformation = WorldTranslate;
}

//Color Features Obtain:
//Ambient:
float Light::Get_RedAmbient()
{
	return this->Ambient_Red;
}
float Light::Get_GreenAmbient()
{
	return this->Ambient_Green;
}
float Light::Get_BlueAmbient()
{
	return this->Ambient_Blue;
}
//Diffuse:
float Light::Get_RedDiffuse()
{
	return this->Diffuse_Red;
}
float Light::Get_GreenDiffuse()
{
	return this->Diffuse_Green;
}
float Light::Get_BlueDiffuse()
{
	return this->Diffuse_Blue;
}
//Specular
float Light::Get_RedSpecular()
{
	return this->Specular_Red;
}
float Light::Get_GreenSpecular()
{
	return this->Specular_Green;
}
float Light::Get_BlueSpecular()
{
	return this->Specular_Blue;
}
//Specular Reflection Degree
float Light::Get_SpecularDegree()
{
	return this->SpecularDegree;
}