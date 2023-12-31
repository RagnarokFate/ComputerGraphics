#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "Light.h"
#include "Renderer.h"
#include "InitShader.h"
#include "glm//gtx/normal.hpp"
#include <random>
#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(width,x,y) ((x)+(y)*(width))
#define power2(x) (x)*(x)

#define scale 200
#define LineDepth 50

bool CalculateOnce = true;
float ModifyIntensity(float input);

float Distance(float x, float y, float z, float x2, float y2, float z2);
glm::vec3 ColorVector(glm::vec3 input);
float DistanceOfLine(glm::vec3 point1, glm::vec3 point2, glm::vec3 input);

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
	delete[] z_buffer;
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
	if (p1.x > p2.x)
	{
		DrawLine(p2, p1, color);
	}

	float x1, x2, y1, y2, e, a, offset;
	// initalizer of parameters
	x1 = p1.x, x2 = p2.  x, y1 = p1.y, y2 = p2.y;

	a = ((y2 - y1) / (x2 - x1));

	float dx = x2 - x1, dy = y2 - y1;

	offset = y1 - a * x1;

	//if dx = 0 , then a should be vertical. otherwords, when the point is up or below the Zero point directly vertical.
	if (x2 - x1 == 0)
	{
		if (y2 - y1 > 0)
		{
			while (y1 <= y2)
			{
				PutPixel(x1, y1, color);
				y1++;
			}
		}
		else
		{
			if (y2 - y1 < 0)
			{
				while (y2 <= y1)
				{
					PutPixel(x2, y2, color);
					y2++;
				}
			}
			else
			{
				PutPixel(x1, y1, color);
			}
		}
	}

	//for every a that in range of 0 < a <= 1
	if (a > 0 && a <= 1)
	{
		x1 = p1.x, x2 = p2.x, y1 = p1.y, y2 = p2.y;
		e = -1 * dx;
		//problem when x1 reachs x2 value : V
		while (x1 <= x2)
		{
			e = 2 * dy * x1 + 2 * dx * offset - 2 * dx * y1 - dx;
			if (e > 0)
			{
				y1 = y1 + 1;
			}
			PutPixel(x1, y1, color);
			x1 = x1 + 1;
		}
	}

	//for every a that in range of -1 <= a < 0
	if (a <= 0 && a > -1)
	{
		x1 = p1.x, x2 = p2.x, y1 = p1.y, y2 = p2.y;
		e = -1 * dx;
		while (x1 <= x2)
		{
			e = 2 * a * x1 + 2 * offset - 2 * y1 - 1;
			if (e <= 0)
			{
				y1 = y1 - 1;
			}

			PutPixel(x1, y1, color);
			x1 = x1 + 1;
		}
	}

	//for every a that in range of a>= 1
	if (a >= 1)
	{
		x1 = p1.y, x2 = p2.y, y1 = p1.x, y2 = p2.x;
		dx = x2 - x1;
		dy = y2 - y1;
		/*if (dx < 0)
		{
			x1 = p2.y, x2 = p1.y, y1 = p2.x, y2 = p1.x;
		}
		dx = x2 - x1;
		dy = y2 - y1;*/
		e = -1 * dx;
		//problem when x1 reachs x2 value : 
		while (x1 <= x2)
		{
			//	e = 2 * dy * x1 + 2 * dx * offset - 2 * dx * y1 - dx;
			if (e > 0)
			{
				y1 = y1 + 1;
				e -= dx;
			}
			PutPixel(y1, x1, color);
			x1 = x1 + 1;
			e += dy;
		}
	}

	//for every a that in range of a <= -1
	if (a <= -1)
	{
		x1 = p1.y, x2 = p2.y, y1 = p1.x, y2 = p2.x;
		dx = x2 - x1;
		dy = y2 - y1;
		if (dx < 0)
		{
			x1 = p2.y, x2 = p1.y, y1 = p2.x, y2 = p1.x;
		}
		dx = x2 - x1;
		dy = y2 - y1;
		e = -1 * dx;
		//problem when x1 reachs x2 value : 
		while (x1 <= x2)
		{
			//	e = 2 * dy * x1 + 2 * dx * offset - 2 * dx * y1 - dx;
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
void Renderer::CreateBuffers(int w, int h)
{
	CreateOpenglBuffer(); //Do not remove this line.
	color_buffer = new float[3 * w * h];
	z_buffer = new float[1920 * 1080];

	PreviousResult = new float[3*1920 * 1080];
	NextResult = new float[3*1920 * 1080];

	for (int i = 0; i < w * h; i++)
	{
		z_buffer[i] = INFINITY;
	}
	for (int i = 0; i < 3* w * h; i++)
	{
		PreviousResult[i] = 0;
	}
	for (int i = 0; i < 3 *w * h; i++)
	{
		NextResult[i] = 0;
	}
	for (int i = 0; i < w * h; i++)
	{
		z_buffer[i] = INFINITY;
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
	const GLfloat vtc[] = {
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[] = {
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1 };

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc) + sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)sizeof(vtc));

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"), 0);
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
void DrawCircle()
{
	
}
void Renderer::Render(const Scene& scene)
{

	Scene sc = scene;
	sc.GetCamera(scene.camerainput).aspect = viewport_width / viewport_height;
	//Draw Light Objects
	for (int j = 0; j < scene.GetLightsCount(); j++)
	{
		DrawLight(sc.GetLight(j), sc.GetCamera(scene.camerainput));
	}
	//Drawing Mesh Model Based on Camera View with no light
	//if (sc.GetLightsCount())
	//{
	//	for (int i = 0; i < scene.GetModelCount(); i++)
	//	{
	//		for (int j = 0; j < sc.GetLightsCount(); j++)
	//		{
	//			DrawMesh(scene.GetModel(i), scene.GetModel(i).Mesh_Model_Color, sc.GetCamera(scene.camerainput), sc.GetLight(j));
	//			DrawForMeshModelAxis(scene.GetModel(i));
	//			DrawForWorldAxis(scene.GetModel(i));
	//		}
	//	}
	//}
	if (sc.GetLightsCount())
{
	for (int i = 0; i < scene.GetModelCount(); i++)
	{
		DrawMesh(scene.GetModel(i), scene.GetModel(i).Mesh_Model_Color, sc.GetCamera(scene.camerainput), sc);
		DrawForMeshModelAxis(scene.GetModel(i));
		DrawForWorldAxis(scene.GetModel(i));
	}
}
	else
	{
		for (int i = 0; i < scene.GetModelCount(); i++)
		{
			DrawMesh(scene.GetModel(i), scene.GetModel(i).Mesh_Model_Color, sc.GetCamera(scene.camerainput));
			DrawForMeshModelAxis(scene.GetModel(i));
			DrawForWorldAxis(scene.GetModel(i));
		}
	}
	
	//Z-Buffer FillUP
	for (int i = 0; i < scene.GetModelCount(); i++)
	{
		if (scene.GetModel(i).FillBlackGray)
		{
			ExtractMinMax();
			z_min, z_max;
			for (int w = 0; w <= viewport_width; w++)
			{
				for (int h = 0; h <= viewport_height; h++)
				{
					//int x = z_buffer[Z_INDEX(viewport_width, w, h)];
					if (z_buffer[Z_INDEX(viewport_width, w, h)] != INFINITY)
					{
						float alpha = 1 - (z_max - z_buffer[Z_INDEX(viewport_width, w, h)]) / (z_max - z_min);
						PutPixel(w, h, glm::vec3(alpha, alpha, alpha));
						z_buffer[Z_INDEX(viewport_width, w, h)] = INFINITY;
					}
					else
					{
						PutPixel(w, h, glm::vec3(0.0f, 0.0f, 0.0f));
					}
				}
			}
		}
	}
	SwapBuffers();
	//Blur Effect
	if (sc.BlurScene)
	{
		ActiveBlurEffect(color_buffer, NextResult, viewport_width, viewport_height, sc);
		color_buffer = NextResult;
	}
	else
	{
		color_buffer = PreviousResult;
	}
	/*
	//Fog Effect
	if (sc.FogScene)
	{
		ActiveFogEffect(color_buffer, NextResult, viewport_width, viewport_height, sc);
		color_buffer = NextResult;
	}
	else
	{
		color_buffer = PreviousResult;
	}
	*/
}
int  Renderer::GetViewportWidth() const
{
	return viewport_width;
}
int  Renderer::GetViewportHeight() const
{
	return viewport_height;
}
//Drawing Mesh Model no light source
void Renderer::DrawMesh(MeshModel& Obj, glm::vec3& color, Camera& Cam)
{
	if (Obj.GetStatus())
	{
		glm::ivec3 tringale_color = glm::ivec3(0, 0, 0);
		glm::mat4x4 Result;
		//unnesscary initalizer of 0.0f,0.0f,0.0f for every Vector3D
		glm::vec3 start_point = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 middle_point = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 end_point = glm::vec3(0.0f, 0.0f, 0.0f);
		if (Cam.Orthograhpic_View)
		{
			Result = Cam.OrthographicMatrix * Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
			Cam.Perspective_View = false;
			Cam.FrustumView = false;

		}
		else
		{
			if (Cam.Perspective_View)
			{
				Result = Cam.PerspectiveMatrix * Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
				Cam.Orthograhpic_View = false;
				Cam.FrustumView = false;

			}
			else //if(Cam.frustumView)
			{
				Result = Cam.FrustumMatrix * Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
				Cam.Orthograhpic_View = false;
				Cam.Perspective_View = false;
			}

		}
		for (int i = 0; i < Obj.GetFacesCount(); i++)
		{
			start_point = Obj.GetVertex(i, 0);
			end_point = Obj.GetVertex(i, 1);
			middle_point = Obj.GetVertex(i, 2);

			if (FaceCuling(Result, start_point, middle_point, end_point, Cam))
			{
				start_point = Result * glm::vec4(start_point, 1);
				end_point = Result * glm::vec4(end_point, 1);
				middle_point = Result * glm::vec4(middle_point, 1);

				start_point.x *= scale; start_point.y *= scale;
				end_point.x *= scale; end_point.y *= scale;
				middle_point.x *= scale; middle_point.y *= scale;

				start_point.x += viewport_width / 2;  start_point.y += viewport_height / 2;
				end_point.x += viewport_width / 2;  end_point.y += viewport_height / 2;
				middle_point.x += viewport_width / 2; middle_point.y += viewport_height / 2;

				if (!(Obj.OffGrid))
				{
					Renderer::DrawLine(glm::vec2(start_point.x, start_point.y), glm::vec2(end_point.x, end_point.y), Obj.Mesh_Model_Color);
					Renderer::DrawLine(glm::vec2(start_point.x, start_point.y), glm::vec2(middle_point.x, middle_point.y), Obj.Mesh_Model_Color);
					Renderer::DrawLine(glm::vec2(end_point.x, end_point.y), glm::vec2(middle_point.x, middle_point.y), Obj.Mesh_Model_Color);
				}
				if (Obj.FillColor)
				{
					FillTringles(start_point, middle_point, end_point, Obj.Mesh_Model_Color);
				}

				DrawMeshFeatures(Obj, start_point, middle_point, end_point, Obj.Mesh_Model_Color, Cam);
			}
		}
		{
			Obj.ExtractZeroPoint();
			glm::vec3 Center_Point = Obj.Get_ZeroPoint();
			//Center_Point = Obj.Get_CurrentMeshModelMatrix() * glm::vec4(Center_Point, 1);
			Center_Point = Obj.Get_CurrentMeshModelMatrix() * glm::vec4(Center_Point, 1);
			Center_Point.x *= scale; Center_Point.y *= scale;
			Center_Point.x += viewport_width / 2; Center_Point.y += viewport_height / 2;

			Obj.Set_ZeroPoint(Center_Point);
			Obj.Adjust_Axis();
			if (Obj.RectangleStatus)
			{
				DrawBox(Obj, Result, Obj.Rectangle_X1, Obj.Rectangle_X2, Obj.Rectangle_Y1, Obj.Rectangle_Y2, Obj.Rectangle_Z1, Obj.Rectangle_Z2, color);
			}
			if (Obj.NormalStatus)
			{
				DrawNormals(Obj, Result, 50, Obj.Normals_Mesh_Model_Color);
			}
			if (Obj.NormalStatusPerFaces)
			{
				DrawNormalsPerFaces(Obj, Result, 50, Obj.Normals_Mesh_Model_Color);
			}
			if (Obj.VerticesColorful)
			{
				DrawTriangleBoxs(Obj, Result);
			}
		}
	}
}
//Drawing Mesh Model with light source
void Renderer::DrawMesh(MeshModel& Obj, glm::vec3& color, Camera& Cam, Light& LightSource)
{
	if (Obj.GetStatus())
	{
	
		glm::vec3 tringale_color = glm::ivec3(0, 0, 0);
		glm::mat4x4 Result;
		//unnesscary initalizer of 0.0f,0.0f,0.0f for every Vector3D
		glm::vec3 start_point;
		glm::vec3 middle_point;
		glm::vec3 end_point;
		if (Cam.Orthograhpic_View)
		{
			Result = Cam.OrthographicMatrix * Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
			Cam.Perspective_View = false;
			Cam.FrustumView = false;

		}
		else
		{
			if (Cam.Perspective_View)
			{
				Result = Cam.PerspectiveMatrix * Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
				Cam.Orthograhpic_View = false;
				Cam.FrustumView = false;

			}
			else //if(Cam.frustumView)
			{
				Result = Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
				Cam.Orthograhpic_View = false;
				Cam.Perspective_View = false;
			}

		}
		for (int i = 0; i < Obj.GetFacesCount(); i++)
		{
			start_point = Obj.GetVertex(i, 0);
			end_point = Obj.GetVertex(i, 1);
			middle_point = Obj.GetVertex(i, 2);

			start_point = Result * glm::vec4(start_point, 1);
			end_point = Result * glm::vec4(end_point, 1);
			middle_point = Result * glm::vec4(middle_point, 1);

			start_point.x *= scale; start_point.y *= scale;
			end_point.x *= scale; end_point.y *= scale;
			middle_point.x *= scale; middle_point.y *= scale;

			start_point.x += viewport_width / 2;  start_point.y += viewport_height / 2;
			end_point.x += viewport_width / 2;  end_point.y += viewport_height / 2;
			middle_point.x += viewport_width / 2; middle_point.y += viewport_height / 2;
		

			if (Obj.FillColor)
			{
				if (LightSource.Flat_Shade)
				{
					glm::vec3 new_position = glm::vec3(LightSource.TotalTransformation * glm::vec4(LightSource.Position, 1));
					

					glm::vec3 Face_Normal = glm::triangleNormal(start_point, end_point, middle_point);
					glm::vec3 CenterFace = (start_point / 3.0f + middle_point / 3.0f + end_point / 3.0f);
					glm::vec3 LightVector = new_position - CenterFace;
					LightVector = glm::normalize(LightVector);
					Face_Normal = glm::normalize(Face_Normal);
					float angle = max(glm::dot(Face_Normal, LightVector), 0.0f);
					auto Attenuation = (power2(Distance(new_position.x, new_position.y, new_position.z, CenterFace.x, CenterFace.y, CenterFace.z)) + 1.0f);
					Attenuation = 1 / Attenuation;
					glm::vec3 Ambient_Vector = LightSource.Ambient_Color * Obj.Ambient_Color * Obj.Mesh_Model_Color;
					glm::vec3 Diffuse_Vector = Attenuation * angle * Obj.Material * Obj.Diffuse_Color * LightSource.FlatIntensity * LightSource.Diffuse_Color;
					//Specular
					glm::vec3 CameraVector = glm::normalize(Cam.Position - CenterFace);
					glm::vec3 reflect_Light = glm::normalize(glm::reflect((float)-1 * LightVector, Face_Normal));
					float specular_angle = pow(max(glm::dot(CameraVector, reflect_Light), 0.0f), LightSource.Get_SpecularDegree());
					glm::vec3 Specular_Vector = Attenuation * Obj.Material * LightSource.FlatIntensity * (1 - specular_angle) * Obj.Specular_Color * LightSource.Specular_Color;

					glm::vec3 face_colortotal = (Ambient_Vector + Diffuse_Vector + Specular_Vector);

					FillTringles(start_point, end_point, middle_point, face_colortotal * color);

				}
				else if (LightSource.Gouraud_Shade)
				{
					glm::vec3 new_position = glm::vec3(LightSource.TotalTransformation * glm::vec4(LightSource.Position, 1));

					glm::vec3 Face_Normal = glm::triangleNormal(start_point, end_point, middle_point);
					//add to more normals

					glm::vec3 LightVector1 = new_position - start_point;
					glm::vec3 LightVector2 = new_position - middle_point;
					glm::vec3 LightVector3 = new_position - end_point;

					LightVector1 = glm::normalize(LightVector1);
					LightVector2 = glm::normalize(LightVector2);
					LightVector3 = glm::normalize(LightVector3);

					float angle1 = max(glm::dot(Face_Normal, LightVector1), 0.0f);
					float angle2 = max(glm::dot(Face_Normal, LightVector2), 0.0f);
					float angle3 = max(glm::dot(Face_Normal, LightVector3), 0.0f);

					glm::vec3 Ambient_Vector = LightSource.Ambient_Color * Obj.Ambient_Color;

					auto Attenuation1 = (power2(Distance(new_position.x, new_position.y, new_position.z, start_point.x, start_point.y, start_point.z)) + 1.0f);
					Attenuation1 = 1 / Attenuation1;
					auto Attenuation2 = (power2(Distance(new_position.x, new_position.y, new_position.z, middle_point.x, middle_point.y, middle_point.z)) + 1.0f);
					Attenuation2 = 1 / Attenuation2;
					auto Attenuation3 = (power2(Distance(new_position.x, new_position.y, new_position.z, end_point.x, end_point.y, end_point.z)) + 1.0f);
					Attenuation3 = 1 / Attenuation3;

					/*
					float auttuenion1 = 1.0f / (power2(new_position.x, new_position.y, new_position.z, start_point.x, start_point.y, start_point.z) + 1);
					float auttuenion2 = 1.0f / (power2(new_position.x, new_position.y, new_position.z, middle_point.x, middle_point.y, middle_point.z) + 1);
					float auttuenion3 = 1.0f / (power2(new_position.x, new_position.y, new_position.z, new_position.x, new_position.y, new_position.z) + 1);

					glm::vec3 Diffuse_Vector1 = angle1 * Obj.Material * auttuenion1 * LightSource.Intensity * LightSource.Diffuse_Color;
					glm::vec3 Diffuse_Vector2 = angle2 * Obj.Material * auttuenion2 * LightSource.Intensity * LightSource.Diffuse_Color;
					glm::vec3 Diffuse_Vector3 = angle3 * Obj.Material * auttuenion3 * LightSource.Intensity * LightSource.Diffuse_Color;
					*/
					glm::vec3 Diffuse_Vector1 = angle1 * Attenuation1 * Obj.Material * LightSource.GouraudIntensity * LightSource.Diffuse_Color;
					glm::vec3 Diffuse_Vector2 = angle2 * Attenuation2 * Obj.Material * LightSource.GouraudIntensity * LightSource.Diffuse_Color;
					glm::vec3 Diffuse_Vector3 = angle3 * Attenuation3 * Obj.Material * LightSource.GouraudIntensity * LightSource.Diffuse_Color;
					//Specular
					glm::vec3 CameraVector1 = (Cam.Position - start_point);
					glm::vec3 CameraVector2 = (Cam.Position - middle_point);
					glm::vec3 CameraVector3 = (Cam.Position - end_point);


					glm::vec3 reflect_Light1 = (glm::reflect((float)-1 * LightVector1, Face_Normal));
					glm::vec3 reflect_Light2 = (glm::reflect((float)-1 * LightVector2, Face_Normal));
					glm::vec3 reflect_Light3 = (glm::reflect((float)-1 * LightVector3, Face_Normal));


					float specular_angle1 = pow(max(glm::dot(CameraVector1, reflect_Light1), 0.0f), LightSource.Get_SpecularDegree());
					float specular_angle2 = pow(max(glm::dot(CameraVector2, reflect_Light2), 0.0f), LightSource.Get_SpecularDegree());
					float specular_angle3 = pow(max(glm::dot(CameraVector3, reflect_Light3), 0.0f), LightSource.Get_SpecularDegree());

					glm::vec3 Specular_Vector1 = Obj.Material * Attenuation1 * LightSource.GouraudIntensity * (specular_angle1) * LightSource.Specular_Color;
					glm::vec3 Specular_Vector2 = Obj.Material * Attenuation2 * LightSource.GouraudIntensity * (specular_angle2) * LightSource.Specular_Color;
					glm::vec3 Specular_Vector3 = Obj.Material * Attenuation3 * LightSource.GouraudIntensity * (specular_angle3) * LightSource.Specular_Color;

					glm::vec3 face_colortotal1 = Ambient_Vector + Diffuse_Vector1 + Specular_Vector1;
					glm::vec3 face_colortotal2 = Ambient_Vector + Diffuse_Vector2 + Specular_Vector2;
					glm::vec3 face_colortotal3 = Ambient_Vector + Diffuse_Vector3 + Specular_Vector3;


					FillTringles_Grouand(start_point, end_point, middle_point, face_colortotal1 * color, face_colortotal2 * color, face_colortotal3 * color);

				}
				else if (LightSource.Phong_Shade)
				{
					
					glm::vec3 new_position = glm::vec3(LightSource.TotalTransformation * glm::vec4(LightSource.Position, 1));
					glm::vec3 NormalVector1 = Obj.GetNormal(i, 0);
					glm::vec3 NormalVector2 = Obj.GetNormal(i, 1);
					glm::vec3 NormalVector3 = Obj.GetNormal(i, 2);
					{
						NormalVector1 = Result * glm::vec4(NormalVector1, 1);
						NormalVector2 = Result * glm::vec4(NormalVector2, 1);
						NormalVector3 = Result * glm::vec4(NormalVector3, 1);

						NormalVector1.x *= scale; NormalVector1.y *= scale;
						NormalVector2.x *= scale; NormalVector2.y *= scale;
						NormalVector3.x *= scale; NormalVector3.y *= scale;

						NormalVector1.x += viewport_width / 2;  NormalVector1.y += viewport_height / 2;
						NormalVector2.x += viewport_width / 2;  NormalVector2.y += viewport_height / 2;
						NormalVector3.x += viewport_width / 2;  NormalVector3.y += viewport_height / 2;

						NormalVector1 = glm::normalize(NormalVector1);
						NormalVector2 = glm::normalize(NormalVector2);
						NormalVector3 = glm::normalize(NormalVector3);
					}
					float min_x, max_x;
					min_x = std::min(start_point.x, std::min(middle_point.x, end_point.x));
					max_x = std::max(start_point.x, std::max(middle_point.x, end_point.x));
					float min_y, max_y;
					min_y = std::min(start_point.y, std::min(middle_point.y, end_point.y));
					max_y = std::max(start_point.y, std::max(middle_point.y, end_point.y));
				
					
					for (int j = max_y; j >= min_y; j--)
					{
						for (int i = min_x; i < max_x; i++)
						{
							if (PointInTriangle(glm::vec2(i, j), start_point, middle_point, end_point) && InWindow(i, j))
							{
								auto Attenuation = (power2(Distance(new_position.x, new_position.y, new_position.z, i, j, 0)) + 1.0f);
								Attenuation = 1 / Attenuation;
								
								glm::vec3 LightVector = new_position - glm::vec3(i,j,0);
								LightVector = glm::normalize(LightVector);
								glm::vec3 VertexNormal = NormalInterpolation(i, j, start_point, middle_point, end_point, NormalVector1, NormalVector2, NormalVector3);
								//VertexNormal = glm::normalize(VertexNormal);

								glm::vec3 Ambient_Vector = LightSource.Ambient_Color * Obj.Ambient_Color;
								
								float angle = max(glm::dot(VertexNormal, LightVector), 0.0f);
								glm::vec3 Diffuse_Vector = Attenuation * LightSource.PhongIntensity * angle * LightSource.Diffuse_Color;
								
								glm::vec3 CameraVector = glm::normalize(Cam.Position - glm::vec3(i, j, 0));
								glm::vec3 reflect_Light = glm::normalize(glm::reflect(LightVector, VertexNormal));
								float specular_angle = pow(max(glm::dot(CameraVector, reflect_Light), 0.0f), LightSource.Get_SpecularDegree());
								glm::vec3 Specular_Vector = Obj.Material * LightSource.PhongIntensity * Attenuation * (specular_angle) * LightSource.Specular_Color;
								
								glm::vec3 Vertex_colortotal = Ambient_Vector + Diffuse_Vector + Specular_Vector;

								Vertex_colortotal = ColorVector(Vertex_colortotal);

								//glm::vec3 pixel_color = LightInterpolation(i, j, start_point, middle_point, end_point, Normal1_Color, Normal_2_Color, Normal_3_Color);
								PutPixel(i, j, Vertex_colortotal);
							}
						}
					}
					
				}
				else
				{
				    if (LightSource.ShowReflectedRays)
				    {
						glm::vec3 new_position = glm::vec3(LightSource.TotalTransformation * glm::vec4(LightSource.Position, 1));
						glm::vec3 CenterFace = (start_point / 3.0f + middle_point / 3.0f + end_point / 3.0f);
						glm::vec3 Face_Normal = glm::triangleNormal(start_point, end_point, middle_point);
						glm::vec3 LightVector = new_position - CenterFace;

						glm::vec3 CameraVector = glm::normalize(Cam.Position - CenterFace);
						glm::vec3 reflect_Light = glm::normalize(glm::reflect((float)-1 * LightVector, Face_Normal));
						DrawLine(CenterFace, CenterFace + (float)VectorLength * reflect_Light, glm::vec3(1.0f, 1.0f, 1.0f));
			     	}
					FillTringles(start_point, end_point, middle_point, color);
				}

			}
			if (!(Obj.OffGrid))
			{
				Renderer::DrawLine(glm::vec2(start_point.x, start_point.y), glm::vec2(end_point.x, end_point.y), Obj.Mesh_Model_Color);
				Renderer::DrawLine(glm::vec2(start_point.x, start_point.y), glm::vec2(middle_point.x, middle_point.y), Obj.Mesh_Model_Color);
				Renderer::DrawLine(glm::vec2(end_point.x, end_point.y), glm::vec2(middle_point.x, middle_point.y), Obj.Mesh_Model_Color);
			}

			DrawMeshFeatures(Obj, start_point, middle_point, end_point, Obj.Mesh_Model_Color, Cam, LightSource);

		}
		{
			Obj.ExtractZeroPoint();
			glm::vec3 Center_Point = Obj.Get_ZeroPoint();
			//Center_Point = Obj.Get_CurrentMeshModelMatrix() * glm::vec4(Center_Point, 1);
			Center_Point = Obj.Get_CurrentMeshModelMatrix() * glm::vec4(Center_Point, 1);
			Center_Point.x *= scale; Center_Point.y *= scale;
			Center_Point.x += viewport_width / 2; Center_Point.y += viewport_height / 2;

			Obj.Set_ZeroPoint(Center_Point);
			Obj.Adjust_Axis();
			if (Obj.RectangleStatus)
			{
				DrawBox(Obj, Result, Obj.Rectangle_X1, Obj.Rectangle_X2, Obj.Rectangle_Y1, Obj.Rectangle_Y2, Obj.Rectangle_Z1, Obj.Rectangle_Z2, color);
			}
			glm::vec3 RedColor = glm::vec3(1.0f, 0.0f, 0.0f);
			if (Obj.NormalStatus)
			{
				DrawNormals(Obj, Result, 50, Obj.Normals_Mesh_Model_Color);
			}
			if (Obj.NormalStatusPerFaces)
			{
				DrawNormalsPerFaces(Obj, Result, 50, Obj.Normals_Mesh_Model_Color);
			}
			if (Obj.VerticesColorful)
			{
				DrawTriangleBoxs(Obj, Result);
			}
		}
	}
}
void Renderer::DrawMesh(MeshModel& Obj, glm::vec3& color, Camera& Cam, Scene& scene)
{
	if (Obj.GetStatus())
	{
		glm::vec3 tringale_color = glm::ivec3(0, 0, 0);
		glm::mat4x4 Result;
		//unnesscary initalizer of 0.0f,0.0f,0.0f for every Vector3D
		glm::vec3 start_point;
		glm::vec3 middle_point;
		glm::vec3 end_point;
		bool FlatShade = false, GouraudShade = false, PhongShade = false;
		if (Cam.Orthograhpic_View)
		{
			Result = Cam.OrthographicMatrix * Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
			Cam.Perspective_View = false;
			Cam.FrustumView = false;

		}
		else
		{
			if (Cam.Perspective_View)
			{
				Result = Cam.PerspectiveMatrix * Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
				Cam.Orthograhpic_View = false;
				Cam.FrustumView = false;

			}
			else //if(Cam.frustumView)
			{
				Result = Cam.FrustumMatrix * Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
				Cam.Orthograhpic_View = false;
				Cam.Perspective_View = false;
			}

		}
		for (int i = 0; i < Obj.GetFacesCount(); i++)
		{

			start_point = Obj.GetVertex(i, 0);
			end_point = Obj.GetVertex(i, 1);
			middle_point = Obj.GetVertex(i, 2);
			
			if (FaceCuling(Result, start_point, middle_point, end_point, Cam))
			{
				start_point = Result * glm::vec4(start_point, 1);
				end_point = Result * glm::vec4(end_point, 1);
				middle_point = Result * glm::vec4(middle_point, 1);

				start_point.x *= scale; start_point.y *= scale;
				end_point.x *= scale; end_point.y *= scale;
				middle_point.x *= scale; middle_point.y *= scale;

				start_point.x += viewport_width / 2;  start_point.y += viewport_height / 2;
				end_point.x += viewport_width / 2;  end_point.y += viewport_height / 2;
				middle_point.x += viewport_width / 2; middle_point.y += viewport_height / 2;


				if (Obj.FillColor)
				{
					for (int j = 0; j < scene.GetLightsCount(); j++)
					{
						if (scene.GetLight(j).Flat_Shade)
						{
							/*for (int k = 0; k < scene.GetLightsCount(); k++)
							{
								scene.GetLight(k).Flat_Shade = true;
								scene.GetLight(k).Gouraud_Shade = false;
								scene.GetLight(k).Phong_Shade = false;
							}*/
							FlatShade = true;
						}
						else if (scene.GetLight(j).Gouraud_Shade)
						{
							/*for (int k = 0; k < scene.GetLightsCount(); k++)
							{
								scene.GetLight(k).Flat_Shade = false;
								scene.GetLight(k).Gouraud_Shade = true;
								scene.GetLight(k).Phong_Shade = false;
							}*/
							GouraudShade = true;
						}
						else if (scene.GetLight(j).Phong_Shade)
						{
							/*for (int k = 0; k < scene.GetLightsCount(); k++)
							{
								scene.GetLight(k).Flat_Shade = false;
								scene.GetLight(k).Gouraud_Shade = false;
								scene.GetLight(k).Phong_Shade = true;
							}*/
							PhongShade = true;
						}
					}
					if (FlatShade)
					{
						glm::vec3 face_colortotal = glm::vec3(0.0f);
						glm::vec3 new_position, LightVector;
						for (int j = 0; j < scene.GetLightsCount(); j++)
						{
							Light LightSource = scene.GetLight(j);
							float Attenuation;
							new_position = glm::vec3(LightSource.TotalTransformation * glm::vec4(LightSource.Position, 1));

							glm::vec3 Face_Normal = glm::triangleNormal(start_point, end_point, middle_point);
							glm::vec3 CenterFace = (start_point / 3.0f + middle_point / 3.0f + end_point / 3.0f);
							if (LightSource.LightRound || LightSource.LightRectangle)
							{
								LightVector = new_position - CenterFace;
							}
							else //LightSource.LightLine
							{
								LightVector = LightSource.LightLineVector;
							}

							LightVector = glm::normalize(LightVector);
							Face_Normal = glm::normalize(Face_Normal);
							float angle = max(glm::dot(Face_Normal, LightVector), 0.0f);
							if (LightSource.LightRound || LightSource.LightRectangle)
							{
								Attenuation = (power2(Distance(new_position.x, new_position.y, new_position.z, CenterFace.x, CenterFace.y, CenterFace.z)) + 1.0f);
								Attenuation = 1 / Attenuation;
							}
							else
							{
								Attenuation = DistanceOfLine(LightSource.LinePosition1, LightSource.LinePosition2, CenterFace) + 1.0f;
								Attenuation = 1 / Attenuation;
							}

							glm::vec3 Ambient_Vector = LightSource.Ambient_Color * Obj.Ambient_Color * Obj.Mesh_Model_Color;
							Ambient_Vector = ColorVector(Ambient_Vector);
							glm::vec3 Diffuse_Vector = Attenuation * angle * Obj.Material * Obj.Diffuse_Color * LightSource.FlatIntensity * LightSource.Diffuse_Color;
							Diffuse_Vector = ColorVector(Diffuse_Vector);

							//Specular
							glm::vec3 CameraVector = glm::normalize(Cam.Position - CenterFace);
							glm::vec3 reflect_Light = glm::normalize(glm::reflect((float)-1 * LightVector, Face_Normal));
							float specular_angle = pow(max(glm::dot(CameraVector, reflect_Light), 0.0f), LightSource.Get_SpecularDegree());
							glm::vec3 Specular_Vector = Attenuation * Obj.Material * LightSource.FlatIntensity * (specular_angle)*Obj.Specular_Color * LightSource.Specular_Color;
							Specular_Vector = ColorVector(Specular_Vector);

							face_colortotal += (Ambient_Vector + Diffuse_Vector + Specular_Vector);
							face_colortotal = ColorVector(face_colortotal);

						}
						FillTringles(start_point, end_point, middle_point, face_colortotal);

					}
					else if (GouraudShade)
					{
						glm::vec3 face_colortotal1 = glm::vec3(0);
						glm::vec3 face_colortotal2 = glm::vec3(0);
						glm::vec3 face_colortotal3 = glm::vec3(0);
						for (int j = 0; j < scene.GetLightsCount(); j++)
						{
							Light& LightSource = scene.GetLight(j);

							glm::vec3 new_position = glm::vec3(LightSource.TotalTransformation * glm::vec4(LightSource.Position, 1));

							glm::vec3 Face_Normal_1 = Obj.GetNormal(i, 0);
							glm::vec3 Face_Normal_2 = Obj.GetNormal(i, 0);
							glm::vec3 Face_Normal_3 = Obj.GetNormal(i, 0);


							glm::vec3 LightVector1 = new_position - start_point;
							glm::vec3 LightVector2 = new_position - middle_point;
							glm::vec3 LightVector3 = new_position - end_point;

							LightVector1 = glm::normalize(LightVector1);
							LightVector2 = glm::normalize(LightVector2);
							LightVector3 = glm::normalize(LightVector3);

							float angle1 = max(glm::dot(Face_Normal_1, LightVector1), 0.0f);
							float angle2 = max(glm::dot(Face_Normal_2, LightVector2), 0.0f);
							float angle3 = max(glm::dot(Face_Normal_3, LightVector3), 0.0f);

							glm::vec3 Ambient_Vector = LightSource.Ambient_Color * Obj.Ambient_Color;

							auto Attenuation1 = (power2(Distance(new_position.x, new_position.y, new_position.z, start_point.x, start_point.y, start_point.z)) + 1.0f);
							Attenuation1 = 1 / Attenuation1;
							auto Attenuation2 = (power2(Distance(new_position.x, new_position.y, new_position.z, middle_point.x, middle_point.y, middle_point.z)) + 1.0f);
							Attenuation2 = 1 / Attenuation2;
							auto Attenuation3 = (power2(Distance(new_position.x, new_position.y, new_position.z, end_point.x, end_point.y, end_point.z)) + 1.0f);
							Attenuation3 = 1 / Attenuation3;
							Attenuation1 = 1.0F;
							Attenuation2 = 1.0F;
							Attenuation3 = 1.0F;
							/*
							float auttuenion1 = 1.0f / (power2(new_position.x, new_position.y, new_position.z, start_point.x, start_point.y, start_point.z) + 1);
							float auttuenion2 = 1.0f / (power2(new_position.x, new_position.y, new_position.z, middle_point.x, middle_point.y, middle_point.z) + 1);
							float auttuenion3 = 1.0f / (power2(new_position.x, new_position.y, new_position.z, new_position.x, new_position.y, new_position.z) + 1);

							glm::vec3 Diffuse_Vector1 = angle1 * Obj.Material * auttuenion1 * LightSource.Intensity * LightSource.Diffuse_Color;
							glm::vec3 Diffuse_Vector2 = angle2 * Obj.Material * auttuenion2 * LightSource.Intensity * LightSource.Diffuse_Color;
							glm::vec3 Diffuse_Vector3 = angle3 * Obj.Material * auttuenion3 * LightSource.Intensity * LightSource.Diffuse_Color;
							*/
							glm::vec3 Diffuse_Vector1 = angle1 * Attenuation1 * Obj.Material * LightSource.GouraudIntensity * LightSource.Diffuse_Color;
							glm::vec3 Diffuse_Vector2 = angle2 * Attenuation2 * Obj.Material * LightSource.GouraudIntensity * LightSource.Diffuse_Color;
							glm::vec3 Diffuse_Vector3 = angle3 * Attenuation3 * Obj.Material * LightSource.GouraudIntensity * LightSource.Diffuse_Color;
							//Specular
							glm::vec3 CameraVector1 = glm::normalize(Cam.Position - start_point);
							glm::vec3 CameraVector2 = glm::normalize(Cam.Position - middle_point);
							glm::vec3 CameraVector3 = glm::normalize(Cam.Position - end_point);


							glm::vec3 reflect_Light1 = glm::normalize(glm::reflect((float)-1 * LightVector1, Face_Normal_1));
							glm::vec3 reflect_Light2 = glm::normalize(glm::reflect((float)-1 * LightVector2, Face_Normal_2));
							glm::vec3 reflect_Light3 = glm::normalize(glm::reflect((float)-1 * LightVector3, Face_Normal_3));


							float specular_angle1 = pow(max(glm::dot(CameraVector1, reflect_Light1), 0.0f), LightSource.Get_SpecularDegree());
							float specular_angle2 = pow(max(glm::dot(CameraVector2, reflect_Light2), 0.0f), LightSource.Get_SpecularDegree());
							float specular_angle3 = pow(max(glm::dot(CameraVector3, reflect_Light3), 0.0f), LightSource.Get_SpecularDegree());

							glm::vec3 Specular_Vector1 = Obj.Material * Attenuation1 * LightSource.GouraudIntensity * (specular_angle1)*LightSource.Specular_Color;
							glm::vec3 Specular_Vector2 = Obj.Material * Attenuation2 * LightSource.GouraudIntensity * (specular_angle2)*LightSource.Specular_Color;
							glm::vec3 Specular_Vector3 = Obj.Material * Attenuation3 * LightSource.GouraudIntensity * (specular_angle3)*LightSource.Specular_Color;

							face_colortotal1 += Ambient_Vector + Diffuse_Vector1 + Specular_Vector1;
							face_colortotal2 += Ambient_Vector + Diffuse_Vector2 + Specular_Vector2;
							face_colortotal3 += Ambient_Vector + Diffuse_Vector3 + Specular_Vector3;

						}
						FillTringles_Grouand(start_point, end_point, middle_point, face_colortotal1 * color, face_colortotal2 * color, face_colortotal3 * color);

					}
					else if (PhongShade)
					{
						Light& LightSource = scene.GetLight(0);

						glm::vec3 new_position = glm::vec3(LightSource.TotalTransformation * glm::vec4(LightSource.Position, 1));
						glm::vec3 NormalVector1 = Obj.GetNormal(i, 0);
						glm::vec3 NormalVector2 = Obj.GetNormal(i, 1);
						glm::vec3 NormalVector3 = Obj.GetNormal(i, 2);
						{
							glm::vec4 NormalVector1_V4 = Result * glm::vec4(NormalVector1, 1);
							glm::vec4 NormalVector2_V4 = Result * glm::vec4(NormalVector2, 1);
							glm::vec4 NormalVector3_V4 = Result * glm::vec4(NormalVector3, 1);

							if (NormalVector1_V4.w != 0 && NormalVector2_V4.w != 0 && NormalVector3_V4.w != 0)
							{
								NormalVector1 = NormalVector1_V4 / NormalVector1_V4.w;
								NormalVector2 = NormalVector2_V4 / NormalVector2_V4.w;
								NormalVector3 = NormalVector3_V4 / NormalVector3_V4.w;

							}
							/*NormalVector1.x *= scale; NormalVector1.y *= scale;
							NormalVector2.x *= scale; NormalVector2.y *= scale;
							NormalVector3.x *= scale; NormalVector3.y *= scale;

							NormalVector1.x += viewport_width / 2;  NormalVector1.y += viewport_height / 2;
							NormalVector2.x += viewport_width / 2;  NormalVector2.y += viewport_height / 2;
							NormalVector3.x += viewport_width / 2;  NormalVector3.y += viewport_height / 2;*/

							NormalVector1 = glm::normalize(NormalVector1);
							NormalVector2 = glm::normalize(NormalVector2);
							NormalVector3 = glm::normalize(NormalVector3);
						}
						float min_x, max_x;
						min_x = std::min(start_point.x, std::min(middle_point.x, end_point.x));
						max_x = std::max(start_point.x, std::max(middle_point.x, end_point.x));
						float min_y, max_y;
						min_y = std::min(start_point.y, std::min(middle_point.y, end_point.y));
						max_y = std::max(start_point.y, std::max(middle_point.y, end_point.y));


						for (int j = max_y; j >= min_y; j--)
						{
							for (int i = min_x; i < max_x; i++)
							{
								if (PointInTriangle(glm::vec2(i, j), start_point, middle_point, end_point) && InWindow(i, j))
								{
									auto Attenuation = (power2(Distance(new_position.x, new_position.y, new_position.z, i, j, 0)) + 1.0f);
									Attenuation = 1 / Attenuation;
									Attenuation = 1.0F;
									glm::vec3 VerticePosition = glm::vec3(i, j, 0.0f);
									glm::vec3 LightVector = new_position - VerticePosition;
									LightVector = glm::normalize(LightVector);
									glm::vec3 VertexNormal = NormalInterpolation(i, j, start_point, middle_point, end_point, NormalVector1, NormalVector2, NormalVector3);
									VertexNormal = glm::normalize(VertexNormal);

									glm::vec3 Ambient_Vector = LightSource.Ambient_Color * Obj.Ambient_Color;

									float angle = max(glm::dot(VertexNormal, LightVector), 0.0f);
									glm::vec3 Diffuse_Vector = Attenuation * LightSource.PhongIntensity * angle * LightSource.Diffuse_Color;

									glm::vec3 CameraVector = glm::normalize(Cam.Position - VerticePosition);
									glm::vec3 reflect_Light = glm::normalize(glm::reflect(LightVector, VertexNormal));
									float specular_angle = pow(max(glm::dot(CameraVector, reflect_Light), 0.0f), LightSource.Get_SpecularDegree());
									glm::vec3 Specular_Vector = Obj.Material * LightSource.PhongIntensity * Attenuation * (specular_angle)*LightSource.Specular_Color;

									glm::vec3 Vertex_colortotal = Ambient_Vector + Diffuse_Vector + Specular_Vector;

									Vertex_colortotal = ColorVector(Vertex_colortotal);

									//glm::vec3 pixel_color = LightInterpolation(i, j, start_point, middle_point, end_point, Normal1_Color, Normal_2_Color, Normal_3_Color);
									PutPixel(i, j, Vertex_colortotal);
								}
							}
						}

					}
					else
						if (!(FlatShade || GouraudShade || PhongShade))
						{
							for (int i = 0; i < scene.GetLightsCount(); i++)
							{
								Light LightSource = scene.GetLight(i);
								if (LightSource.ShowReflectedRays)
								{
									glm::vec3 new_position = glm::vec3(LightSource.TotalTransformation * glm::vec4(LightSource.Position, 1));
									glm::vec3 CenterFace = (start_point / 3.0f + middle_point / 3.0f + end_point / 3.0f);

									glm::vec3 Face_Normal = glm::triangleNormal(start_point, end_point, middle_point);
									glm::vec3 LightVector = new_position - CenterFace;
									LightVector = glm::normalize(LightVector);
									//DrawLine(new_position, new_position - (float)VectorLength * LightVector, glm::vec3(1.0f, 1.0f, 1.0f));
									glm::vec3 CameraVector = glm::normalize(Cam.Position - CenterFace);
									glm::vec3 reflect_Light = glm::normalize(glm::reflect((float)-1 * LightVector, Face_Normal));
									DrawLine(CenterFace, CenterFace + (float)VectorLength * reflect_Light, glm::vec3(1.0f, 1.0f, 1.0f));
								}
							}

							FillTringles(start_point, end_point, middle_point, color);
						}

				}
				if (!(Obj.OffGrid))
				{
					Renderer::DrawLine(glm::vec2(start_point.x, start_point.y), glm::vec2(end_point.x, end_point.y), Obj.Mesh_Model_Color);
					Renderer::DrawLine(glm::vec2(start_point.x, start_point.y), glm::vec2(middle_point.x, middle_point.y), Obj.Mesh_Model_Color);
					Renderer::DrawLine(glm::vec2(end_point.x, end_point.y), glm::vec2(middle_point.x, middle_point.y), Obj.Mesh_Model_Color);
				}

				DrawMeshFeatures(Obj, start_point, middle_point, end_point, Obj.Mesh_Model_Color, Cam);
			}
		}

		try
		{
			if (!(Obj.GetStatus()))
			{
				throw -1;
			}
			Obj.ExtractZeroPoint();
			glm::vec3 Center_Point = Obj.Get_ZeroPoint();
			//Center_Point = Obj.Get_CurrentMeshModelMatrix() * glm::vec4(Center_Point, 1);
			Center_Point = Obj.Get_CurrentMeshModelMatrix() * glm::vec4(Center_Point, 1);
			Center_Point.x *= scale; Center_Point.y *= scale;
			Center_Point.x += viewport_width / 2; Center_Point.y += viewport_height / 2;

			Obj.Set_ZeroPoint(Center_Point);
			Obj.Adjust_Axis();
			if (Obj.RectangleStatus)
			{
				DrawBox(Obj, Result, Obj.Rectangle_X1, Obj.Rectangle_X2, Obj.Rectangle_Y1, Obj.Rectangle_Y2, Obj.Rectangle_Z1, Obj.Rectangle_Z2, color);
			}
			glm::vec3 RedColor = glm::vec3(1.0f, 0.0f, 0.0f);
			if (Obj.NormalStatus)
			{
				DrawNormals(Obj, Result, 50, Obj.Normals_Mesh_Model_Color);
			}
			if (Obj.NormalStatusPerFaces)
			{
				DrawNormalsPerFaces(Obj, Result, 50, Obj.Normals_Mesh_Model_Color);
			}
			if (Obj.VerticesColorful)
			{
				DrawTriangleBoxs(Obj, Result);
			}
		}
		catch (int a)
		{
			cout << "Mesh Model Status Is Off" << endl;
		}
	}
}

//Function that calls all Mesh Model Features:
void Renderer::DrawMeshFeatures(MeshModel& Obj, glm::vec3 Start, glm::vec3 Middle, glm::vec3 End, const glm::vec3& color, Camera& Cam, Light& LightSource)
{
	glm::ivec3 tringale_color;
	glm::mat4x4 Result = glm::mat4x4(1);

	if (Obj.GetStatus())
	{
		//Camera Projection View SetUp
		if (Cam.Orthograhpic_View)
		{
			Result = Cam.OrthographicMatrix * Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
			Cam.Perspective_View = false;
			Cam.FrustumView = false;
		}
		else
		{
			if (Cam.Perspective_View)
			{
				Result = Cam.PerspectiveMatrix * Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
				Cam.Orthograhpic_View = false;
				Cam.FrustumView = false;
			}
			else //if(Cam.frustumView)
			{
				Result = Cam.FrustumMatrix * Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
				Cam.Orthograhpic_View = false;
				Cam.Perspective_View = false;
			}

		}

		if (Obj.SubDivision > 1)
		{
			SubDivision(Start, Middle, End, Obj.SubDivision, color);
		}
		if (Obj.Color_MeshModel_Triangles)
		{
			float R_Color = (rand() % (255));
			float G_Color = (rand() % (255));
			float B_Color = (rand() % (255));

			R_Color = R_Color / 100;
			G_Color = G_Color / 100;
			B_Color = B_Color / 100;
			tringale_color = glm::ivec3(R_Color, G_Color, B_Color);
			FillTringles(Start, Middle, End, tringale_color);
		}

		if (Obj.FillBlackGray)
		{
			ScanLine(Start, Middle, End);
		}
	}
}
void Renderer::DrawMeshFeatures(MeshModel& Obj, glm::vec3 Start, glm::vec3 Middle, glm::vec3 End, const glm::vec3& color, Camera& Cam)
{
	glm::ivec3 tringale_color;
	glm::mat4x4 Result = glm::mat4x4(1);

	if (Obj.GetStatus())
	{
		//Camera Projection View SetUp
		if (Cam.Orthograhpic_View)
		{
			Result = Cam.OrthographicMatrix * Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
			Cam.Perspective_View = false;
			Cam.FrustumView = false;

		}
		else
		{
			if (Cam.Perspective_View)
			{
				Result = Cam.PerspectiveMatrix * Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
				Cam.Orthograhpic_View = false;
				Cam.FrustumView = false;

			}
			else //if(Cam.frustumView)
			{
				Result = Cam.FrustumMatrix * Cam.lookat * glm::inverse(Cam.TotalTransform) * Obj.Get_CurrentMeshModelMatrix();
				Cam.Orthograhpic_View = false;
				Cam.Perspective_View = false;
			}

		}

		if (Obj.SubDivision > 1)
		{
			SubDivision(Start, Middle, End, Obj.SubDivision, color);
		}
		if (Obj.Color_MeshModel_Triangles)
		{
			float R_Color = (rand() % (255));
			float G_Color = (rand() % (255));
			float B_Color = (rand() % (255));

			R_Color = R_Color / 100;
			G_Color = G_Color / 100;
			B_Color = B_Color / 100;
			tringale_color = glm::ivec3(R_Color, G_Color, B_Color);
			FillTringles(Start, Middle, End, tringale_color);
		}
		if (Obj.FillBlackGray)
		{
			ScanLine(Start, Middle, End);
		}
	}
}

//Tool Functions
bool Renderer::InWindow(int i, int j)
{
	if ((i >= 0 && i <= viewport_width) && (j >= 0 && j <= viewport_height))
	{
		return true;
	}
	else
	{
		return false;
	}
}
float Distance(float x, float y , float z, float x2, float y2 , float z2)
{
	return sqrt(power2(x - x2) + power2(y - y2) + power2(z - z2));
}
bool Renderer::IsRound(float x, float y, glm::vec2 position, float radius)
{
	if ((position.x - x) * (position.x - x) + (position.y - y) * (position.y - y) <= (radius) * (radius))
		return true;
	else return false;
}
//Paint a Mesh Model Faces Walking Edge & ScanLine
void Renderer::FillTringles(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 Color)
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
void Renderer::ScanLine(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3)
{
	float min_x, max_x;
	min_x = std::min(point1.x, std::min(point2.x, point3.x));
	max_x = std::max(point1.x, std::max(point2.x, point3.x));
	float min_y, max_y;
	min_y = std::min(point1.y, std::min(point2.y, point3.y));
	max_y = std::max(point1.y, std::max(point2.y, point3.y));
	/*float min_z, max_z;
	min_z = std::min(point1.z, std::min(point2.z, point3.z));
	max_z = std::max(point1.z, std::max(point2.z, point3.z));*/

	int index;
	float new_depth, previous_depth;
	for (int j = max_y; j >= min_y; j--)
	{
		for (int i = min_x; i < max_x; i++)
		{
			if (PointInTriangle(glm::vec2(i, j), point1, point2, point3) && InWindow(i, j))
			{
				//PutPixel(i, j, glm::ivec3(123, 12, 200));
				index = Z_INDEX(viewport_width, i, j);
				new_depth = DepthCalculation(i, j, point1, point2, point3);
				previous_depth = z_buffer[index];
				if (previous_depth == INFINITY)
				{
					z_buffer[index] = new_depth;
				}
				else
				{
					if (previous_depth > new_depth)
					{
						z_buffer[index] = previous_depth;
					}
					else
					{
						z_buffer[index] = new_depth;
					}
				}
			}
		}
	}

}

//Calculating Depth (Z-Buffer)
float Renderer::area(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
{
	if (abs((p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y)) / 2.0) == INFINITY)
	{
		return 0;
	}
	else
		return abs((p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y)) / 2.0);
}
float Renderer::DepthCalculation(float x, float y, glm::vec3 point1, glm::vec3 point2, glm::vec3 point3)
{
	float area_12 = area(glm::vec2(x, y), point1, point2);
	float area_13 = area(glm::vec2(x, y), point1, point3);
	float area_23 = area(glm::vec2(x, y), point2, point3);

	float total_area = area(point1, point2, point3);

	return((area_23 / total_area) * point1.z + (area_13 / total_area) * point2.z + (area_12 / total_area) * point3.z);

}
bool Renderer::PointInTriangle(glm::ivec2 p, glm::ivec2 p0, glm::ivec2 p1, glm::ivec2 p2)
{
	float r = (p0.x - p2.x) * (p.y - p2.y) - (p0.y - p2.y) * (p.x - p2.x);
	float t = (p1.x - p0.x) * (p.y - p0.y) - (p1.y - p0.y) * (p.x - p0.x);

	if ((r < 0) != (t < 0) && r != 0 && t != 0)
		return false;

	float d = (p2.x - p1.x) * (p.y - p1.y) - (p2.y - p1.y) * (p.x - p1.x);
	return d == 0 || (d < 0) == (r + t <= 0);
}
void Renderer::DepthCalculationInterpolation(float Left, float Right, float current_y, glm::vec3 point1, glm::vec3 point2, glm::vec3 point3)
{
	if (Left > Right)
	{
		DepthCalculationInterpolation(Right, Left, current_y, point1, point2, point3);
	}
	if (Right == Left)
	{
		return;
	}
	float current = Left + 1;
	if (!(point1.x - point2.x == 0 || point1.x - point3.x == 0))
	{
		float alpha_left = (Left - point2.x) / (point1.x - point2.x);
		float alpha_right = (Right - point3.x) / (point1.x - point3.x);

		z_buffer[Z_INDEX(viewport_width, (int)Left, (int)current_y)] = alpha_left * point1.z;
		z_buffer[Z_INDEX(viewport_width, (int)Right, (int)current_y)] = alpha_right;

		float tilt;

		while (current <= Right)
		{
			tilt = (current - Right) / (Left - Right);

			z_buffer[Z_INDEX(viewport_width, (int)current, (int)current_y)] = alpha_right * tilt + (1 - tilt) * alpha_left;

			current++;
		}
	}
	else
	{
		float alpha_left = (current_y - point2.y) / (point1.y - point2.y);
		float alpha_right = (current_y - point3.y) / (point1.y - point3.y);

		z_buffer[Z_INDEX(viewport_width, (int)Left, (int)current_y)] = alpha_left;
		z_buffer[Z_INDEX(viewport_width, (int)Right, (int)current_y)] = alpha_right;

		float tilt;

		while (current <= Right)
		{
			tilt = (current - Right) / (Left - Right);

			z_buffer[Z_INDEX(viewport_width, (int)current, (int)current_y)] = alpha_right * tilt + (1 - tilt) * alpha_left;

			current++;
		}


	}



}
void Renderer::ExtractMinMax()
{
	for (int i = 0; i < viewport_height * viewport_width; i++)
	{
		if (z_buffer[i] != INFINITY)
		{
			z_max = z_buffer[i];
			z_min = z_buffer[i];
		}
	}
	for (int i = 0; i < viewport_height * viewport_width; i++)
	{
		if (z_buffer[i] != INFINITY)
		{
			if (z_buffer[i] > z_max)
			{
				z_max = z_buffer[i];
			}
			if (z_buffer[i] < z_min)
			{
				z_min = z_buffer[i];
			}
		}
	}
}
//Mesh Model Features + Extensions
void Renderer::DrawForMeshModelAxis(MeshModel& Obj)
{
	if (Obj.AxisStatus)
	{

		/*	glm::ivec3 RedColor = glm::ivec3(1.0f, 0.0f, 0.0f);
			glm::ivec3 GreenColor = glm::ivec3(0.0f, 1.0f, 0.0f);
			glm::ivec3 BlueColor = glm::ivec3(0.0f, 0.0f, 1.0f);
			*/
		glm::ivec3 YellowColor = glm::ivec3(1.0f, 1.0f, 0.0f);
		glm::ivec3 LightBlueColor = glm::ivec3(0.0f, 1.0f, 1.0f);
		glm::ivec3 PinkColor = glm::ivec3(1.0f, 0.0f, 1.0f);

		/*	Obj.Set_XAxis(glm::vec3(Obj.Get_RotateTotalMeshModel() * glm::vec4(Obj.Get_XAxis(),1)));
			Obj.Set_YAxis(glm::vec3(Obj.Get_RotateTotalMeshModel() * glm::vec4(Obj.Get_YAxis(), 1)));
			Obj.Set_ZAxis(glm::vec3(Obj.Get_RotateTotalMeshModel() * glm::vec4(Obj.Get_ZAxis(), 1)));*/


		DrawLine(Obj.Get_ZeroPoint(), Obj.Get_XAxis(), YellowColor);
		DrawLine(Obj.Get_ZeroPoint(), Obj.Get_YAxis(), LightBlueColor);
		DrawLine(Obj.Get_ZeroPoint(), Obj.Get_ZAxis(), PinkColor);
	}

}
void Renderer::DrawForWorldAxis(MeshModel& Obj)
{
	if (Obj.WorldAxisStatus)
	{
		glm::ivec3 RedColor = glm::ivec3(1.0f, 0.0f, 0.0f);
		glm::ivec3 GreenColor = glm::ivec3(0.0f, 1.0f, 0.0f);
		glm::ivec3 BlueColor = glm::ivec3(0.0f, 0.0f, 1.0f);

		glm::vec3 Center_WorldAxis = glm::vec3(viewport_width / 2, viewport_height / 2, 0);
		glm::vec3 X_WorldAxis = glm::vec3(viewport_width, viewport_height / 2, 0);
		glm::vec3 Y_WorldAxis = glm::vec3(viewport_width / 2, viewport_height, 0);
		glm::vec3 Z_WorldAxis = glm::vec3(viewport_width / 2, viewport_height / 2, 200);

		Center_WorldAxis = glm::vec3(Obj.World_matrix * glm::vec4(Center_WorldAxis, 1));
		X_WorldAxis = glm::vec3(Obj.World_matrix * glm::vec4(X_WorldAxis, 1));
		Y_WorldAxis = glm::vec3(Obj.World_matrix * glm::vec4(Y_WorldAxis, 1));
		Z_WorldAxis = glm::vec3(Obj.World_matrix * glm::vec4(Z_WorldAxis, 1));

		DrawLine(Center_WorldAxis, X_WorldAxis, RedColor);
		DrawLine(Center_WorldAxis, Y_WorldAxis, GreenColor);
		DrawLine(Center_WorldAxis, Z_WorldAxis, BlueColor);
	}
}
void Renderer::DrawBox(MeshModel Obj, glm::mat4x4 input, float Rectangle_X1, float Rectangle_X2, float Rectangle_Y1, float Rectangle_Y2, float Rectangle_Z1, float Rectangle_Z2, glm::vec3 Color)
{
	if (Obj.RectangleStatus)
	{
		glm::vec3 point1, point2, point3, point4, point5, point6, point7, point8;

		point1 = glm::vec3(Rectangle_X1, Rectangle_Y1, Rectangle_Z1);
		point2 = glm::vec3(Rectangle_X1, Rectangle_Y1, Rectangle_Z2);

		point3 = glm::vec3(Rectangle_X1, Rectangle_Y2, Rectangle_Z1);
		point4 = glm::vec3(Rectangle_X1, Rectangle_Y2, Rectangle_Z2);

		point5 = glm::vec3(Rectangle_X2, Rectangle_Y1, Rectangle_Z1);
		point6 = glm::vec3(Rectangle_X2, Rectangle_Y1, Rectangle_Z2);

		point7 = glm::vec3(Rectangle_X2, Rectangle_Y2, Rectangle_Z1);
		point8 = glm::vec3(Rectangle_X2, Rectangle_Y2, Rectangle_Z2);

		point1 = glm::vec3(input * glm::vec4(point1, 1));
		point2 = glm::vec3(input * glm::vec4(point2, 1));
		point3 = glm::vec3(input * glm::vec4(point3, 1));
		point4 = glm::vec3(input * glm::vec4(point4, 1));
		point5 = glm::vec3(input * glm::vec4(point5, 1));
		point6 = glm::vec3(input * glm::vec4(point6, 1));
		point7 = glm::vec3(input * glm::vec4(point7, 1));
		point8 = glm::vec3(input * glm::vec4(point8, 1));

		point1 = glm::vec3(glm::mat4x4(scale) * glm::vec4(point1, 1));
		point2 = glm::vec3(glm::mat4x4(scale) * glm::vec4(point2, 1));
		point3 = glm::vec3(glm::mat4x4(scale) * glm::vec4(point3, 1));
		point4 = glm::vec3(glm::mat4x4(scale) * glm::vec4(point4, 1));
		point5 = glm::vec3(glm::mat4x4(scale) * glm::vec4(point5, 1));
		point6 = glm::vec3(glm::mat4x4(scale) * glm::vec4(point6, 1));
		point7 = glm::vec3(glm::mat4x4(scale) * glm::vec4(point7, 1));
		point8 = glm::vec3(glm::mat4x4(scale) * glm::vec4(point8, 1));

		point1.x += viewport_width / 2;
		point1.y += viewport_height / 2;
		point2.x += viewport_width / 2;
		point2.y += viewport_height / 2;
		point3.x += viewport_width / 2;
		point3.y += viewport_height / 2;
		point4.x += viewport_width / 2;
		point4.y += viewport_height / 2;
		point5.x += viewport_width / 2;
		point5.y += viewport_height / 2;
		point6.x += viewport_width / 2;
		point6.y += viewport_height / 2;
		point7.x += viewport_width / 2;
		point7.y += viewport_height / 2;
		point8.x += viewport_width / 2;
		point8.y += viewport_height / 2;



		glm::vec3 yellow1 = glm::vec3(1.0f, 1.0f, 0.0f);
		glm::vec3 lightblue = glm::vec3(0.0f, 1.0f, 1.0f);
		glm::vec3 pink = glm::vec3(1.0f, 0.0f, 1.0f);
		glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);



		DrawLine(glm::vec2(point2), glm::vec2(point4), green);
		DrawLine(glm::vec2(point1), glm::vec2(point2), green);
		DrawLine(glm::vec2(point4), glm::vec2(point3), green);
		DrawLine(glm::vec2(point3), glm::vec2(point1), green);

		DrawLine(glm::vec2(point4), glm::vec2(point8), green);
		DrawLine(glm::vec2(point8), glm::vec2(point7), green);
		DrawLine(glm::vec2(point7), glm::vec2(point3), green);
		DrawLine(glm::vec2(point3), glm::vec2(point4), green);

		DrawLine(glm::vec2(point3), glm::vec2(point7), green);
		DrawLine(glm::vec2(point7), glm::vec2(point5), green);
		DrawLine(glm::vec2(point5), glm::vec2(point1), green);
		DrawLine(glm::vec2(point1), glm::vec2(point3), green);

		DrawLine(glm::vec2(point6), glm::vec2(point2), green);
		DrawLine(glm::vec2(point6), glm::vec2(point8), green);
		DrawLine(glm::vec2(point6), glm::vec2(point5), green);



	}
}
void Renderer::DrawNormals(MeshModel Obj, glm::mat4x4 input, int Vector_Length, glm::vec3 color)
{
	//unnesscary initalizer of 0.0f,0.0f,0.0f for every Vector3D
	glm::vec3 Normal1, Normal2, Normal3;
	glm::vec3 start1, start2, start3;
	glm::vec3 end1, end2, end3;
	if (Obj.NormalStatus)
	{
		for (int i = 0; i < Obj.GetFacesCount(); i++)
		{
			start1 = Obj.GetVertex(i, 0);
			start2 = Obj.GetVertex(i, 1);
			start3 = Obj.GetVertex(i, 2);

			start1 = input * glm::vec4(start1, 1);
			start2 = input * glm::vec4(start2, 1);
			start3 = input * glm::vec4(start3, 1);

			start1 *= scale, start2 *= scale, start3 *= scale;

			start1.x += viewport_width / 2; start1.y += viewport_height / 2;
			start2.x += viewport_width / 2; start2.y += viewport_height / 2;
			start3.x += viewport_width / 2; start3.y += viewport_height / 2;


			Normal1 = Obj.GetNormal(i, 0);
			Normal2 = Obj.GetNormal(i, 1);
			Normal3 = Obj.GetNormal(i, 2);

			end1 = start1 + glm::vec3(Vector_Length * Normal1.x, Vector_Length * Normal1.y, Vector_Length * Normal1.z);
			end2 = start2 + glm::vec3(Vector_Length * Normal2.x, Vector_Length * Normal2.y, Vector_Length * Normal2.z);
			end3 = start3 + glm::vec3(Vector_Length * Normal3.x, Vector_Length * Normal3.y, Vector_Length * Normal3.z);

			Renderer::DrawLine(start1, end1, color);
			Renderer::DrawLine(start2, end2, color);
			Renderer::DrawLine(start3, end3, color);

		}
	}

}
void Renderer::DrawNormalsPerFaces(MeshModel Obj, glm::mat4x4 input, int Vector_Length, glm::vec3 color)
{
	glm::vec3 start1, start2, start3;
	glm::vec3 end;
	glm::vec3 center_point_of_face;

	if (Obj.NormalStatusPerFaces)
	{
		for (int i = 0; i < Obj.GetFacesCount(); i++)
		{
			start1 = Obj.GetVertex(i, 0);
			start2 = Obj.GetVertex(i, 1);
			start3 = Obj.GetVertex(i, 2);


			start1 = input * glm::vec4(start1, 1);
			start1 *= scale;
			start1.x += viewport_width / 2; start1.y += viewport_height / 2;
			start2 = input * glm::vec4(start2, 1);
			start2 *= scale;
			start2.x += viewport_width / 2; start2.y += viewport_height / 2;
			start3 = input * glm::vec4(start3, 1);
			start3 *= scale;
			start3.x += viewport_width / 2; start3.y += viewport_height / 2;


			glm::vec3 face_normal = glm::triangleNormal(start1, start2, start3);


			center_point_of_face.x = (start1.x + start2.x + start3.x) / 3;
			center_point_of_face.y = (start1.y + start2.y + start3.y) / 3;
			center_point_of_face.z = (start1.z + start2.z + start3.z) / 3;

			end = center_point_of_face + glm::vec3(Vector_Length * face_normal.x, Vector_Length * face_normal.y, Vector_Length * face_normal.z);

			DrawLine(center_point_of_face, end, color);

		}
	}
}
void Renderer::DrawTriangleBoxs(MeshModel Obj, glm::mat4x4 input)
{
	glm::vec3 point1, point2, point3, point4, point5, point6, point7, point8;
	//glm::vec3 triangle_center;
	glm::vec3 tringale_color;
	float min_x, max_x, min_y, max_y, min_z, max_z;

	if (Obj.VerticesColorful)
	{
		for (int i = 0; i < Obj.GetFacesCount(); i++)
		{
			point1 = Obj.GetVertex(i, 0);
			point2 = Obj.GetVertex(i, 1);
			point3 = Obj.GetVertex(i, 2);


			point1 = input * glm::vec4(point1, 1);
			point1 *= scale;
			point1.x += viewport_width / 2; point1.y += viewport_height / 2;
			point2 = input * glm::vec4(point2, 1);
			point2 *= scale;
			point2.x += viewport_width / 2; point2.y += viewport_height / 2;
			point3 = input * glm::vec4(point3, 1);
			point3 *= scale;
			point3.x += viewport_width / 2; point3.y += viewport_height / 2;



			float R_Color = (rand() % (100));
			float G_Color = (rand() % (100));
			float B_Color = (rand() % (100));

			R_Color = R_Color / 100;
			G_Color = G_Color / 100;
			B_Color = B_Color / 100;

			tringale_color = glm::vec3(R_Color, G_Color, B_Color);


			min_x = std::min(point1.x, std::min(point2.x, point3.x));
			max_x = std::max(point1.x, std::max(point2.x, point3.x));
			min_y = std::min(point1.y, std::min(point2.y, point3.y));
			max_y = std::max(point1.y, std::max(point2.y, point3.y));
			min_z = std::min(point1.z, std::min(point2.z, point3.z));
			max_z = std::max(point1.z, std::max(point2.z, point3.z));



			point1 = glm::vec3(min_x, min_y, min_z);
			point2 = glm::vec3(min_x, min_y, max_z);

			point3 = glm::vec3(min_x, max_y, min_z);
			point4 = glm::vec3(min_x, max_y, max_z);

			point5 = glm::vec3(max_x, min_y, min_z);
			point6 = glm::vec3(max_x, min_y, max_z);

			point7 = glm::vec3(max_x, max_y, min_z);
			point8 = glm::vec3(max_x, max_y, max_z);



			DrawLine(glm::vec2(point2), glm::vec2(point4), tringale_color);
			DrawLine(glm::vec2(point1), glm::vec2(point2), tringale_color);
			DrawLine(glm::vec2(point4), glm::vec2(point3), tringale_color);
			DrawLine(glm::vec2(point3), glm::vec2(point1), tringale_color);

			DrawLine(glm::vec2(point4), glm::vec2(point8), tringale_color);
			DrawLine(glm::vec2(point8), glm::vec2(point7), tringale_color);
			DrawLine(glm::vec2(point7), glm::vec2(point3), tringale_color);
			DrawLine(glm::vec2(point3), glm::vec2(point4), tringale_color);

			DrawLine(glm::vec2(point3), glm::vec2(point7), tringale_color);
			DrawLine(glm::vec2(point7), glm::vec2(point5), tringale_color);
			DrawLine(glm::vec2(point5), glm::vec2(point1), tringale_color);
			DrawLine(glm::vec2(point1), glm::vec2(point3), tringale_color);

			DrawLine(glm::vec2(point6), glm::vec2(point2), tringale_color);
			DrawLine(glm::vec2(point6), glm::vec2(point8), tringale_color);
			DrawLine(glm::vec2(point6), glm::vec2(point5), tringale_color);

		}
		/*	triangle_center.x = (point1.x + point2.x + point3.x) / 3;
			triangle_center.y = (point1.y + point2.y + point3.y) / 3;
			triangle_center.z = (point1.z + point2.z + point3.z) / 3;
			HDC test;
			FloodFill(test, triangle_center.x, triangle_center.y, tringale_color);
			*/



	}



}
void Renderer::SubDivision(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, int subdivision, glm::ivec3 Color)
{
	if (subdivision == 1)
		return;
	else
	{
		glm::vec3 middle_p1_p2 = glm::vec3((point1.x + point2.x) / 2, (point1.y + point2.y) / 2, (point1.z + point2.z) / 2);
		glm::vec3 middle_p1_p3 = glm::vec3((point1.x + point3.x) / 2, (point1.y + point3.y) / 2, (point1.z + point3.z) / 2);
		glm::vec3 middle_p2_p3 = glm::vec3((point2.x + point3.x) / 2, (point2.y + point3.y) / 2, (point2.z + point3.z) / 2);
		{
			DrawLine(middle_p1_p2, middle_p1_p3, Color);
			DrawLine(middle_p1_p2, middle_p2_p3, Color);
			DrawLine(middle_p2_p3, middle_p1_p3, Color);
		}
		SubDivision(point1, middle_p1_p3, middle_p1_p2, subdivision - 1, Color);
		SubDivision(point2, middle_p1_p2, middle_p2_p3, subdivision - 1, Color);
		SubDivision(point3, middle_p2_p3, middle_p1_p3, subdivision - 1, Color);
		SubDivision(middle_p1_p2, middle_p2_p3, middle_p1_p3, subdivision - 1, Color);

	}
}

