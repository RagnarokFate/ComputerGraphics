#pragma once
#include "Scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"
#include "SkyBox.h"
#include "Cubemap.h"

class Renderer
{
public:
	Renderer();	
	virtual ~Renderer();
	void Render(const Scene& scene);
	void LoadShaders();
	void Renderer::DrawMesh(MeshModel& input, mat4x4 LocalTransformation, mat4x4 WorldTransformation, mat4x4 CameraView , mat4x4 CameraProjection);
	void Renderer::DrawMesh(Scene scene, mat4x4 CameraView, mat4x4 CameraProjection);

private:
	ShaderProgram ColorShader;
	ShaderProgram CubemapShader;
	ShaderProgram SkyBoxShader;

	Cubemap Cubemap;
	SkyBox Skybox;
	
};