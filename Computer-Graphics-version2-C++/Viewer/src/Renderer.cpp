#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <iostream>
#include "Renderer.h"
#include "InitShader.h"
#include "glm//gtx/normal.hpp"

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(width,x,y) ((x)+(y)*(width))

typedef enum TriangleFillMode{ScanLine,WalkingEdge};
typedef enum ScanLineMode { Black_and_White, GrayScale , ColorFul };
typedef enum WalkingEdgeMode { Constant , Random , Constant_With_Depth};

static vec2 Minimum = vec2(0.0f, 0.0f);
static vec2 Maximum = vec2(1920.0f, 1080.0f);

//Function declearations 
double triangleArea(vec3 A, vec3 B, vec3 C);
//bool pointInTriangle(vec3 A, vec3 B, vec3 C, vec3 P);
bool PointInTriangle(vec3 P, vec3 A, vec3 B, vec3 C);
int GetIndex(int Width, int Height, int i, int j);
float Z_ValueInterpolation(vec3 A, vec3 B, vec3 C, vec2 D);
bool FaceCulling(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, Camera CurrentCamera);

Renderer::Renderer()
{
	
}

Renderer::Renderer(int viewport_width, int viewport_height) :
	viewport_width(viewport_width),
	viewport_height(viewport_height)
{
	InitOpenglRendering();
	CreateBuffers(viewport_width, viewport_height);
}

Renderer::~Renderer()
{
	delete[] color_buffer;
}

void Renderer::PutPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewport_width) return;
	if (j < 0) return; if (j >= viewport_height) return;
	
	color_buffer[INDEX(viewport_width, i, j, 0)] = color.x;
	color_buffer[INDEX(viewport_width, i, j, 1)] = color.y;
	color_buffer[INDEX(viewport_width, i, j, 2)] = color.z;
}
void Renderer::DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color)
{
	// TODO: Implement bresenham algorithm
	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
	
	//-------------------------------------
	float x1 = p1.x, x2 = p2.x, y1 = p1.y, y2 = p2.y ;
	if (x1 > x2) {//switch point if x2<x1
		float tmp = x2;
		x2 = x1;
		x1 = tmp;
		float tmps = y2;
		y2 = y1;
		y1 = tmps;
		
	}
	float dy = y2 - y1;
	float dx = x2 - x1;
	float a = dy / dx;
	float c = y1 - a * x1;
	float e;
	if (x1 == x2)//straight line case 
	{
		while (y1 != y2)
		{
		
			PutPixel(x1, y1, color);
			if (y1 > y2)
			{
				y1--;
			}
			else
			{
				y1++;
			}
		}
		return;
	}
	else if (a >= 0 && a < 1) {//
		//x1 = p1.x, x2 = p2.x, y1 = p1.y, y2 = p2.y;
		e = 2 * dy * x1 + 2 * dx * c - 2 * dx * y1 - dx;

		while (x1 <= x2)
		{
			if (e > 0)
			{
				y1 = y1 + 1;
				e = e - 2 * dx;
			}
			PutPixel(x1, y1, color);
			x1 = x1 + 1;
			e = e + 2 * dy;

		}
	}
	else if (a >= 1)//we need to swap the cordinates of the starting pint and the ending point of the line
	{
		//x1 = p1.x, x2 = p2.x, y1 = p1.y, y2 = p2.y;
		e = 2 * dy * x1 + 2 * dx * c - 2 * dx * y1 - dx;

		while (x1 <= x2) {
			if (e < 0) {
				x1 = x1 + 1;
				e = e + 2 * dy;
			}
			PutPixel(x1, y1, color);
			y1 = y1 + 1;
			e = e - 2 * dx;
		}
	}

	else if (a<=0 && a>-1) {
		e = 2 * a * x1 + 2  * c - 2  * y1 - 1;
		while (x1 <= x2) {
			if (e < 0) {
				y1=y1-1;
				e = e + 2 ;
			}
			PutPixel(x1, y1, color);
			x1 = x1 + 1;
			e = e + 2 *a;
		}
	}
	else if (a <= -1) {
			x1 = p1.y, x2 = p2.y, y1 = p1.x, y2 = p2.x;
			if (x1>x2)
			{
				x1 = p2.y, x2 = p1.y, y1 = p2.x, y2 = p1.x;
			}
			dx = x2 - x1;
			dy = y2 - y1;
			e = -1 * dx;
			//problem when x1 reachs x2 value : 
			while (x1 <= x2)
			{
				if (e > 0)
				{
					y1 = y1 - 1;
					e -= dx;
				}
				PutPixel(y1, x1, color);
				x1 = x1 + 1;
				e -= dy;
			}
		}
	
}
void Renderer::Drawcircle(const glm::ivec2& p1, const float radius, const glm::vec3& color,int a)
{
	float x1 = p1.x;
	float y1 = p1.y;
	for (int i = 0; i < a; i++)
	{
		float x2 = float(x1 + radius * sin(2 * M_PI * i / a));
		float y2 = float(y1 + radius * cos(2 * M_PI * i / a));
		glm::vec2 p2 = glm::vec2(x2, y2);
		DrawLine(p1, p2, color);
	}
	return;
}
void Renderer::DrawIphone()
{
	for (int i = 1; i < 40; i++)
	{
		DrawLine(glm::vec2(370 + i, 600), glm::vec2(370 + i, 200), glm::vec3(0, 0, 0));
		DrawLine(glm::vec2(600 - i, 600), glm::vec2(600 - i, 200), glm::vec3(0, 0, 0));
		DrawLine(glm::vec2(370, 600 - i), glm::vec2(600, 600 - i), glm::vec3(0, 0, 0));
		DrawLine(glm::vec2(370, 200 + i), glm::vec2(600, 200 + i), glm::vec3(0, 0, 0));
	}
	Drawcircle(glm::vec2(485, 220), 10, glm::vec3(1, 1, 1),999);
	for (int i = 1; i < 20; i++) {
		DrawLine(glm::vec2(370 + i, 650), glm::vec2(370 + i, 600), glm::vec3(0, 0, 0));
		DrawLine(glm::vec2(600 - i, 650), glm::vec2(600 - i, 600), glm::vec3(0, 0, 0));
		DrawLine(glm::vec2(390, 650 - i), glm::vec2(600, 650 - i), glm::vec3(0, 0, 0));
	}
	Drawcircle(glm::vec2(520, 615), 7, glm::vec3(0, 0, 0),999);
	for (int i = 1; i < 10; i++)
		DrawLine(glm::vec2(475, 618 - i), glm::vec2(500, 618 - i), glm::vec3(0, 0, 0));
	Drawcircle(glm::vec2(800, 200), 100, glm::vec3(0, 0, 0),60);

	return;
}
void Renderer::CreateBuffers(int w, int h)
{
	CreateOpenglBuffer(); //Do not remove this line.
	color_buffer = new float[3 * w * h];
	Z_Buffer = new float[w * h];

	for (int i = 0; i < w * h; i++)
	{
		Z_Buffer[i] = static_cast<float>(INFINITY);
	}
	
	ClearColorBuffer(glm::vec3(0.0f, 0.0f, 0.0f));
}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.

void Renderer::InitOpenglRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &gl_screen_tex);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &gl_screen_vtc);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(gl_screen_vtc);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[]={
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"),0);
}

void Renderer::CreateOpenglBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewport_width, viewport_height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewport_width, viewport_height);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewport_width, viewport_height, GL_RGB, GL_FLOAT, color_buffer);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(gl_screen_vtc);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewport_width; i++)
	{
		for (int j = 0; j < viewport_height; j++)
		{
			PutPixel(i, j, color);
		}
	}
}