//Drawing Light Object
void Renderer::DrawLight(Light& input, Camera& Cam)
{
	if (input.Status)
	{
		if (input.LightRound)
		{
			glm::vec3 new_position = glm::vec3(input.TotalTransformation * glm::vec4(input.Position, 1));

			float min_x = new_position.x - input.Radius;
			float max_x = new_position.x + input.Radius;

			float min_y = new_position.y - input.Radius;
			float max_y = new_position.y + input.Radius;

			for (float j = min_y; j <= max_y; j++)
			{
				for (float i = min_x; i <= max_x; i++)
				{
					if (IsRound(i, j, new_position, input.Radius))
					{
						PutPixel(i, j, input.LightColor);
					}
				}
			}
		}
		else
			if (input.LightLine)
			{
				float incline = (input.LinePosition1.y - input.LinePosition2.y) / (input.LinePosition1.x - input.LinePosition2.x);

				if (incline != 0 && incline != INFINITY)
				{
					for (int i = 0; i < LineDepth; i++)
					{
						DrawLine(input.LinePosition1 + glm::vec3(i, i, 0), input.LinePosition2 + glm::vec3(i, i, 0), input.LightColor);
					}
				}
				if (incline == INFINITY)
				{
					for (int i = 0; i < LineDepth; i++)
					{
						DrawLine(input.LinePosition1 + glm::vec3(i, 0, 0), input.LinePosition2 + glm::vec3(i, 0, 0), input.LightColor);
					}
				}
				if (incline == 0)
				{
					for (int i = 0; i < LineDepth; i++)
					{
						DrawLine(input.LinePosition1 + glm::vec3(0, i, 0), input.LinePosition2 + glm::vec3(0, i, 0), input.LightColor);
					}
				}
			}
			else 
			if(input.LightRectangle)
			{
				glm::vec3 new_position = glm::vec3(input.TotalTransformation * glm::vec4(input.Position, 1));

				float min_x = new_position.x - input.Radius;
				float max_x = new_position.x + input.Radius;

				float min_y = new_position.y - input.Radius;
				float max_y = new_position.y + input.Radius;

				for (float j = min_y; j <= max_y; j++)
				{
					for (float i = min_x; i <= max_x; i++)
					{
						PutPixel(i, j, input.LightColor);
					}
				}
			}
	}

}
void Renderer::Adjust_LightPosition(Light& input)
{
	input.Position = glm::vec3(viewport_width / 2, viewport_height / 2, 0);
}


