#pragma once
#include "Scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

class Renderer
{
public:
	Renderer(int viewportWidth, int viewportHeight);
	virtual ~Renderer();
	void Render(const Scene& scene);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	int GetViewportWidth() const;
	int GetViewportHeight() const;
	void PutPixel(const int i, const int j, const glm::vec3& color);
	void DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color);

	void DrawMesh(MeshModel& Obj, glm::vec3& color, Camera& Cam);
	void DrawMesh(MeshModel& Obj, glm::vec3& color, Camera& Cam, Light& LightSource);
	void DrawMeshFeatures(MeshModel& Obj, glm::vec3 Start, glm::vec3 Middle, glm::vec3 End, const glm::vec3& color, Camera& Cam);
	void DrawForMeshModelAxis(MeshModel& Obj);
	void DrawForWorldAxis(MeshModel& Obj);
	bool PointInTriangle(glm::ivec2 p, glm::ivec2 p0, glm::ivec2 p1, glm::ivec2 p2);
	bool IsRound(float x, float y, glm::vec2 position, float radius);


	void DrawBox(MeshModel Obj, glm::mat4x4 input, float Rectangle_X1, float Rectangle_X2, float Rectangle_Y1, float Rectangle_Y2, float Rectangle_Z1, float Rectangle_Z2, glm::vec3 Color);
	void DrawNormals(MeshModel Obj, glm::mat4x4 input, int Vector_Length, glm::vec3 color);
	void DrawNormalsPerFaces(MeshModel Obj, glm::mat4x4 input, int Vector_Length, glm::vec3 color);
	void DrawTriangleBoxs(MeshModel Obj, glm::mat4x4 input);
	void FillTringles(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 Color);
	void SubDivision(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, int subdivision, glm::ivec3 Color);
	float area(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);
	void DepthCalculationInterpolation(float Left, float Right, float current_y, glm::vec3 point1, glm::vec3 point2, glm::vec3 point3);
	void ExtractMinMax();

	void ScanLine(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3);
	float DepthCalculation(float x, float y, glm::vec3 point1, glm::vec3 point2, glm::vec3 point3);

	bool InWindow(int i, int j);
	void DrawLight(Light& input, Camera& Cam);
	void Adjust_LightPosition(Light& input);
	void DrawMeshFeatures(MeshModel& Obj, glm::vec3 Start, glm::vec3 Middle, glm::vec3 End, const glm::vec3& color, Camera& Cam, Light& LightSource);
	void FillTringles_Grouand(glm::vec3 start_point, glm::vec3 end_point, glm::vec3  middle_point, glm::vec3 Normal1_Color, glm::vec3  Normal_2_Color, glm::vec3 Normal_3_Color);
	glm::vec3 LightInterpolation(float x, float y, glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 Normal1_Color, glm::vec3  Normal_2_Color, glm::vec3 Normal_3_Color);
	glm::vec3 Renderer::NormalInterpolation(float x, float y, glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 Normal1, glm::vec3 Normal2, glm::vec3 Normal3);
	void Renderer::DrawMesh(MeshModel& Obj, glm::vec3& color, Camera& Cam, Scene& scene);

	bool FaceCuling(glm::mat4x4 input ,glm::vec3 start_point, glm::vec3 end_point, glm::vec3  middle_point, Camera& Camerainput);
	void ActiveBlurEffect(float* arr, float* result, int width, int height, Scene& scene);
	void ActiveFogEffect(float* arr, float* result, int width, int height, Scene& scene);

private:
	void CreateBuffers(int w, int h);
	void CreateOpenglBuffer();
	void InitOpenglRendering();


	//Z-Buffer Creation
	float* color_buffer;
	float* z_buffer;
	float z_min = -1 * INFINITY, z_max = INFINITY;

	float* PreviousResult, *NextResult;

	int viewport_width;
	int viewport_height;
	GLuint gl_screen_tex;
	GLuint gl_screen_vtc;
};