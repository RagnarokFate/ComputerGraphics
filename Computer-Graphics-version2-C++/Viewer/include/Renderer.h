#pragma once
#include "Scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

class Renderer
{
public:
	Renderer();
	Renderer(int viewportWidth, int viewportHeight);
	virtual ~Renderer();
	void Render(const Scene& scene);
	void SwapBuffers();
	void Renderer::DrawSceneAxis(Scene scene);
	void UpdateCenterPoint(mat4x4 input, MeshModel Object);
	void Renderer::DrawMeshModelFeatures(MeshModel& Object, mat4x4 Camera, mat4x4 Transformation);
	void Renderer::DrawBoundingBox(mat4x4 input, MeshModel Object);
	void Renderer::FillTriangleScanLine(vec3 Color, vec3 A, vec3 B, vec3 C);
	void Renderer::DrawFacesRectangles(vec3 A, vec3 B, vec3 C, vec3 GeneratedColor);
	void Renderer::FillTringlesWalkingEdge(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 Color);
	void ClearColorBuffer(const glm::vec3& color);
	int GetViewportWidth() const;
	int GetViewportHeight() const;
	void Renderer::DrawMeshObject(Scene scene);
	void Renderer::ApplyLightEffect(Scene scene);

	//Lighting & Shading
	void Renderer::ApplyGouraudShade(vec3 A, vec3 B, vec3 C, vec3 Color_A, vec3 Color_B, vec3 Color_C);
	void Renderer::ApplyPhongShade(vec3 A, vec3 B, vec3 C, vec3 Normal_A, vec3 Normal_B, vec3 Normal_C);
	vec3 Renderer::ColorInterpolation(vec3 A, vec3 B, vec3 C, vec3 Color_A, vec3 Color_B, vec3 Color_C, vec2 Point);
	vec3 Renderer::NormalInterpolation(vec3 A, vec3 B, vec3 C, vec3 Normal_A, vec3 Normal_B, vec3 Normal_C, vec2 Point);

	bool Renderer::CoordinatesInWindow(int x, int y);
	void Renderer::DrawLight(Light input);

	void Renderer::SetViewportHeight(int input);
	void Renderer::SetViewportWidth(int input);
	void CreateOpenglBuffer();
	void InitOpenglRendering();
	void CreateBuffers(int w, int h);

private:
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);
	void DrawIphone();
	void Renderer::Drawcircle(const glm::ivec2& p1, const float radius, const glm::vec3& color, int a);

	float* color_buffer;
	float* Z_Buffer;

	int viewport_width;
	int viewport_height;
	GLuint gl_screen_tex;
	GLuint gl_screen_vtc;
};