//Grounand Shading
void Renderer::FillTringles_Grouand(glm::vec3 start_point, glm::vec3 end_point, glm::vec3  middle_point, glm::vec3 Normal1_Color, glm::vec3  Normal_2_Color, glm::vec3 Normal_3_Color)
{
	float min_x, max_x;
	min_x = std::min(start_point.x, std::min(middle_point.x, end_point.x));
	max_x = std::max(start_point.x, std::max(middle_point.x, end_point.x));
	float min_y, max_y;
	min_y = std::min(start_point.y, std::min(middle_point.y, end_point.y));
	max_y = std::max(start_point.y, std::max(middle_point.y, end_point.y));



	for (int j = max_y; j >= min_y; j--)
	{
		for (int i = min_x; i < max_x; i++)
		{
			if (PointInTriangle(glm::vec2(i, j), start_point, middle_point, end_point) && InWindow(i, j))
			{
				glm::vec3 pixel_color = LightInterpolation(i, j, start_point, middle_point, end_point, Normal1_Color, Normal_2_Color, Normal_3_Color);
				PutPixel(i, j, pixel_color);
			}
		}
	}
}

glm::vec3 Renderer::LightInterpolation(float x, float y, glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 Color1, glm::vec3 Color2, glm::vec3 Color3)
{
	float area_12 = area(glm::vec2(x, y), point1, point2);
	float area_13 = area(glm::vec2(x, y), point1, point3);
	float area_23 = area(glm::vec2(x, y), point2, point3);

	float total_area = area(point1, point2, point3);

	return((area_23 / total_area) * Color1 + (area_13 / total_area) * Color2 + (area_12 / total_area) * Color3);
}