void Renderer::Render(const Scene& scene)
{
	// TODO: Replace this code with real scene rendering code
	int half_width = viewport_width / 2;
	int half_height = viewport_height / 2;
	// draw circle
	if (scene.GetLightCount() == 0)
		DrawMeshObject(scene);
	else
		ApplyLightEffect(scene);

	if (scene.World_Axis)
	{
		DrawSceneAxis(scene);
	}

	if (scene.GetModelCount())
	{
		MeshModel& CurrentMeshModel = scene.GetActiveModel();
		if (CurrentMeshModel.TriangleFillMode == ScanLine)
		{
			if (CurrentMeshModel.ScanLineMode == Black_and_White)
			{
				float MinimumZ = INFINITY;
				float MaximumZ = -1 * INFINITY;
				for (int i = Minimum.x; i < Maximum.x; i++)
				{
					for (int j = Minimum.y; j < Maximum.y; j++)
					{
						if (CoordinatesInWindow(i, j))
						{
							float Z_Value = Z_Buffer[Z_INDEX(viewport_width, i, j)];
							if (Z_Value != INFINITY && Z_Value != -1 * INFINITY)
							{
								MinimumZ = std::min(MinimumZ, Z_Value);
								MaximumZ = std::max(MaximumZ, Z_Value);
							}
						}
					}
				}
				//float Average = MinimumZ + MaximumZ / 2;

				for (int i = Minimum.x; i < Maximum.x; i++)
				{
					for (int j = Minimum.y; j < Maximum.y; j++)
					{
						if (CoordinatesInWindow(i, j))
						{
							float depth = Z_Buffer[Z_INDEX(viewport_width, i, j)];
							float a = 1 - (depth - MinimumZ) / (MaximumZ - MinimumZ);

							if (depth != INFINITY)
							{
								if (a >= 0.5)
								{
									PutPixel(i, j, vec3(1.0f, 1.0f, 1.0f));

								}
								else
								{
									PutPixel(i, j, vec3(0.0f, 0.0f, 0.0f));
								}
								Z_Buffer[Z_INDEX(viewport_width, i, j)] = INFINITY;


							}
						}
					}
				}
			}
			if (CurrentMeshModel.ScanLineMode == GrayScale)
			{
				float MinimumZ = INFINITY;
				float MaximumZ = -1 * INFINITY;
				for (int i = Minimum.x; i < Maximum.x; i++)
				{
					for (int j = Minimum.y; j < Maximum.y; j++)
					{
						if (CoordinatesInWindow(i, j))
						{
							float Z_Value = Z_Buffer[Z_INDEX(viewport_width, i, j)];
							if (Z_Value != INFINITY && Z_Value != -1 * INFINITY)
							{
								MinimumZ = std::min(MinimumZ, Z_Value);
								MaximumZ = std::max(MaximumZ, Z_Value);
							}	
						}
					}
				}

				for (int i = Minimum.x; i < Maximum.x; i++)
				{
					for (int j = Minimum.y; j < Maximum.y; j++)
					{
						if (Z_Buffer[Z_INDEX(viewport_width, i, j)] != INFINITY && CoordinatesInWindow(i, j))
						{
							float Z_Value = Z_Buffer[Z_INDEX(viewport_width, i, j)];
							//float a =  1 - clamp(Z_Value, MinimumZ, MaximumZ);
							float a = 1 - (Z_Value - MinimumZ) / (MaximumZ - MinimumZ);
							PutPixel(i, j, vec3(a, a, a));
							Z_Buffer[Z_INDEX(viewport_width, i, j)] = INFINITY;

						}

					}
				}
			}
		}
	}

	if (scene.GetLightCount())
	{
		for (int i = 0; i < scene.GetLightCount(); i++)
			DrawLight(scene.GetLight(i));
	}
}

int Renderer::GetViewportWidth() const
{
	return viewport_width;
}

int Renderer::GetViewportHeight() const
{
	return viewport_height;
}

void Renderer::SetViewportHeight(int input)
{
	this->viewport_height = input;
}

void Renderer::SetViewportWidth(int input)
{
	this->viewport_width = input;
}

void Renderer::DrawSceneAxis(Scene scene)
{
	vec3 CenterPoint = vec3((float)this->GetViewportWidth()/2, (float)this->GetViewportHeight()/2, 0);
	Camera& CurrentCam = scene.GetActiveCamera();
	CurrentCam.SetViewTransformation();
	mat4x4 Result = CurrentCam.view_transformation * CurrentCam.GetProjectionTransformation() * scene.GetActiveModel().WorldTransformation.MainMatrix;

	vec4 X_World = vec4(Result * vec4(scene.X_World, 1.0f));
	vec4 Y_World = vec4(Result * vec4(scene.Y_World, 1.0f));
	vec4 Z_World = vec4(Result * vec4(scene.Z_World, 1.0f));

	X_World = glm::normalize(X_World);
	Y_World = glm::normalize(Y_World);
	Z_World = glm::normalize(Z_World);

	DrawLine(CenterPoint, CenterPoint + scene.World_Length * vec3((X_World) / X_World.w), vec3(1.0f, 0.0f, 0.0f));
	DrawLine(CenterPoint, CenterPoint + scene.World_Length * vec3((Y_World) / Y_World.w), vec3(0.0f, 1.0f, 0.0f));
	DrawLine(CenterPoint, CenterPoint + scene.World_Length * vec3((Z_World) / Z_World.w), vec3(0.0f, 0.0f, 1.0f));

}

