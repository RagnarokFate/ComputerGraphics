#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <iostream>
#include "Renderer.h"
#include "InitShader.h"
#include "glm//gtx/normal.hpp"


#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(width,x,y) ((x)+(y)*(width))

using namespace glm;
using namespace std;

//enum TextureMode { PlannerMapping, CylinderMapping, SphereMapping, NormalMapping, EnvironmentMapping, ToonShading};

Renderer::Renderer()
{
	this->Cubemap.CubemapSetup();
	this->Skybox.SkyBoxSetup();
	/*CubemapShader.loadShaders("cubemapvshader.glsl", "cubemapfshader.glsl");
	SkyBoxShader.loadShaders("skyboxvshader.glsl", "skyboxfshader.glsl");*/

}

Renderer::~Renderer()
{
}

void Renderer::Render(const Scene& scene)
{
	scene.GetActiveCamera().SetMainMatrix();
	scene.GetActiveCamera().SetViewTransformation();

	mat4x4 CameraView = scene.GetActiveCamera().view_transformation;
	mat4x4 CameraProjection = scene.GetActiveCamera().GetProjectionTransformation();
	vec3 CameraPosition = scene.GetActiveCamera().eye;
	for (int i = 0; i < scene.GetModelCount() && !scene.GetLightCount() ;i++)
	{
		MeshModel& CurrentMesh = scene.GetModel(i);
		
		CurrentMesh.MeshModelTransformation.SetTransformation();
		CurrentMesh.WorldTransformation.SetTransformation();

		mat4x4 Local = CurrentMesh.MeshModelTransformation.GetMainMatrix();
		mat4x4 World = CurrentMesh.WorldTransformation.GetMainMatrix();
		DrawMesh(CurrentMesh, Local, World, CameraView, CameraProjection);
	}
	
	if(scene.GetModelCount() && scene.GetLightCount())
	{
		DrawMesh(scene,CameraView,CameraProjection);
	}
	if (scene.Cubemap)
	{
		// draw scene as normal
		CubemapShader.use();
		mat4x4 temp = mat4x4(1.0f);
		CubemapShader.setUniform("model", temp);
		CubemapShader.setUniform("view", CameraView);
		CubemapShader.setUniform("projection", CameraProjection);
		CubemapShader.setUniform("cameraPos", CameraPosition);
		// cubes
		Cubemap.cubemap.bind(0);
		glBindVertexArray(Cubemap.cubemapVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Cubemap.cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		Cubemap.cubemap.unbind(0);
	}
	//skybox

	if (scene.Skybox)
	{
		// skybox cube
		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxShader.use();
		
		SkyBoxShader.setUniform("view", CameraView);
		SkyBoxShader.setUniform("projection", CameraProjection);
		Skybox.Skybox.bind(0);
		glBindVertexArray(Skybox.skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, scene.GetActiveModel().GetVBO());
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->Cubemap.cubemapVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default
		Skybox.Skybox.unbind(0);

	}
	
}

void Renderer::LoadShaders()
{
	ColorShader.loadShaders("vshader.glsl", "fshader.glsl");
	CubemapShader.loadShaders("cubemapvshader.glsl", "cubemapfshader.glsl");
	SkyBoxShader.loadShaders("skyboxvshader.glsl", "skyboxfshader.glsl");

}

//void Renderer::DrawMesh(MeshModel& input, mat4x4 LocalTransformation = mat4x4(1.0f), mat4x4 WorldTransformation = mat4x4(1.0f), mat4x4 CameraView = mat4x4(1.0f), mat4x4 CameraProjection = mat4x4(1.0f))
void Renderer::DrawMesh(MeshModel& input, mat4x4 LocalTransformation, mat4x4 WorldTransformation, mat4x4 CameraView, mat4x4 CameraProjection)
{
	mat4x4 MeshModelTransfomration = WorldTransformation * LocalTransformation;
	mat4x4 Result = CameraProjection * CameraView * MeshModelTransfomration;
	input.UpdatedCenterPoint = vec3(Result * vec4(input.CenterPoint, 1.0f));

	{
		// Activate the 'colorShader' program (vertex and fragment shaders)
		ColorShader.use();
		vec3 EmptyVec = vec3(0.0f);
		// Set the uniform variables
		ColorShader.setUniform("model", MeshModelTransfomration);
		ColorShader.setUniform("view", CameraView);
		ColorShader.setUniform("projection", CameraProjection);
		//ColorShader.setUniform("material.textureMap", 0);
		ColorShader.setUniform("TriangleFillMode", input.TriangleFillMode);
		ColorShader.setUniform("MappingMode", input.MappingMode);
		ColorShader.setUniform("MappingExtra", input.MappingExtra);
		ColorShader.setUniform("MeshModelColor", input.MeshModelFillColor);

		ColorShader.setUniform("LightPosition", EmptyVec);
		ColorShader.setUniform("LightAmbient", EmptyVec);
		ColorShader.setUniform("LightDiffuse", EmptyVec);
		ColorShader.setUniform("LightSpecular", EmptyVec);
		ColorShader.setUniform("LightSpecularDegree", 0);
		ColorShader.setUniform("LightIntensity", 1);
		ColorShader.setUniform("LightSobel", EmptyVec);

		ColorShader.setUniform("MeshModelAmbient", EmptyVec);
		ColorShader.setUniform("MeshModelDiffuse", EmptyVec);
		ColorShader.setUniform("MeshModelSpecular", EmptyVec);

		ColorShader.setUniform("CameraPosition", EmptyVec);
		//input.texture.bind(0);

		if (input.TriangleFillMode == WithLightTexture || input.TriangleFillMode == WithTextureNoLight)
		{
			// Set 'texture1' as the active texture at slot #0
			input.texture.bind(0);
		}
		if (input.TriangleFill)
		{
			// Drag our model's faces (triangles) in fill mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBindVertexArray(input.GetVAO());
			glDrawArrays(GL_TRIANGLES, 0, input.GetVerticesData().size());
			glBindVertexArray(0);
		}
		ColorShader.setUniform("color", glm::vec3(0, 0, 0));
		if (!input.OffGridDraw)
		{
			// Drag our model's faces (triangles) in line mode (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBindVertexArray(input.GetVAO());
			glDrawArrays(GL_TRIANGLES, 0, input.GetVerticesData().size());
			glBindVertexArray(0);
		}
		if (input.TriangleFillMode == WithLightTexture || input.TriangleFillMode == WithTextureNoLight)
		{
			// Unset 'texture1' as the active texture at slot #0
			input.texture.unbind(0);
		}
		//input.texture.unbind(0);

	}
	
}

void Renderer::DrawMesh(Scene scene, mat4x4 CameraView, mat4x4 CameraProjection)
{
	for (int i = 0; i < scene.GetModelCount(); i++)
	{
		MeshModel& CurrentMesh = scene.GetModel(i);
		CurrentMesh.MeshModelTransformation.SetTransformation();
		CurrentMesh.WorldTransformation.SetTransformation();

		mat4x4 Result = CurrentMesh.WorldTransformation.GetMainMatrix() * CurrentMesh.MeshModelTransformation.GetMainMatrix();
		Light& LightSource = scene.GetActiveLight();
		LightSource.Local.SetTransformation();
		LightSource.World.SetTransformation();

		mat4x4 LightMatrix = LightSource.World.GetMainMatrix() * LightSource.Local.GetMainMatrix();
		vec3 UpdatedLightPosition = vec3(LightMatrix * vec4(LightSource.Light_Position, 1));
		{
			// Activate the 'colorShader' program (vertex and fragment shaders)
			ColorShader.use();

			// Set the uniform variables
			ColorShader.setUniform("model", Result);
			ColorShader.setUniform("view", CameraView);
			ColorShader.setUniform("projection", CameraProjection);
			ColorShader.setUniform("material.textureMap", 0);
			ColorShader.setUniform("TriangleFillMode", CurrentMesh.TriangleFillMode);
			ColorShader.setUniform("MappingMode", CurrentMesh.MappingMode);
			ColorShader.setUniform("MappingExtra", CurrentMesh.MappingExtra);
			ColorShader.setUniform("MeshModelColor", CurrentMesh.MeshModelFillColor);

			ColorShader.setUniform("LightPosition", UpdatedLightPosition);
			ColorShader.setUniform("LightAmbient", LightSource.AmbientColor);
			ColorShader.setUniform("LightDiffuse", LightSource.DiffuseColor);
			ColorShader.setUniform("LightSpecular", LightSource.SpecularColor);
			ColorShader.setUniform("LightSpecularDegree", LightSource.Specular_Degree);
			ColorShader.setUniform("LightIntensity", LightSource.LightIntensity);
			ColorShader.setUniform("LightSobel", LightSource.LightSobel);

			ColorShader.setUniform("MeshModelAmbient", CurrentMesh.AmbientColor);
			ColorShader.setUniform("MeshModelDiffuse", CurrentMesh.DiffuseColor);
			ColorShader.setUniform("MeshModelSpecular", CurrentMesh.SpecularColor);

			ColorShader.setUniform("CameraPosition", scene.GetActiveCamera().eye);

			if (CurrentMesh.TriangleFillMode == WithLightTexture || CurrentMesh.TriangleFillMode == WithTextureNoLight)
			{
				// Set 'texture1' as the active texture at slot #0
				CurrentMesh.texture.bind(0);
			}
			if (!CurrentMesh.OffGridDraw)
			{
				// Drag our model's faces (triangles) in line mode (wireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glBindVertexArray(CurrentMesh.GetVAO());
				glDrawArrays(GL_TRIANGLES, 0, CurrentMesh.GetVerticesData().size());
				glBindVertexArray(0);
			}
			if (CurrentMesh.TriangleFill)
			{
				// Drag our model's faces (triangles) in fill mode
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glBindVertexArray(CurrentMesh.GetVAO());
				glDrawArrays(GL_TRIANGLES, 0, CurrentMesh.GetVerticesData().size());
				glBindVertexArray(0);
			}

			if (CurrentMesh.TriangleFillMode == WithLightTexture || CurrentMesh.TriangleFillMode == WithTextureNoLight)
			{
				// Unset 'texture1' as the active texture at slot #0
				CurrentMesh.texture.unbind(0);
			}
		}
		
	}

	

}