glm::vec3 ColorVector(glm::vec3 input)
{
	if (input.x > 1)
	{
		input.x = 1;
	}
	else if (input.x < 0)
	{
		input.x = 0;
	}

	if (input.y > 1)
	{		  
		input.y = 1;
	}
	else if (input.y < 0)
	{
		input.y = 0;
	}

	if (input.z > 1)
	{
		input.z = 1;
	}
	else if (input.z < 0)
	{
		input.z = 0;
	}
	return input;
}

glm::vec3 Renderer::NormalInterpolation(float x, float y, glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, glm::vec3 Normal1, glm::vec3 Normal2, glm::vec3 Normal3)
{
	float area_12 = area(glm::vec2(x, y), point1, point2);
	float area_13 = area(glm::vec2(x, y), point1, point3);
	float area_23 = area(glm::vec2(x, y), point2, point3);

	float total_area = area(point1, point2, point3);

	return((area_23 / total_area) * Normal1 + (area_13 / total_area) * Normal2 + (area_12 / total_area) * Normal3);
}

bool Renderer::FaceCuling(glm::mat4x4 input ,glm::vec3 start_point, glm::vec3 end_point, glm::vec3  middle_point , Camera& Camerainput)
{
	glm::vec3 Face_Normal = glm::triangleNormal(start_point, middle_point, end_point);
	Face_Normal = glm::vec3(input * glm::vec4(Face_Normal, 1));
	Face_Normal = glm::normalize(Face_Normal);

	//glm::vec3 Camera_Normal_Vector = Camerainput.Position - Face_Normal;
	if (glm::dot(Face_Normal,Camerainput.Orientation) < 0)
	{
		return true;
	}
	return false;
}