void Renderer::DrawMeshObject(Scene scene) {
	Camera& CurrentCamera = scene.GetActiveCamera();
	CurrentCamera.SetViewTransformation();
	CurrentCamera.SetMainMatrix();
	mat4x4 CameraMatrix = CurrentCamera.MainMatrix;

	for (int i = 0; i < scene.GetModelCount(); i++)
	{	
		MeshModel& MeshObject = scene.GetModel(i);

		MeshObject.MeshModelTransformation.SetTransformation();
		MeshObject.WorldTransformation.SetTransformation();

		mat4x4 WorldXLocalMatrix = (MeshObject.WorldTransformation.GetMainMatrix()) * MeshObject.MeshModelTransformation.GetMainMatrix();
		MeshObject.UpdatedCenterPoint = vec3(CameraMatrix * WorldXLocalMatrix * vec4(MeshObject.CenterPoint, 1.0f));
		DrawMeshModelFeatures(MeshObject, CameraMatrix, WorldXLocalMatrix);

		//vec2 Max_Coordinates = -1.0f * vec2(static_cast<float>INFINITY, static_cast<float>INFINITY);
		//vec2 Min_Coordinates = -1.0f * vec2(static_cast<float>INFINITY, static_cast<float>INFINITY);
		vec2 Max_Coordinates = vec2(0.0f,0.0f);
		vec2 Min_Coordinates = vec2(this->GetViewportWidth(),this->GetViewportHeight());

		float Z_Minimum = MeshObject.MinPoints.z; float Z_Maximum = MeshObject.MaxPoints.z;

		//glm::vec3 FirstPoint = vec3(0.0f), SecondPoint = vec3(0.0f), ThirdPoint = vec3(0.0f);
		//glm::vec4 FirstTransformedPoint = vec4(0.0f), SecondTransformedPoint = vec4(0.0f), ThirdTransformedPoint = vec4(0.0f);

		for (int i = 0; i < MeshObject.GetFacesCount(); i++)
		{
			vec3 FirstPoint = MeshObject.GetVertex(i, 0);
			vec3 SecondPoint = MeshObject.GetVertex(i, 1);
			vec3 ThirdPoint = MeshObject.GetVertex(i, 2);
			
			vec4 FirstTransformedPoint = CameraMatrix * WorldXLocalMatrix * vec4(FirstPoint, 1.0f);
			vec4 SecondTransformedPoint = CameraMatrix * WorldXLocalMatrix * vec4(SecondPoint, 1.0f);
			vec4 ThirdTransformedPoint = CameraMatrix * WorldXLocalMatrix * vec4(ThirdPoint, 1.0f);

			if (FirstTransformedPoint.w != 0 && SecondTransformedPoint.w != 0 && ThirdTransformedPoint.w != 0 && CurrentCamera.ProjectionStatus != Perspective)
			{
				FirstTransformedPoint /= FirstTransformedPoint.w;
				SecondTransformedPoint /= SecondTransformedPoint.w;
				ThirdTransformedPoint /= ThirdTransformedPoint.w;
			}

			FirstPoint = FirstTransformedPoint;
			SecondPoint = SecondTransformedPoint;
			ThirdPoint = ThirdTransformedPoint;

			Z_Minimum = std::min(Z_Minimum,std::min(FirstPoint.z, std::min(SecondPoint.z, ThirdPoint.z)));
			Z_Maximum = std::max(Z_Maximum,std::max(FirstPoint.z, std::max(SecondPoint.z, ThirdPoint.z)));

			if (FaceCulling(FirstPoint, SecondPoint, ThirdPoint, CurrentCamera))
			{

				Max_Coordinates.x = std::max(std::max(std::max(FirstPoint.x, SecondPoint.x), ThirdPoint.x), Max_Coordinates.x);
				Min_Coordinates.x = std::min(std::min(std::min(FirstPoint.x, SecondPoint.x), ThirdPoint.x), Min_Coordinates.x);

				Max_Coordinates.y = std::max(std::max(std::max(FirstPoint.y, SecondPoint.y), ThirdPoint.y), Max_Coordinates.y);
				Min_Coordinates.y = std::min(std::min(std::min(FirstPoint.y, SecondPoint.y), ThirdPoint.y), Min_Coordinates.y);

				if (!MeshObject.OffGridDraw)
				{
					Renderer::DrawLine(vec2(FirstPoint.x, FirstPoint.y), vec2(SecondPoint.x, SecondPoint.y), MeshObject.MeshModelGridColor);
					Renderer::DrawLine(vec2(FirstPoint.x, FirstPoint.y), vec2(ThirdPoint.x, ThirdPoint.y), MeshObject.MeshModelGridColor);
					Renderer::DrawLine(vec2(ThirdPoint.x, ThirdPoint.y), vec2(SecondPoint.x, SecondPoint.y), MeshObject.MeshModelGridColor);
				}
				if (MeshObject.DrawFacesRectangles)
				{
					/*float red = (rand() % 256) / 256.0f;
					float green = (rand() % 256) / 256.0f;
					float blue = (rand() % 256) / 256.0f;

					vec3 NewGeneratedColor = vec3(red, green, blue);*/
					vec3 FaceCenter = MeshObject.GetVertex(i, 0) + MeshObject.GetVertex(i, 1) + MeshObject.GetVertex(i, 2);
					FaceCenter /= 3.0f;
					FaceCenter = vec4(MeshObject.WorldTransformation.RotateMatrix * MeshObject.MeshModelTransformation.RotateMatrix * vec4(FaceCenter, 1.0f));
					float Value = clamp(FaceCenter.z, MeshObject.MinPoints.z, MeshObject.MaxPoints.z);
					Value *= 3;

					vec3 NewGeneratedColor = Value * MeshObject.FacesRectangleColor;

					DrawFacesRectangles(FirstPoint, SecondPoint, ThirdPoint, NewGeneratedColor);
				}
				if (MeshObject.TriangleFill)
				{
					if (MeshObject.TriangleFillMode == WalkingEdge)
					{
					if (MeshObject.WalkingEdgeMode == Constant)
					{
						FillTringlesWalkingEdge(FirstPoint, SecondPoint, ThirdPoint, MeshObject.MeshModelFillColor);
					}
					if (MeshObject.WalkingEdgeMode == Random)
					{
						float red = (rand() % 256) / 256.0f;
						float green = (rand() % 256) / 256.0f;
						float blue = (rand() % 256) / 256.0f;

						vec3 NewGeneratedColor = vec3(red, green, blue);

						FillTringlesWalkingEdge(FirstPoint, SecondPoint, ThirdPoint, NewGeneratedColor);
					}
					if (MeshObject.WalkingEdgeMode == Constant_With_Depth)
					{
						vec3 centerface = (MeshObject.GetVertex(i, 0) + MeshObject.GetVertex(i, 1) + MeshObject.GetVertex(i, 2)) / 3.0f;
						centerface = vec3(CameraMatrix * WorldXLocalMatrix * vec4(centerface, 1.0f));
						float Value = clamp(centerface.z, MeshObject.MinPoints.z, MeshObject.MaxPoints.z);
						FillTringlesWalkingEdge(FirstPoint, SecondPoint, ThirdPoint, Value* MeshObject.MeshModelFillColor);

					}
					}
					if (MeshObject.TriangleFillMode == ScanLine)
					{
						//int width = Max_Coordinates.x - Min_Coordinates.x;
						//int height = Max_Coordinates.y - Min_Coordinates.y;
						vec3 MinOfThree;
						vec3 MaxOfThree;

						MinOfThree.x = std::min(FirstPoint.x, std::min(SecondPoint.x, ThirdPoint.x));
						MaxOfThree.x = std::max(FirstPoint.x, std::max(SecondPoint.x, ThirdPoint.x));

						MinOfThree.y = std::min(FirstPoint.y, std::min(SecondPoint.y, ThirdPoint.y));
						MaxOfThree.y = std::max(FirstPoint.y, std::max(SecondPoint.y, ThirdPoint.y));

						MinOfThree.z = std::min(FirstPoint.z, std::min(SecondPoint.z, ThirdPoint.z));
						MaxOfThree.z = std::max(FirstPoint.z, std::max(SecondPoint.z, ThirdPoint.z));

						if (MeshObject.ScanLineMode == Black_and_White)
						{
							for (int i = MinOfThree.x; i < MaxOfThree.x; i++)
							{
								for (int j = MinOfThree.y; j < MaxOfThree.y; j++)
								{
									if (PointInTriangle(vec3(i, j, 0), FirstPoint, SecondPoint, ThirdPoint) && CoordinatesInWindow(i, j))
									{
										float New_Depth = Z_ValueInterpolation(FirstPoint, SecondPoint, ThirdPoint, vec2(i, j));
										int Z_index = Z_INDEX(viewport_width, i, j);
										float Previous_Depth = Z_Buffer[Z_index];
										if (Z_Buffer[Z_index] == INFINITY)
										{
											Z_Buffer[Z_index] = New_Depth;
										}
										else
										{
											if (Previous_Depth > New_Depth)
											{
												Z_Buffer[Z_index] = Previous_Depth;

											}
											else
											{
												Z_Buffer[Z_index] = New_Depth;
											}
										}
									}

								}
							}

						}
						//FillTriangleScanLine(MeshObject, FirstPoint, SecondPoint, ThirdPoint);
						if (MeshObject.ScanLineMode == GrayScale)
						{
							//create Z-Buffer Procedure...
							for (int i = MinOfThree.x; i < MaxOfThree.x; i++)
							{
								for (int j = MinOfThree.y; j < MaxOfThree.y; j++)
								{
									if (PointInTriangle(vec3(i, j, 0), FirstPoint, SecondPoint, ThirdPoint) && CoordinatesInWindow(i, j))
									{
										float New_Depth = Z_ValueInterpolation(FirstPoint, SecondPoint, ThirdPoint, vec2(i, j));
										int Z_index = Z_INDEX(viewport_width, i, j);
										float Previous_Depth = Z_Buffer[Z_index];
										if (Z_Buffer[Z_index] == INFINITY)
										{
											Z_Buffer[Z_index] = New_Depth;
										}
										else
										{
											if (Previous_Depth > New_Depth)
											{
												Z_Buffer[Z_index] = Previous_Depth;

											}
											else
											{	
												Z_Buffer[Z_index] = New_Depth;
											}
										}
									}

								}
							}
						}
						if (MeshObject.ScanLineMode == ColorFul)
						{
							for (int i = MinOfThree.x; i < MaxOfThree.x; i++)
							{
								for (int j = MinOfThree.y; j < MaxOfThree.y; j++)
								{
									if (PointInTriangle(vec3(i, j, 0), FirstPoint, SecondPoint, ThirdPoint) && CoordinatesInWindow(i, j))
									{
										PutPixel(i, j, MeshObject.MeshModelFillColor);
									}
								}
							}
						}
					}

				}
			}



		}

		if (MeshObject.WorldBoundingBox)
		{
			vec3 WorldBoundingBoxColor = MeshObject.WorldBoundingBoxColor;

			DrawLine(Min_Coordinates, vec2(Min_Coordinates.x, Max_Coordinates.y), WorldBoundingBoxColor);
			DrawLine(Min_Coordinates, vec2(Max_Coordinates.x, Min_Coordinates.y), WorldBoundingBoxColor);

			DrawLine(Max_Coordinates, vec2(Min_Coordinates.x, Max_Coordinates.y), WorldBoundingBoxColor);
			DrawLine(Max_Coordinates, vec2(Max_Coordinates.x, Min_Coordinates.y), WorldBoundingBoxColor);
		}

		Maximum = Max_Coordinates;
		Minimum = Min_Coordinates;

	}
	

}

