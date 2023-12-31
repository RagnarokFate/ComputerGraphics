#pragma once
#include <glm/glm.hpp>
#include<iostream>
#include<iterator> // for iterators
#include<vector>

using namespace std;


class Light
{
public:
	Light();
	~Light();

	void Ambient();
	void Diffuse();
	void Specular();
	void CalculateTransformation();
	void CalculateWorldTransformation();
	//Color Features Obtain:
	//Ambient:
	float Get_RedAmbient();
	float Get_GreenAmbient();
	float Get_BlueAmbient();
	//Diffuse:
	float Get_RedDiffuse();
	float Get_GreenDiffuse();
	float Get_BlueDiffuse();
	//Specular
	float Get_RedSpecular();
	float Get_GreenSpecular();
	float Get_BlueSpecular();
	//Specular Reflection Degree
	float Get_SpecularDegree();


	//Color Features Updates:
	//Ambient:
	void Set_RedAmbient(float input)
	{
		this->Ambient_Red = input;
	}
	void Set_GreenAmbient(float input)
	{
		this->Ambient_Green = input;
	}
	void Set_BlueAmbient(float input)
	{
		this->Ambient_Blue = input;
	}
	//Diffuse:
	void Set_RedDiffuse(float input)
	{
		this->Diffuse_Red = input;
	}
	void Set_GreenDiffuse(float input)
	{
		this->Diffuse_Green = input;
	}
	void Set_BlueDiffuse(float input)
	{
		this->Diffuse_Blue = input;
	}
	//Specular
	void Set_RedSpecular(float input)
	{
		this->Specular_Red = input;
	}
	void Set_GreenSpecular(float input)
	{
		this->Specular_Green = input;
	}
	void Set_BlueSpecular(float input)
	{
		this->Specular_Blue = input;
	}
	//Specular Reflection Degree
	void Set_SpecularDegree(float input)
	{
		this->SpecularDegree = input;
	}
	void ResetTotalTransform();

public:
	glm::vec3 Position = glm::vec3(960 + 400, 720 + 80, 0);
	glm::vec3 LinePosition1 = glm::vec3(1500 ,800, 0);
	glm::vec3 LinePosition2 = glm::vec3(1500, 200, 0);
	glm::vec3 LightLineVector = glm::vec3(-1, 0, 0);

	glm::mat4x4 Translate = glm::mat4x4(1);
	glm::mat4x4 TotalTransformation = glm::mat4x4(1);
	glm::mat4x4 WorldTranslate = glm::mat4x4(1);
	glm::mat4x4 WorldTotalTransformation = glm::mat4x4(1);

	//Features Of Light
	float FlatIntensity = 1.0f;
	float GouraudIntensity = 1.0f;
	float PhongIntensity = 1.0f;



	glm::vec3 LightColor = glm::vec3(1.0f,1.0f,1.0f);

	bool LightRound = true;
	bool LightLine = false;
	bool LightRectangle = false;

	float Radius = 50;
	
	bool Status = true;
	bool ShowReflectedRays = false;

	bool Flat_Shade = false;
	bool Gouraud_Shade = false;
	bool Phong_Shade = false;
private:
	//Colors Features:
	float Ambient_Red = 0.0f;
	float Ambient_Green = 0.0f;
	float Ambient_Blue = 0.0f;

	float Diffuse_Red = 1.0f;
	float Diffuse_Green = 1.0f;
	float Diffuse_Blue = 1.0f;

	float Specular_Red   = 0.0f;
	float Specular_Green = 0.0f;
	float Specular_Blue  = 0.0f;
	float SpecularDegree = 32;

public:
	//Color Vectors
	glm::vec3 Ambient_Color = glm::vec3(Ambient_Red, Ambient_Green, Ambient_Blue);
	glm::vec3 Diffuse_Color = glm::vec3(Diffuse_Red, Diffuse_Green, Diffuse_Blue);
	glm::vec3 Specular_Color = glm::vec3(Specular_Red, Specular_Green, Specular_Blue);
	
};