float DistanceOfLine(glm::vec3 point1, glm::vec3 point2, glm::vec3 input)
{
	float dy = point2.y - point1.y, dx = point2.x - point1.x;
	float incline = dy / dx;
	float offset = point1.y - incline * point1.x;

	return (abs(dy * input.x - dx * input.y - dy * point1.x + dx * point1.x + dx * point1.y) / sqrt(dx * dx + dy * dy));
}

float BlurPixel(float* arr, int col, int row, int k, int width, int height, Scene& Scene)
{
	float sum = 0;
	float sumKernel = 0;

	for (int j = -1; j <= 1; j++)
	{
		for (int i = -1; i <= 1; i++)
		{
			if ((row + j) >= 0 && (row + j) < height && (col + i) >= 0 && (col + i) < width)
			{
				float color = arr[(row + j) * 3 * width + (col + i) * 3 + k];
				sum += color * Scene.KernelMatrix[i + 1][j + 1];
				sumKernel += Scene.KernelMatrix[i + 1][j + 1];
			}
		}
	}
	return sum / sumKernel;
}

void Renderer::ActiveBlurEffect(float* arr, float* result, int width, int height, Scene& scene)
{
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			for (int k = 0; k < 3; k++)
			{
				result[3 * row * width + 3 * col + k] = BlurPixel(arr, col, row, k, width, height,scene);
			}
		}
	}
}

void Renderer::ActiveFogEffect(float* arr, float* result, int width, int height, Scene& scene)
{
	for (int row = 0; row < height; row++)
	{
		float attenuation = 1 / (Distance(scene.GetCamera(scene.camerainput).Position.x, row, 0, scene.GetCamera(scene.camerainput).Position.x, scene.GetCamera(scene.camerainput).Position.y, 0) + 1);
		for (int col = 0; col < width; col++)
		{
			for (int k = 0; k < 3; k++)
			{
				result[3 * row * width + 3 * col + k] = result[3 * row * width + 3 * col + k] * (1 - ModifyIntensity(attenuation * scene.FogFactor)) + ModifyIntensity(attenuation * scene.FogFactor);
			}
		}	
	}
}

float ModifyIntensity(float input)
{
	if (input < 1)
	{
		return 1.0f;
	}
	else
		if (input < 0)
		{
			return 0.0f;
		}
		else
			return input;
}