void Renderer::ApplyLightEffect(Scene scene)
{
	Camera& CurrentCamera = scene.GetActiveCamera();
	CurrentCamera.SetViewTransformation();
	CurrentCamera.SetMainMatrix();
	mat4x4 CameraMatrix = CurrentCamera.MainMatrix;


	for (int MeshIndex = 0; MeshIndex < scene.GetModelCount(); MeshIndex++)
	{
		MeshModel& MeshObject = scene.GetModel(MeshIndex);

		MeshObject.MeshModelTransformation.SetTransformation();
		MeshObject.WorldTransformation.SetTransformation();

		mat4x4 WorldXLocalMatrix = (MeshObject.WorldTransformation.GetMainMatrix()) * MeshObject.MeshModelTransformation.GetMainMatrix();
		MeshObject.UpdatedCenterPoint = vec3(CameraMatrix * WorldXLocalMatrix * vec4(MeshObject.CenterPoint, 1.0f));
		DrawMeshModelFeatures(MeshObject, CameraMatrix, WorldXLocalMatrix);

		vec3 Max_Coordinates = vec3(0.0f, 0.0f, MeshObject.MaxPoints.z);
		vec3 Min_Coordinates = vec3(this->GetViewportWidth(), this->GetViewportHeight(), MeshObject.MinPoints.z);
		/*
		//Lights
		//==========================================================
		//Flat Shading Sum!
		vec3 Flat_ShadingAmbientTotal = vec3(0.0f, 0.0f, 0.0f);
		vec3 Flat_ShadingDiffuseTotal = vec3(0.0f, 0.0f, 0.0f);
		vec3 Flat_ShadingSpecularTotal = vec3(0.0f, 0.0f, 0.0f);
		//==========================================================
		//Gouraud Shading Sum!
		vec3 Gouraud_ShadingAmbientTotal = vec3(0.0f, 0.0f, 0.0f);
		vec3 Gouraud_ShadingDiffuseTotal = vec3(0.0f, 0.0f, 0.0f);
		vec3 Gouraud_ShadingSpecularTotal = vec3(0.0f, 0.0f, 0.0f);
		//==========================================================
		//Phong Shading Sum!
		vec3 Phong_ShadingAmbientTotal = vec3(0.0f, 0.0f, 0.0f);
		vec3 Phong_ShadingDiffuseTotal = vec3(0.0f, 0.0f, 0.0f);
		vec3 Phong_ShadingSpecularTotal = vec3(0.0f, 0.0f, 0.0f);
		*/

		for (int i = 0; i < MeshObject.GetFacesCount(); i++)
		{
			vec3 FirstPoint = MeshObject.GetVertex(i, 0);
			vec3 SecondPoint = MeshObject.GetVertex(i, 1);
			vec3 ThirdPoint = MeshObject.GetVertex(i, 2);

			// Regular Calculations for 3 Vertices of Each Face
			{

				vec4 FirstTransformedPoint = CameraMatrix * WorldXLocalMatrix * vec4(FirstPoint, 1.0f);
				vec4 SecondTransformedPoint = CameraMatrix * WorldXLocalMatrix * vec4(SecondPoint, 1.0f);
				vec4 ThirdTransformedPoint = CameraMatrix * WorldXLocalMatrix * vec4(ThirdPoint, 1.0f);

				if (FirstTransformedPoint.w != 0 && SecondTransformedPoint.w != 0 && ThirdTransformedPoint.w != 0 && CurrentCamera.ProjectionStatus != Perspective)
				{
					FirstTransformedPoint /= FirstTransformedPoint.w;
					SecondTransformedPoint /= SecondTransformedPoint.w;
					ThirdTransformedPoint /= ThirdTransformedPoint.w;
				}

				FirstPoint = FirstTransformedPoint;
				SecondPoint = SecondTransformedPoint;
				ThirdPoint = ThirdTransformedPoint;
			}

			if (FaceCulling(FirstPoint, SecondPoint, ThirdPoint, CurrentCamera))
			{

				Max_Coordinates.x = std::max(std::max(std::max(FirstPoint.x, SecondPoint.x), ThirdPoint.x), Max_Coordinates.x);
				Min_Coordinates.x = std::min(std::min(std::min(FirstPoint.x, SecondPoint.x), ThirdPoint.x), Min_Coordinates.x);

				Max_Coordinates.y = std::max(std::max(std::max(FirstPoint.y, SecondPoint.y), ThirdPoint.y), Max_Coordinates.y);
				Min_Coordinates.y = std::min(std::min(std::min(FirstPoint.y, SecondPoint.y), ThirdPoint.y), Min_Coordinates.y);

				if (!MeshObject.OffGridDraw)
				{
					Renderer::DrawLine(vec2(FirstPoint.x, FirstPoint.y), vec2(SecondPoint.x, SecondPoint.y), MeshObject.MeshModelGridColor);
					Renderer::DrawLine(vec2(FirstPoint.x, FirstPoint.y), vec2(ThirdPoint.x, ThirdPoint.y), MeshObject.MeshModelGridColor);
					Renderer::DrawLine(vec2(ThirdPoint.x, ThirdPoint.y), vec2(SecondPoint.x, SecondPoint.y), MeshObject.MeshModelGridColor);
				}
				if (scene.Activiate_LightSource)
				{
					for (int L = 0; L < scene.GetLightCount(); L++)
					{
						Light& LightSource = scene.GetLight(L);
						LightSource.Local.SetTransformation();
						LightSource.World.SetTransformation();

						mat4x4 Result = CameraMatrix * LightSource.World.GetMainMatrix() * LightSource.Local.GetMainMatrix();
						vec3 LightPosition = vec3(Result * vec4(LightSource.Light_Position, 1.0f));
						if (scene.Mode == Flat_Shading)
						{
							//Caculating Ambient
							vec3 AmbientTotal = MeshObject.AmbientColor * LightSource.AmbientColor;
							//===============================================================================================================
							//Calculating Diffuse
							vec3 FaceNormal = triangleNormal(FirstPoint, SecondPoint, ThirdPoint);
							vec3 CenterFace = (FirstPoint + SecondPoint + ThirdPoint) / 3.0f;
							vec3 LightVector = LightPosition - CenterFace;

							FaceNormal = normalize(FaceNormal);
							LightVector = normalize(LightVector);

							float angle = std::max(glm::dot(FaceNormal, LightVector), 0.0f);

							vec3 DiffuseTotal = angle * LightSource.DiffuseColor * MeshObject.DiffuseColor;
							//===============================================================================================================
							//Calculating Specular
							vec3 CameraVector = glm::normalize(scene.GetActiveCamera().eye - CenterFace);
							vec3 reflect_Light = glm::normalize(glm::reflect(LightVector, FaceNormal));
							float specular_angle = pow(std::max(glm::dot(-1.0f * CameraVector, reflect_Light), 0.0f), LightSource.Specular_Degree);
							vec3 SpecularTotal = specular_angle * LightSource.SpecularColor * MeshObject.SpecularColor;
							//vec3 SpecularTotal = vec3(0.0f,0.0f,0.0f);
							//==============================================================================================================
							//Sum of All Ambient + Diffuse + Specular
							vec3 TotalColor = AmbientTotal + DiffuseTotal + SpecularTotal;
							TotalColor = clamp(TotalColor, vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
							FillTriangleScanLine(TotalColor, FirstPoint, SecondPoint, ThirdPoint);
							//===============================================================================================================
						}
						if (scene.Mode == Gouraud_Shading)
						{
							//Interpolation By Color and fill!
							vec3 Color_A, Color_B, Color_C;
							//Calculating Ambient
							vec3 AmbientTotal = MeshObject.AmbientColor * LightSource.AmbientColor;
							Color_A = AmbientTotal, Color_B = AmbientTotal, Color_C = AmbientTotal;
							
							vec3 Normal1 = MeshObject.GetNormal(i, 0);
							vec3 Normal2 = MeshObject.GetNormal(i, 1);
							vec3 Normal3 = MeshObject.GetNormal(i, 2);
							
							{

								vec4 TransformedNormal1 = CameraMatrix * WorldXLocalMatrix * vec4(Normal1, 1.0f);
								vec4 TransformedNormal2 = CameraMatrix * WorldXLocalMatrix * vec4(Normal2, 1.0f);
								vec4 TransformedNormal3 = CameraMatrix * WorldXLocalMatrix * vec4(Normal3, 1.0f);

								if (TransformedNormal1.w != 0 && TransformedNormal2.w != 0 && TransformedNormal3.w != 0 && CurrentCamera.ProjectionStatus != Perspective)
								{
									TransformedNormal1 /= TransformedNormal1.w;
									TransformedNormal2 /= TransformedNormal2.w;
									TransformedNormal3 /= TransformedNormal3.w;
								}

								Normal1 = normalize(TransformedNormal1);
								Normal2 = normalize(TransformedNormal2);
								Normal3 = normalize(TransformedNormal3);
							}
							//==============================================================================================================
							//Calculating Diffuse
							vec3 LightDir1 = normalize(LightPosition - FirstPoint);
							vec3 LightDir2 = normalize(LightPosition - SecondPoint);
							vec3 LightDir3 = normalize(LightPosition - ThirdPoint);

							float angle1 = std::max(glm::dot(Normal1, -1.0f * LightDir1), 0.0f);
							float angle2 = std::max(glm::dot(Normal2, -1.0f * LightDir2), 0.0f);
							float angle3 = std::max(glm::dot(Normal3, -1.0f * LightDir3), 0.0f);

							vec3 Diffuse1 = angle1 * LightSource.DiffuseColor * MeshObject.DiffuseColor;
							vec3 Diffuse2 = angle2 * LightSource.DiffuseColor * MeshObject.DiffuseColor;
							vec3 Diffuse3 = angle3 * LightSource.DiffuseColor * MeshObject.DiffuseColor;
							//Calculating Specular
							vec3 CameraDir1 = glm::normalize(scene.GetActiveCamera().eye - LightDir1);
							vec3 CameraDir2 = glm::normalize(scene.GetActiveCamera().eye - LightDir2);
							vec3 CameraDir3 = glm::normalize(scene.GetActiveCamera().eye - LightDir3);

							vec3 reflect_Light1 = glm::normalize(glm::reflect((float)-1 * LightDir1, Normal1));
							vec3 reflect_Light2 = glm::normalize(glm::reflect((float)-1 * LightDir1, Normal2));
							vec3 reflect_Light3 = glm::normalize(glm::reflect((float)-1 * LightDir1, Normal3));

							float specular_angle1 = pow(std::max(glm::dot(CameraDir1, reflect_Light1), 0.0f), LightSource.Specular_Degree);
							float specular_angle2 = pow(std::max(glm::dot(CameraDir2, reflect_Light2), 0.0f), LightSource.Specular_Degree);
							float specular_angle3 = pow(std::max(glm::dot(CameraDir3, reflect_Light3), 0.0f), LightSource.Specular_Degree);

							vec3 Specular1 = specular_angle1 * LightSource.SpecularColor * MeshObject.SpecularColor;
							vec3 Specular2 = specular_angle2 * LightSource.SpecularColor * MeshObject.SpecularColor;
							vec3 Specular3 = specular_angle3 * LightSource.SpecularColor * MeshObject.SpecularColor;

							//==============================================================================================================
							//Sum of All Ambient + Diffuse + Specular
							Color_A = AmbientTotal + Diffuse1 + Specular1;
							Color_B = AmbientTotal + Diffuse2 + Specular2;
							Color_C = AmbientTotal + Diffuse3 + Specular3;

							ApplyGouraudShade(FirstPoint, SecondPoint, ThirdPoint,Color_A,Color_B,Color_C);
							//===============================================================================================================

						}
						if (scene.Mode == Phong_Shading)
						{
							//Interpolation By Color and fill!

							vec3 Normal1 = MeshObject.GetNormal(i, 0);
							vec3 Normal2 = MeshObject.GetNormal(i, 1);
							vec3 Normal3 = MeshObject.GetNormal(i, 2);

							{

								vec4 TransformedNormal1 = CameraMatrix * WorldXLocalMatrix * vec4(Normal1, 1.0f);
								vec4 TransformedNormal2 = CameraMatrix * WorldXLocalMatrix * vec4(Normal2, 1.0f);
								vec4 TransformedNormal3 = CameraMatrix * WorldXLocalMatrix * vec4(Normal3, 1.0f);

								if (TransformedNormal1.w != 0 && TransformedNormal2.w != 0 && TransformedNormal3.w != 0 && CurrentCamera.ProjectionStatus != Perspective)
								{
									TransformedNormal1 /= TransformedNormal1.w;
									TransformedNormal2 /= TransformedNormal2.w;
									TransformedNormal3 /= TransformedNormal3.w;
								}

								Normal1 = normalize(TransformedNormal1);
								Normal2 = normalize(TransformedNormal2);
								Normal3 = normalize(TransformedNormal3);

							}

							vec2 Min = FirstPoint, Max = FirstPoint;
							Min.x = std::min(FirstPoint.x, std::min(SecondPoint.x, ThirdPoint.x));
							Max.x = std::max(FirstPoint.x, std::max(SecondPoint.x, ThirdPoint.x));

							Min.y = std::min(FirstPoint.y, std::min(SecondPoint.y, ThirdPoint.y));
							Max.y = std::max(FirstPoint.y, std::max(SecondPoint.y, ThirdPoint.y));


							for (int i = Min.x; i <= Max.x; i++)
							{
								for (int j = Min.y; j <= Max.y; j++)
								{
									if (PointInTriangle(vec3(i, j, 1.0f), FirstPoint, SecondPoint, ThirdPoint) && CoordinatesInWindow(i, j))
									{
										vec3 PixelNormal = NormalInterpolation(FirstPoint, SecondPoint, ThirdPoint, Normal1, Normal2, Normal3, vec2(i, j));
										//Caculating Ambient
										vec3 AmbientTotal = MeshObject.AmbientColor * LightSource.AmbientColor;
										//===============================================================================================================
										//Calculating Diffuse
										vec3 VerticePosition = vec3(i, j, 0.0f);
										vec3 LightVector = LightPosition - VerticePosition;

										PixelNormal = normalize(PixelNormal);
										LightVector = normalize(LightVector);

										float angle = std::max(glm::dot(PixelNormal, LightVector), 0.0f);

										vec3 DiffuseTotal = angle * LightSource.DiffuseColor * MeshObject.DiffuseColor;
										//===============================================================================================================
										//Calculating Specular
										vec3 CameraVector = normalize(scene.GetActiveCamera().eye - VerticePosition);
										vec3 reflect_Light = normalize(glm::reflect(LightVector, PixelNormal));
										float specular_angle = pow(std::max(glm::dot(CameraVector, reflect_Light), 0.0f), LightSource.Specular_Degree);
										vec3 SpecularTotal = specular_angle * LightSource.SpecularColor * MeshObject.SpecularColor;
										//==============================================================================================================
										//Sum of All Ambient + Diffuse + Specular
										vec3 TotalColor = AmbientTotal + DiffuseTotal + SpecularTotal;
										TotalColor = clamp(TotalColor, vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
										// FillTriangleScanLine(TotalColor, FirstPoint, SecondPoint, ThirdPoint);
										//===============================================================================================================
										PutPixel(i, j, TotalColor);
									}
								}
							}
						
						}
					}
				}
				if (scene.DrawRays)
				{
					for (int LightIndex = 0; LightIndex < scene.GetLightCount(); LightIndex++)
					{
						Light& LightSource = scene.GetLight(LightIndex);
						mat4x4 Result = CameraMatrix * LightSource.World.GetMainMatrix() * LightSource.Local.GetMainMatrix();
						vec3 LightPosition = vec3(Result * vec4(LightSource.Light_Position, 1.0f));
						vec3 CenterFace = FirstPoint + SecondPoint + ThirdPoint;
						CenterFace /= 3.0f;
						vec3 NormalFace = triangleNormal(FirstPoint, SecondPoint, ThirdPoint);
						vec3 CameraVector = glm::normalize(scene.GetActiveCamera().eye - CenterFace);
						vec3 LightDir = normalize(LightPosition - CenterFace);
						vec3 reflect_Light = glm::normalize(glm::reflect(LightDir, NormalFace));
						//From Center Face to Light Position
						DrawLine(CenterFace, CenterFace + scene.RayLength * LightDir, vec3(1.0f, 1.0f, 0.0f));
						DrawLine(CenterFace, CenterFace + scene.RayLength * reflect_Light, vec3(0.0f, 1.0f, 1.0f));
						DrawLine(LightPosition, LightPosition + -1.0f * scene.RayLength * LightDir, vec3(1.0f, 0.0f, 1.0f));

					}
				}


			}

		}

		if (MeshObject.WorldBoundingBox)
		{
			vec3 WorldBoundingBoxColor = MeshObject.WorldBoundingBoxColor;

			DrawLine(Min_Coordinates, vec2(Min_Coordinates.x, Max_Coordinates.y), WorldBoundingBoxColor);
			DrawLine(Min_Coordinates, vec2(Max_Coordinates.x, Min_Coordinates.y), WorldBoundingBoxColor);

			DrawLine(Max_Coordinates, vec2(Min_Coordinates.x, Max_Coordinates.y), WorldBoundingBoxColor);
			DrawLine(Max_Coordinates, vec2(Max_Coordinates.x, Min_Coordinates.y), WorldBoundingBoxColor);
		}

		Maximum = Max_Coordinates;
		Minimum = Min_Coordinates;

	}


}

void Renderer::DrawMeshModelFeatures(MeshModel& Object, mat4x4 Camera, mat4x4 Transformation)
{
	mat4x4 Result = Camera * Transformation;
	//UpdateCenterPoint(Result, Object);
	if (Object.BoundingBoxDraw)
	{
		DrawBoundingBox(Result, Object);
	}
	if (Object.NormalsDraw)
	{
		vec3 Point1, Point2, Point3;
		vec3 End1, End2, End3;

		vec3 Normal1, Normal2, Normal3;

		for (int i = 0; i < Object.GetFacesCount(); i++)
		{
			Point1 = Object.GetVertex(i, 0);
			Point2 = Object.GetVertex(i, 1);
			Point3 = Object.GetVertex(i, 2);

			Point1 = Result * glm::vec4(Point1, 1);
			Point2 = Result * glm::vec4(Point2, 1);
			Point3 = Result * glm::vec4(Point3, 1);

			Normal1 = Object.GetNormal(i, 0);
			Normal2 = Object.GetNormal(i, 1);
			Normal3 = Object.GetNormal(i, 2);



			End1 = Point1 + Object.NormalsLength * Normal1;
			End2 = Point2 + Object.NormalsLength * Normal2;
			End3 = Point3 + Object.NormalsLength * Normal3;

			Renderer::DrawLine(Point1, End1, Object.NormalsColor);
			Renderer::DrawLine(Point2, End2, Object.NormalsColor);
			Renderer::DrawLine(Point3, End3, Object.NormalsColor);

		}
	}
	if (Object.CenterOfFaceDraw)
	{
		vec3 Point1, Point2, Point3;
		vec3 CenterFace;
		vec3 NormalOfFace;

		for (int i = 0; i < Object.GetFacesCount(); i++)
		{
			Point1 = Object.GetVertex(i, 0);
			Point2 = Object.GetVertex(i, 1);
			Point3 = Object.GetVertex(i, 2);

			CenterFace = (Point1 + Point2 + Point3) / 3.0f;
			CenterFace = Result * glm::vec4(CenterFace, 1);
			NormalOfFace = triangleNormal(Point1, Point2, Point3);

			Renderer::DrawLine(CenterFace, CenterFace + Object.CenterOfFaceLength * NormalOfFace, Object.NormalsColor);
		}


	}
	if (Object.Local_Axis)
	{
		vec3 X_Local = Result * vec4(Object.MeshModelAxis.X_Local, 0.0f);
		vec3 Y_Local = Result * vec4(Object.MeshModelAxis.Y_Local, 0.0f);
		vec3 Z_Local = Result * vec4(Object.MeshModelAxis.Z_Local, 0.0f);

		X_Local = normalize(X_Local);
		Y_Local = normalize(Y_Local);
		Z_Local = normalize(Z_Local);

		DrawLine(Object.UpdatedCenterPoint, Object.UpdatedCenterPoint + Object.AxisLength * X_Local, vec3(1.0f, 1.0f, 0.0f));
		DrawLine(Object.UpdatedCenterPoint, Object.UpdatedCenterPoint + Object.AxisLength * Y_Local, vec3(0.0f, 1.0f, 1.0f));
		DrawLine(Object.UpdatedCenterPoint, Object.UpdatedCenterPoint + Object.AxisLength * Z_Local, vec3(1.0f, 0.0f, 1.0f));

	}
}

void Renderer::UpdateCenterPoint(mat4x4 input, MeshModel Object)
{
	Object.UpdatedCenterPoint = vec3(input * vec4(Object.CenterPoint, 1.0f));
}

void Renderer::DrawBoundingBox(mat4x4 input, MeshModel Object)
{
	vec3 point1, point2, point3, point4, point5, point6, point7, point8;
	//Obtaining Edge Points
	{
		point1 = Object.EdgePoints[0];
		point2 = Object.EdgePoints[1];
		point3 = Object.EdgePoints[2];
		point4 = Object.EdgePoints[3];
		point5 = Object.EdgePoints[4];
		point6 = Object.EdgePoints[5];
		point7 = Object.EdgePoints[6];
		point8 = Object.EdgePoints[7];
	}

	//Implemmenting Transformation
	{
		point1 = vec3(input * vec4(point1, 1));
		point2 = vec3(input * vec4(point2, 1));
		point3 = vec3(input * vec4(point3, 1));
		point4 = vec3(input * vec4(point4, 1));
		point5 = vec3(input * vec4(point5, 1));
		point6 = vec3(input * vec4(point6, 1));
		point7 = vec3(input * vec4(point7, 1));
		point8 = vec3(input * vec4(point8, 1));
	}

	//Draw Bounding Box using 8 points
	{
		DrawLine(glm::vec2(point2), glm::vec2(point4), Object.BoundingBoxColor);
		DrawLine(glm::vec2(point1), glm::vec2(point2), Object.BoundingBoxColor);
		DrawLine(glm::vec2(point4), glm::vec2(point3), Object.BoundingBoxColor);
		DrawLine(glm::vec2(point3), glm::vec2(point1), Object.BoundingBoxColor);

		DrawLine(glm::vec2(point4), glm::vec2(point8), Object.BoundingBoxColor);
		DrawLine(glm::vec2(point8), glm::vec2(point7), Object.BoundingBoxColor);
		DrawLine(glm::vec2(point7), glm::vec2(point3), Object.BoundingBoxColor);
		DrawLine(glm::vec2(point3), glm::vec2(point4), Object.BoundingBoxColor);

		DrawLine(glm::vec2(point3), glm::vec2(point7), Object.BoundingBoxColor);
		DrawLine(glm::vec2(point7), glm::vec2(point5), Object.BoundingBoxColor);
		DrawLine(glm::vec2(point5), glm::vec2(point1), Object.BoundingBoxColor);
		DrawLine(glm::vec2(point1), glm::vec2(point3), Object.BoundingBoxColor);

		DrawLine(glm::vec2(point6), glm::vec2(point2), Object.BoundingBoxColor);
		DrawLine(glm::vec2(point6), glm::vec2(point8), Object.BoundingBoxColor);
		DrawLine(glm::vec2(point6), glm::vec2(point5), Object.BoundingBoxColor);
	}

	

}

void Renderer::FillTriangleScanLine(vec3 Color, vec3 A, vec3 B, vec3 C)
{
	vec2 Min = A,Max = A;
	Min.x = std::min(A.x, std::min(B.x, C.x));
	Max.x = std::max(A.x, std::max(B.x, C.x));

	Min.y = std::min(A.y, std::min(B.y, C.y));
	Max.y = std::max(A.y, std::max(B.y, C.y));


	for (int i = Min.x; i <= Max.x; i++)
	{
		for (int j = Min.y; j <= Max.y; j++)
		{
			if (PointInTriangle(vec3(i, j, 1.0f),A,B,C))
			{
				PutPixel(i, j, Color);
			}
		}
	}
	

}

void Renderer::FillTringlesWalkingEdge(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 Color)
{
	glm::vec3 Top = glm::vec3(0), Middle = glm::vec3(0), Bottom = glm::vec3(0);

	//sorting points 
	if (point1.y > point2.y && point1.y > point3.y)
	{
		Top = point1;
		if (point2.y > point3.y)
		{
			Middle = point2;
			Bottom = point3;
		}
		else
		{
			Middle = point3;
			Bottom = point2;
		}
	}
	if (point2.y > point1.y && point2.y > point3.y)
	{
		Top = point2;
		if (point1.y > point3.y)
		{
			Middle = point1;
			Bottom = point3;
		}
		else
		{
			Middle = point3;
			Bottom = point1;
		}

	}
	if (point3.y > point1.y && point3.y > point2.y)
	{
		Top = point3;
		if (point1.y > point2.y)
		{
			Middle = point1;
			Bottom = point2;
		}
		else
		{
			Middle = point2;
			Bottom = point1;
		}

	}

	if (!(Top == glm::vec3(0) && Middle == glm::vec3(0) && Bottom == glm::vec3(0)))
	{
		int MiddleIsLeft = 0;
		int MiddleIsRight = 0;

		bool Infinite_detectorTM = false;
		bool Infinite_detectorTB = false;
		bool Infinite_detectorMB = false;

		float delta_x_TM = Top.x - Middle.x;
		float delta_x_TB = Top.x - Bottom.x;
		float delta_x_MB = Middle.x - Bottom.x;

		if (delta_x_TM == 0)
		{
			Infinite_detectorTM = true;
		}
		if (delta_x_TB == 0)
		{
			Infinite_detectorTB = true;
		}
		if (delta_x_MB == 0)
		{
			Infinite_detectorMB = true;
		}

		float A_TM = (Top.y - Middle.y) / delta_x_TM, A_TB = (Top.y - Bottom.y) / delta_x_TB, A_MB = (Middle.y - Bottom.y) / delta_x_MB;

		float Current_Y = Top.y - 1;
		float Right, Left;

		if (Top.y == Middle.y && !(Infinite_detectorTM || Infinite_detectorTB || Infinite_detectorMB))
		{
			while (Current_Y >= Bottom.y)
			{
				Right = (Current_Y - Top.y) / A_TB + Top.x;
				//Calculating the x Line From Top To Middle
				Left = (Current_Y - Middle.y) / A_MB + Middle.x;
			}
			DrawLine(glm::vec2(Right, Current_Y), glm::vec2(Left, Current_Y), Color);
			Current_Y -= 1;
		}

		//first stage
		while (Current_Y >= Middle.y)
		{
			if (Top.y == Middle.y)
			{
				if (!(Infinite_detectorTB || Infinite_detectorMB))
				{
					if (Top.x > Middle.x)
					{
						MiddleIsRight = 0;
						MiddleIsLeft = 1;

						Right = (Current_Y - Bottom.y) / A_TB + Bottom.x;
						Left = (Current_Y - Bottom.y) / A_MB + Bottom.x;
					}
					else
					{
						MiddleIsRight = 1;
						MiddleIsLeft = 0;

						Left = (Current_Y - Bottom.y) / A_TB + Bottom.x;
						Right = (Current_Y - Bottom.y) / A_MB + Bottom.x;
					}
				}
				if (Infinite_detectorTB || Infinite_detectorMB)
				{
					if (Top.x > Middle.x)
					{
						MiddleIsLeft = 0;
						MiddleIsRight = 1;
						Right = (Current_Y - Top.y) / A_TB + Top.x;
						Left = Middle.x;
					}
					else
					{
						MiddleIsLeft = 1;
						MiddleIsRight = 0;
						Right = Bottom.x;
						Left = (Current_Y - Middle.y) / A_MB + Middle.x;
					}
				}

			}

			if (Infinite_detectorTM || Infinite_detectorTB || Infinite_detectorMB)
			{
				if (Infinite_detectorTM)
				{
					if (Top.x < Bottom.x)
					{
						MiddleIsLeft = 0;
						MiddleIsRight = 1;
						Right = (Current_Y - Top.y) / A_TB + Top.x;
						Left = Middle.x;

					}
					else
					{
						MiddleIsLeft = 1;
						MiddleIsRight = 0;
						Right = Middle.x;
						Left = (Current_Y - Top.y) / A_TB + Top.x;

					}
				}
				if (Infinite_detectorTB)
				{
					if (Middle.x > Top.x)
					{
						MiddleIsLeft = 0;
						MiddleIsRight = 1;
						Right = Top.x;
						Left = (Current_Y - Top.y) / A_TM + Top.x;

					}
					else
					{
						MiddleIsLeft = 1;
						MiddleIsRight = 0;
						Right = (Current_Y - Top.y) / A_TM + Top.x;
						Left = Top.x;

					}

				}
				if (Infinite_detectorMB)
				{
					if (Top.x > Middle.x)
					{
						MiddleIsLeft = 0;
						MiddleIsRight = 1;
						Right = (Current_Y - Top.y) / A_TB + Top.x;
						Left = (Current_Y - Top.y) / A_TM + Top.x;;
					}
					else
					{
						MiddleIsLeft = 1;
						MiddleIsRight = 0;

						Right = (Current_Y - Top.y) / A_TB + Top.x;
						Left = (Current_Y - Top.y) / A_TM + Top.x;;
					}
					DrawLine(glm::vec2(Right, Current_Y), glm::vec2(Left, Current_Y), Color);
					Current_Y -= 1;
				}
				DrawLine(glm::vec2(Right, Current_Y), glm::vec2(Left, Current_Y), Color);
				Current_Y -= 1;
			}
			else
			{
				//Calculating the x Line From Top To Bottom
				Right = (Current_Y - Top.y) / A_TB + Top.x;
				//Calculating the x Line From Top To Middle
				Left = (Current_Y - Top.y) / A_TM + Top.x;

				DrawLine(glm::vec2(Right, Current_Y), glm::vec2(Left, Current_Y), Color);
				Current_Y -= 1;
			}
		}

		//second stage
		while (Current_Y >= Bottom.y)
		{
			if (Top.y == Middle.y)
			{
				if (!(Infinite_detectorTB || Infinite_detectorMB))
				{
					if (Top.x > Middle.x)
					{
						MiddleIsRight = 0;
						MiddleIsLeft = 1;

						Right = (Current_Y - Bottom.y) / A_TB + Bottom.x;
						Left = (Current_Y - Bottom.y) / A_MB + Bottom.x;
					}
					else
					{
						MiddleIsRight = 1;
						MiddleIsLeft = 0;

						Left = (Current_Y - Bottom.y) / A_TB + Bottom.x;
						Right = (Current_Y - Bottom.y) / A_MB + Bottom.x;
					}
				}
				if (Infinite_detectorTB || Infinite_detectorMB)
				{
					if (Top.x > Middle.x)
					{
						MiddleIsLeft = 0;
						MiddleIsRight = 1;
						Right = (Current_Y - Top.y) / A_TB + Top.x;
						Left = Middle.x;
					}
					else
					{
						MiddleIsLeft = 1;
						MiddleIsRight = 0;
						Right = Bottom.x;
						Left = (Current_Y - Middle.y) / A_MB + Middle.x;
					}
				}

				/*if (Infinite_detectorMB)
				{
					if (Bottom.x < Middle.x)
					{
						MiddleIsLeft = 0;
						MiddleIsRight = 1;
						Right = (Current_Y - Top.y) / A_TB + Top.x;
						Left = Middle.x;
					}
					else
					{
						MiddleIsLeft = 1;
						MiddleIsRight = 0;
						Right = Bottom.x;
						Left = (Current_Y - Middle.y) / A_MB + Middle.x;
					}
				}*/


			}
			if (Middle.y == Bottom.y)
			{
				if (Infinite_detectorTM)
				{
					if (Middle.x < Bottom.x)
					{
						MiddleIsRight = 0;
						MiddleIsLeft = 1;

						Left = Bottom.x;
						Right = (Current_Y - Bottom.y) / A_TB + Bottom.x;

					}
					else
					{
						MiddleIsRight = 1;
						MiddleIsLeft = 0;


						Right = Bottom.x;
						Left = (Current_Y - Bottom.y) / A_TM + Bottom.x;

					}
				}
				if (Infinite_detectorTB)
				{
					if (Middle.x < Bottom.x)
					{
						MiddleIsLeft = 1;
						MiddleIsRight = 0;

						Right = Bottom.x;
						Left = (Current_Y - Middle.y) / A_TM + Middle.x;
					}
					else
					{
						MiddleIsLeft = 0;
						MiddleIsRight = 1;

						Left = Bottom.x;
						Right = (Current_Y - Bottom.y) / A_TB + Bottom.x;

					}

				}
				else
				{
					Right = (Current_Y - Bottom.y) / A_TM + Bottom.x;
					Left = (Current_Y - Middle.y) / A_TB + Middle.x;
				}

			}
			else
			{
				//Continue Calculating the x Line From Top To Bottom
				Right = (Current_Y - Top.y) / A_TB + Top.x;
				//Calculating the x Line From Top To Middle
				Left = (Current_Y - Middle.y) / A_MB + Middle.x;
			}
			DrawLine(glm::vec2(Right, Current_Y), glm::vec2(Left, Current_Y), Color);
			Current_Y -= 1;

		}
	}
	else
	{
		if (point1.y == std::max(point3.y, point2.y))
		{
			Top = point1;
			Middle = (point2.y > point3.y) ? point2 : point3;
			Bottom = (point2.y < point3.y) ? point2 : point3;
		}
		if (point2.y == std::max(point1.y, point3.y))
		{
			Top = point2;
			Middle = (point1.y > point3.y) ? point1 : point3;
			Bottom = (point3.y < point1.y) ? point3 : point1;
		}
		if (point3.y == std::max(point1.y, point1.y))
		{
			Top = point3;
			Middle = (point1.y > point2.y) ? point1 : point2;
			Bottom = (point2.y < point1.y) ? point2 : point1;
		}

		int MiddleIsLeft = 0;
		int MiddleIsRight = 0;

		bool Infinite_detectorTM = false;
		bool Infinite_detectorTB = false;
		bool Infinite_detectorMB = false;

		float delta_x_TM = Top.x - Middle.x;
		float delta_x_TB = Top.x - Bottom.x;
		float delta_x_MB = Middle.x - Bottom.x;

		if (delta_x_TM == 0)
		{
			Infinite_detectorTM = true;
		}
		if (delta_x_TB == 0)
		{
			Infinite_detectorTB = true;
		}
		if (delta_x_MB == 0)
		{
			Infinite_detectorMB = true;
		}

		float A_TM = (Top.y - Middle.y) / delta_x_TM, A_TB = (Top.y - Bottom.y) / delta_x_TB, A_MB = (Middle.y - Bottom.y) / delta_x_MB;

		float Current_Y = Top.y - 1;
		float Right, Left;

		while (Current_Y >= Bottom.y)
		{
			Left = (Current_Y - Bottom.y) / A_MB + Bottom.x;
			Right = (Current_Y - Bottom.y) / A_TB + Bottom.x;
			DrawLine(glm::vec2(Right, Current_Y), glm::vec2(Left, Current_Y), Color);
			Current_Y -= 1;
		}

	}

}

void Renderer::DrawFacesRectangles(vec3 A, vec3 B, vec3 C, vec3 GeneratedColor)
{
	float MinX = std::min(A.x, std::min(B.x, C.x));
	float MaxX = std::max(A.x, std::max(B.x, C.x));

	float MinY = std::min(A.y, std::min(B.y, C.y));
	float MaxY = std::max(A.y, std::max(B.y, C.y));


	{
		DrawLine(vec2(MinX, MinY), vec2(MinX, MaxY), GeneratedColor);
		DrawLine(vec2(MinX, MinY), vec2(MaxX, MinY), GeneratedColor);

		DrawLine(vec2(MaxX, MaxY), vec2(MinX, MaxY), GeneratedColor);
		DrawLine(vec2(MaxX, MaxY), vec2(MaxX, MinY), GeneratedColor);
	}

}

void Renderer::DrawLight(Light input)
{
	input.Local.SetTransformation();
	input.World.SetTransformation();

	mat4x4 Result = input.World.GetMainMatrix() * input.Local.GetMainMatrix();
	vec3 TransformedPosition = vec3(Result * vec4(input.Light_Position, 1.0f));

	/*for (float x = input.Light_Position.x - input.Light_Radius; x < input.Light_Position.x + input.Light_Radius; x++)
	{
		for (float y = input.Light_Position.y - input.Light_Radius; y < input.Light_Position.y + input.Light_Radius; y++)
		{
			float distance = pow((x - input.Light_Position.x),2) + pow((y - input.Light_Position.y), 2);
			if (distance <= input.Light_Radius * input.Light_Radius)
			{
				PutPixel(x,y, input.Light_Color);
			}
		}
	}*/
	for (float x = TransformedPosition.x - input.Light_Radius; x < TransformedPosition.x + input.Light_Radius; x++)
	{
		for (float y = TransformedPosition.y - input.Light_Radius; y < TransformedPosition.y + input.Light_Radius; y++)
		{
			float distance = pow((x - TransformedPosition.x), 2) + pow((y - TransformedPosition.y), 2);
			if (distance <= input.Light_Radius * input.Light_Radius)
			{
				PutPixel(x, y, input.Light_Color);
			}
		}
	}
}

void Renderer::ApplyGouraudShade(vec3 A, vec3 B, vec3 C, vec3 Color_A, vec3 Color_B, vec3 Color_C)
{
	vec2 Min = A, Max = A;
	Min.x = std::min(A.x, std::min(B.x, C.x));
	Max.x = std::max(A.x, std::max(B.x, C.x));

	Min.y = std::min(A.y, std::min(B.y, C.y));
	Max.y = std::max(A.y, std::max(B.y, C.y));


	for (int i = Min.x; i <= Max.x; i++)
	{
		for (int j = Min.y; j <= Max.y; j++)
		{
			if (PointInTriangle(vec3(i, j, 1.0f), A, B, C) && CoordinatesInWindow(i, j))
			{
				vec3 PixelColor = ColorInterpolation(A, B, C, Color_A, Color_B, Color_C, vec2(i,j));
				PutPixel(i, j, PixelColor);
			}
		}
	}
}

void Renderer::ApplyPhongShade(vec3 A, vec3 B, vec3 C, vec3 Normal_A, vec3 Normal_B, vec3 Normal_C)
{
	vec2 Min = A, Max = A;
	Min.x = std::min(A.x, std::min(B.x, C.x));
	Max.x = std::max(A.x, std::max(B.x, C.x));

	Min.y = std::min(A.y, std::min(B.y, C.y));
	Max.y = std::max(A.y, std::max(B.y, C.y));


	for (int i = Min.x; i <= Max.x; i++)
	{
		for (int j = Min.y; j <= Max.y; j++)
		{
			if (PointInTriangle(vec3(i, j, 1.0f), A, B, C) && CoordinatesInWindow(i, j))
			{
				//vec3 PixelNormal = NormalInterpolation(A, B, C, Normal_A, Normal_B, Normal_C, vec2(i, j));
				////Caculating Ambient
				//vec3 AmbientTotal = MeshObject.AmbientColor * LightSource.AmbientColor;
				////===============================================================================================================
				////Calculating Diffuse
				//vec3 CenterFace = (FirstPoint + SecondPoint + ThirdPoint) / 3.0f;
				//vec3 LightVector = LightSource.Light_Position - CenterFace;

				//FaceNormal = normalize(FaceNormal);
				//LightVector = normalize(LightVector);

				//float angle = std::max(glm::dot(FaceNormal, LightVector), 0.0f);

				//vec3 DiffuseTotal = angle * LightSource.DiffuseColor * MeshObject.DiffuseColor;
				////===============================================================================================================
				////Calculating Specular
				//vec3 CameraVector = glm::normalize(scene.GetActiveCamera().eye - CenterFace);
				//vec3 reflect_Light = glm::normalize(glm::reflect(LightVector, FaceNormal));
				//float specular_angle = pow(std::max(glm::dot(-1.0f * CameraVector, reflect_Light), 0.0f), LightSource.Specular_Degree);
				//vec3 SpecularTotal = specular_angle * LightSource.SpecularColor * MeshObject.SpecularColor;
				////vec3 SpecularTotal = vec3(0.0f,0.0f,0.0f);
				////==============================================================================================================
				////Sum of All Ambient + Diffuse + Specular
				//vec3 TotalColor = AmbientTotal + DiffuseTotal + SpecularTotal;
				//TotalColor = clamp(TotalColor, vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
				//FillTriangleScanLine(TotalColor, FirstPoint, SecondPoint, ThirdPoint);
				////===============================================================================================================
				//PutPixel(i, j, PixelColor);
			}
		}
	}
}

vec3 Renderer::ColorInterpolation(vec3 A, vec3 B, vec3 C, vec3 Color_A, vec3 Color_B, vec3 Color_C , vec2 Point)
{
	float ABC = triangleArea(A, B, C);

	float DBC = triangleArea(vec3(Point, 1.0f), B, C);   //A1
	float DCA = triangleArea(vec3(Point, 1.0f), C, A);	//A2
	float DBA = triangleArea(vec3(Point, 1.0f), B, A);	//A3

	DBC /= ABC;
	DCA /= ABC;
	DBA /= ABC;

	return (DBC * Color_A + DCA * Color_B + DBA * Color_C);
}

vec3 Renderer::NormalInterpolation(vec3 A, vec3 B, vec3 C, vec3 Normal_A, vec3 Normal_B, vec3 Normal_C , vec2 Point)
{
	float ABC = triangleArea(A, B, C);

	float DBC = triangleArea(vec3(Point, 1.0f), B, C);   //A1
	float DCA = triangleArea(vec3(Point, 1.0f), C, A);	//A2
	float DBA = triangleArea(vec3(Point, 1.0f), B, A);	//A3

	DBC /= ABC;
	DCA /= ABC;
	DBA /= ABC;

	return (DBC * Normal_A + DCA * Normal_B + DBA * Normal_C);

}

// Computes the area of the triangle formed by A, B, and C.
double triangleArea(vec3 A, vec3 B, vec3 C) {
	return std::abs((A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)) / 2);
}
/*
// Returns true if and only if point P is inside the triangle formed by A, B, and C.
bool pointInTriangle(vec3 A, vec3 B, vec3 C, vec3 P) {
	// Compute the areas of the three sub-triangles formed by P and the sides of the triangle.
	double areaTotal = triangleArea(A, B, C);
	double area1 = triangleArea(A, B, P);
	double area2 = triangleArea(A, P, C);
	double area3 = triangleArea(P, B, C);

	// If the sum of the areas of the sub-triangles is equal to the area of the original triangle,
	// then P is inside the triangle.
	return (std::abs(areaTotal - (area1 + area2 + area3)) < 0);
}
*/
bool PointInTriangle(vec3 P, vec3 A, vec3 B, vec3 C)
{
	float r = (A.x - C.x) * (P.y - C.y) - (A.y - C.y) * (P.x - C.x);
	float t = (B.x - A.x) * (P.y - A.y) - (B.y - A.y) * (P.x - A.x);

	if ((r < 0) != (t < 0) && r != 0 && t != 0)
		return false;

	float d = (C.x - B.x) * (P.y - B.y) - (C.y - B.y) * (P.x - B.x);
	return (d == 0 || (d < 0) == (r + t <= 0));
}

bool FaceCulling(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,Camera CurrentCamera)
{
	// Calculate the face normal by taking the cross product of two edges of the face
	glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));

	// Calculate the dot product of the face normal and the view direction
	float dotProduct = glm::dot(normal, CurrentCamera.at - CurrentCamera.eye);

	// If the dot product is positive, the face is facing the viewer and should be drawn
	return (dotProduct > 0.0f);
}

int GetIndex(int Width, int Height, int i, int j)
{
	return j + i * Width;
}

float Z_ValueInterpolation(vec3 A, vec3 B, vec3 C, vec2 D)
{
	float ABC = triangleArea(A, B, C);

	float DBC = triangleArea(vec3(D,1.0f), B, C);   //A1
	float DCA = triangleArea(vec3(D, 1.0f), C, A);	//A2
	float DBA = triangleArea(vec3(D, 1.0f), B, A);	//A3

	DBC /= ABC;
	DCA /= ABC;
	DBA /= ABC;

	return DBC * A.z + DCA * B.z + DBA * C.z;

}

bool Renderer::CoordinatesInWindow(int x,int y)
{
	if (x >= 0 && y >= 0 && x <= viewport_width && y <= viewport_height)
		return true;
	return false;
}



