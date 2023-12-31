#define _USE_MATH_DEFINES
#include <cmath>
#include <imgui/imgui.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nfd.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
//#include "Light.h"
#include "Renderer.h"
#include "Scene.h"
#include "Utils.h"

/**
 * Fields
 */
bool show_demo_window = false;
bool show_another_window = false;

bool MeshLocal_window = false;
bool MeshWorld_window = false;
bool Extensions = false;
bool MeshModel_Effects = false;

bool Camera_Settings = false;
bool initial_settings = false;
bool transformation_settings = false;

bool Lights_Controls = false;
bool Lights_Transformation = false;

bool statics_window = false;

bool BackGround_Settings = false;

//initializer of Mesh Model & Camera Index
static int ModelNumber = -1;
static int Camera_Number = 0;
static int Light_Number = -1;
//glm::vec4 Background_Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.00f);
glm::vec4 Background_Color = glm::vec4(0.2f, 0.2f, 0.2f, 1.00f);
glm::vec4 MeshModel_Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.00f);
glm::vec4 MeshModel_Normals = glm::vec4(0.0f, 0.0f, 0.0f, 1.00f);

void PrintMatrix(glm::mat4x4 input)
{
	for (int i = 0; i < 4; i++)
	{
		ImGui::Text("%f  %f  %f  %f", input[i][0], input[i][1], input[i][2], input[i][3]);
	}
}

/**
 * Function declarations
 */
static void GlfwErrorCallback(int error, const char* description);
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupDearImgui(GLFWwindow* window);
void StartFrame();
void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io);
void Cleanup(GLFWwindow* window);
void DrawImguiMenus(ImGuiIO& io, Scene& scene);
void PrintMatrix(glm::mat4x4 input);
/**
 * Function implementation
 */
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	// TODO: Handle mouse scroll here
}

int main(int argc, char **argv)
{
	int windowWidth = 1920, windowHeight = 1280;
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, "Mesh Viewer");
	if (!window)
		return 1;

	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

	Renderer renderer = Renderer(frameBufferWidth, frameBufferHeight);
	Scene scene = Scene();
	Camera camera = Camera();
	std::shared_ptr<Camera> cameras = std::make_shared<Camera>(camera);
	scene.AddCamera(cameras);
	camera.aspect = (windowWidth / windowHeight);
	ImGuiIO& io = SetupDearImgui(window);
	glfwSetScrollCallback(window, ScrollCallback);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
		StartFrame();
		DrawImguiMenus(io, scene);
		RenderFrame(window, scene, renderer, io);
    }

	Cleanup(window);
    return 0;
}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name)
{
	glfwSetErrorCallback(GlfwErrorCallback);
	if (!glfwInit())
		return NULL;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	#if __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	
	GLFWwindow* window = glfwCreateWindow(w, h, window_name, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
						 // very importent!! initialization of glad
						 // https://stackoverflow.com/questions/48582444/imgui-with-the-glad-opengl-loader-throws-segmentation-fault-core-dumped

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	return window;
}

ImGuiIO& SetupDearImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
	return io;
}

void StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io)
{
	ImGui::Render();
	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
	
	if (frameBufferWidth != renderer.GetViewportWidth() || frameBufferHeight != renderer.GetViewportHeight())
	{
		// TODO: Set new aspect ratio
		Renderer main_renderer = Renderer(frameBufferWidth, frameBufferHeight);
	}
	
	if(scene.meshmodelinput < scene.GetModelCount() && scene.meshmodelinput >=0)
	{
		scene.GetModel(scene.meshmodelinput).ResetCurrentMatrix();
		scene.GetModel(scene.meshmodelinput).ApplyMatrix();
		if (!io.WantCaptureKeyboard)
		{
			glm::mat4x4 temporary;
			// TODO: Handle keyboard events here
			if (io.KeysDown[65])
			{
				temporary = scene.GetModel(scene.meshmodelinput).Get_TranslateMeshModelMatrix();
				temporary[3][0] -= 0.1;
				scene.GetModel(scene.meshmodelinput).Set_TranslateMeshModelMatrix(temporary);
				scene.GetActiveModel().ApplyMatrix();

				// A key is right
				// Use the ASCII table for more key codes (https://www.asciitable.com/)
			}
			if (io.KeysDown[68])
			{
				temporary = scene.GetModel(scene.meshmodelinput).Get_TranslateMeshModelMatrix();
				temporary[3][0] += 0.1;
				scene.GetModel(scene.meshmodelinput).Set_TranslateMeshModelMatrix(temporary);
				scene.GetModel(scene.meshmodelinput).ApplyMatrix();

				// D key is left
				// Use the ASCII table for more key codes (https://www.asciitable.com/)
			}
			if (io.KeysDown[83])
			{
				temporary = scene.GetModel(scene.meshmodelinput).Get_TranslateMeshModelMatrix();
				temporary[3][1] -= 0.1;
				scene.GetModel(scene.meshmodelinput).Set_TranslateMeshModelMatrix(temporary);
				scene.GetModel(scene.meshmodelinput).ApplyMatrix();

				// S key is down
				// Use the ASCII table for more key codes (https://www.asciitable.com/)
			}
			if (io.KeysDown[87])
			{
				temporary = scene.GetModel(scene.meshmodelinput).Get_TranslateMeshModelMatrix();
				temporary[3][1] += 0.1;
				scene.GetModel(scene.meshmodelinput).Set_TranslateMeshModelMatrix(temporary);
				scene.GetModel(scene.meshmodelinput).ApplyMatrix();
				
				// W key is up
				// Use the ASCII table for more key codes (https://www.asciitable.com/)
			}
			if (io.KeysDown[120])
			{
				scene.GetModel(scene.meshmodelinput).Set_AlphaXMeshModelMatrix(scene.GetModel(scene.meshmodelinput).Get_AlphaXMeshModelMatrix() + 1);
				scene.GetModel(scene.meshmodelinput).RotationX();
				scene.GetModel(scene.meshmodelinput).CalculateRotation();
				scene.GetActiveModel().ApplyMatrix();

				// X kEY is rotation across x - increasing
				// Use the ASCII table for more key codes (https://www.asciitable.com/)
			}
			if (io.KeysDown[88])
			{
				scene.GetModel(scene.meshmodelinput).Set_AlphaXMeshModelMatrix(scene.GetModel(scene.meshmodelinput).Get_AlphaXMeshModelMatrix() - 1);
				scene.GetModel(scene.meshmodelinput).RotationX();
				scene.GetModel(scene.meshmodelinput).CalculateRotation();
				scene.GetActiveModel().ApplyMatrix();

				// x kEY is rotation across x - decreasing
				// Use the ASCII table for more key codes (https://www.asciitable.com/)
			}
			if (io.KeysDown[121])
			{
				scene.GetModel(scene.meshmodelinput).Set_AlphaYMeshModelMatrix(scene.GetModel(scene.meshmodelinput).Get_AlphaYMeshModelMatrix() + 1);
				scene.GetModel(scene.meshmodelinput).RotationY();
				scene.GetModel(scene.meshmodelinput).CalculateRotation();
				scene.GetActiveModel().ApplyMatrix();

				// Y kEY is rotation across y - increasing
				// Use the ASCII table for more key codes (https://www.asciitable.com/)
			}
			if (io.KeysDown[89])
			{
				scene.GetModel(scene.meshmodelinput).Set_AlphaYMeshModelMatrix(scene.GetModel(scene.meshmodelinput).Get_AlphaYMeshModelMatrix() - 1);
				scene.GetModel(scene.meshmodelinput).RotationY();
				scene.GetModel(scene.meshmodelinput).CalculateRotation();
				scene.GetActiveModel().ApplyMatrix();

				// y kEY is rotation across y - decreasing
				// Use the ASCII table for more key codes (https://www.asciitable.com/)
			}
			if (io.KeysDown[122])
			{
				scene.GetModel(scene.meshmodelinput).Set_AlphaZMeshModelMatrix(scene.GetModel(scene.meshmodelinput).Get_AlphaZMeshModelMatrix() + 1);
				scene.GetModel(scene.meshmodelinput).RotationZ();
				scene.GetModel(scene.meshmodelinput).CalculateRotation();
				scene.GetActiveModel().ApplyMatrix();

				// Z kEY is rotation across z - increasing
				// Use the ASCII table for more key codes (https://www.asciitable.com/)
			}
			if (io.KeysDown[90])
			{
				scene.GetModel(scene.meshmodelinput).Set_AlphaZMeshModelMatrix(scene.GetModel(scene.meshmodelinput).Get_AlphaZMeshModelMatrix() - 1);
				scene.GetModel(scene.meshmodelinput).RotationZ();
				scene.GetModel(scene.meshmodelinput).CalculateRotation();
				scene.GetActiveModel().ApplyMatrix();

				// y kEY is rotation across z - decreasing
				// Use the ASCII table for more key codes (https://www.asciitable.com/)
			}

		}
	}

	if (scene.GetCameraCount())
	{
		//Left Arrow
		if (io.KeysDown[37])
		{
			scene.GetCamera(Camera_Number).Position.x -= 0.1;
			scene.GetCamera(Camera_Number).SetCameraLookAt();
		}
		//Up Arrow
		if (io.KeysDown[38])
		{
			scene.GetCamera(Camera_Number).Position.y += 0.1;
			scene.GetCamera(Camera_Number).SetCameraLookAt();

		}
		//Right Arrow
		if (io.KeysDown[39])
		{
			scene.GetCamera(Camera_Number).Position.x += 0.1;
			scene.GetCamera(Camera_Number).SetCameraLookAt();

		}
		//Left Arrow
		if (io.KeysDown[40])
		{
			scene.GetCamera(Camera_Number).Position.y -= 0.1;
			scene.GetCamera(Camera_Number).SetCameraLookAt();
		}
	}
	if (io.MouseDown[0])
	{
		
	}

	if (!io.WantCaptureMouse)
	{
		// TODO: Handle mouse events here
		
	}

	renderer.ClearColorBuffer(Background_Color);
	renderer.Render(scene);
	renderer.SwapBuffers();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwMakeContextCurrent(window);
	glfwSwapBuffers(window);
}

void Cleanup(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void DrawImguiMenus(ImGuiIO& io, Scene& scene)
{
	// Top Menu Bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				nfdchar_t* outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("obj;", NULL, &outPath);
				if (result == NFD_OKAY)
				{
					scene.AddModel(Utils::LoadMeshModel(outPath));
					free(outPath);
					ModelNumber++;
				}
				else if (result == NFD_CANCEL)
				{

				}
				else
				{
				}

			}
			ImGui::EndMenu();
		}
		// TODO: Add more menubar items (if you want to)
		if (ImGui::BeginMenu("Transformation"))
		{
			if (ImGui::MenuItem("Local Transformation"))
			{
				MeshLocal_window = true;
			}
			if (ImGui::MenuItem("World Transformation"))
			{
				MeshWorld_window = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Camera Settings"))
		{
			if (ImGui::MenuItem("Add Camera", "CTRL+ R"))
			{
				Camera new_cam = Camera();
				std::shared_ptr<Camera> cameras = std::make_shared<Camera>(new_cam);
				scene.AddCamera(cameras);
				Camera_Number++;
			}
			if (ImGui::MenuItem("Default Settings"))
			{
				initial_settings = true;
			}

			if (ImGui::MenuItem("Camera Transformation"))
			{
				transformation_settings = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Mesh Model Settings"))
		{
			if (ImGui::MenuItem("Extensions"))
			{
				Extensions = true;
			}
			if (ImGui::MenuItem("Effects"))
			{
				MeshModel_Effects = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Lighting & Shading"))
		{
			if (ImGui::MenuItem("Add Light"))
			{
				//creating a new default Light Source
				Light new_Light = Light();
				std::shared_ptr<Light> lights = std::make_shared<Light>(new_Light);
				scene.AddLight(lights);
				Light_Number++;
			}
			if (ImGui::MenuItem("Lighting & Shading Controls"))
			{
				Lights_Controls = true;
			}
			if (ImGui::MenuItem("Lighting & Shading Transformation"))
			{
				Lights_Transformation = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("BackGround Options"))
		{
			BackGround_Settings = true;
		}
		if (ImGui::MenuItem("Statics"))
		{
			statics_window = true;
		}
		if (ImGui::MenuItem("Help"))
		{
			show_demo_window = true;
		}

		ImGui::EndMainMenuBar();
	}

	//___________________________________________________________________________________________
	//Main Menu :
	ImGui::Begin("Main Menu");

	ImGui::Text("Main Control Panel Of Project");
	if (ImGui::InputInt("Mesh Model Index :", &ModelNumber, 1, 10, 1))
	{
		scene.meshmodelinput = ModelNumber;
	}
	if (ImGui::Button("Turn Off"))
	{
		if (ModelNumber != -1 && ModelNumber < scene.GetModelCount())
			scene.GetModel(ModelNumber).Set_Status(false);
	}
	ImGui::SameLine();
	if (ImGui::Button("Turn On"))
	{
		if (ModelNumber != -1 && ModelNumber < scene.GetModelCount())
			scene.GetModel(ModelNumber).Set_Status(true);
	}
	ImGui::NewLine();
	ImGui::InputInt("Camera Index : ", &Camera_Number, 1, 10, 1);
	if (ImGui::Button("Turn Off"))
	{
		if (Camera_Number != -1 && Camera_Number < scene.GetModelCount())
			scene.GetCamera(Camera_Number).SetStatus(false);
	}
	ImGui::SameLine();
	if (ImGui::Button("Turn On"))
	{
		if (Camera_Number != -1 && Camera_Number < scene.GetModelCount())
			scene.GetCamera(Camera_Number).SetStatus(true);
	}
	ImGui::NewLine();
	if(ImGui::InputInt("Light Index :", &Light_Number, 1, 10, 1))
	{
		scene.Lightinput = Light_Number;
	}
	if (ImGui::Checkbox("Blur Scene", &scene.BlurScene))
	{
	}
	if (ImGui::Checkbox("Fog Scene", &scene.FogScene))
	{
	}
	if (scene.FogScene)
	{
		ImGui::SliderFloat("Fog Factor =", &scene.FogFactor, 0.0f, 1.0f);
	}
	/*ImGui::Text("Mesh Models Number are %d", scene.GetModelCount());
	ImGui::Text("Cameras Number are %d", scene.GetCameraCount());
	ImGui::Text("Active Mesh Model Number is %d", scene.GetActiveModelIndex());
	ImGui::Text("Active Camera Number is %d", scene.GetActiveCameraIndex());
	ImGui::Text("Active Light Number is %d", scene.GetActiveLightIndex());
	ImGui::Text("Lights Number is %d", scene.GetLightsCount());*/
	ImGui::End();
	//Mesh Model Effects
	//___________________________________________________________________________________________
	
	//Camera Settings 
	if (Camera_Number >= 0 && Camera_Number < scene.GetCameraCount() && scene.GetCamera(Camera_Number).GetStatus())
	{
		scene.camerainput = Camera_Number;

		if (initial_settings)
		{
			static float distance = 0;
			ImGui::Begin("Camera initial settings");
			ImGui::Text("Camera Number %d", Camera_Number);
			ImGui::NewLine();
			ImGui::Text("Position :", Camera_Number);

			if (ImGui::SliderFloat("Position X =", &scene.GetCamera(Camera_Number).Position.x, -500,500))
			{
				scene.GetCamera(Camera_Number).SetCameraLookAt();
			}
			if (ImGui::SliderFloat("Position Y =", &scene.GetCamera(Camera_Number).Position.y, -500,500))
			{
				scene.GetCamera(Camera_Number).SetCameraLookAt();
			}
			if (ImGui::SliderFloat("Position Z =", &scene.GetCamera(Camera_Number).Position.z, -500,500))
			{
				scene.GetCamera(Camera_Number).SetCameraLookAt();
			}
			
			ImGui::NewLine();
			ImGui::Text("Orientation :");
			if (ImGui::SliderFloat("Orientation X =", &scene.GetCamera(Camera_Number).Orientation.x, -500, 500))
			{
				scene.GetCamera(Camera_Number).SetCameraLookAt(scene.GetCamera(Camera_Number).Position, scene.GetCamera(Camera_Number).Orientation, scene.GetCamera(Camera_Number).Up_vector);
			}
			if (ImGui::SliderFloat("Orientation Y =", &scene.GetCamera(Camera_Number).Orientation.y, -500, 500))
			{
				scene.GetCamera(Camera_Number).SetCameraLookAt(scene.GetCamera(Camera_Number).Position, scene.GetCamera(Camera_Number).Orientation, scene.GetCamera(Camera_Number).Up_vector);
			}
			if (ImGui::SliderFloat("Orientation Z =", &scene.GetCamera(Camera_Number).Orientation.z, -500, 500))
			{
				scene.GetCamera(Camera_Number).SetCameraLookAt(scene.GetCamera(Camera_Number).Position, scene.GetCamera(Camera_Number).Orientation, scene.GetCamera(Camera_Number).Up_vector);
			}

			ImGui::NewLine();


			if (ImGui::Button("Reset Values"))
			{
				scene.GetCamera(Camera_Number).Position.x	 = 0;
				scene.GetCamera(Camera_Number).Position.y	 = 0;
				scene.GetCamera(Camera_Number).Position.z	 = -1;
				scene.GetCamera(Camera_Number).Orientation.x= 0;
				scene.GetCamera(Camera_Number).Orientation.y= 0;
				scene.GetCamera(Camera_Number).Orientation.z= 0;
				scene.GetCamera(Camera_Number).SetCameraLookAt();

			}

			ImGui::SameLine();
			if (ImGui::Button("Close"))
			{
				initial_settings = false;
			}
			ImGui::End();
		}
		if (transformation_settings)
		{
			ImGui::Begin("Camera Transformation Settings");
			static bool local = false, world = false, OrthogonalView = false, PerspectiveView = false;
			static bool Position_Adjust = false;

			if (ImGui::Checkbox("Local Transformation", &local))
			{
			}
			if (ImGui::Checkbox("World Transformation", &world))
			{
			}
			if (ImGui::Checkbox("Orthogonal View", &OrthogonalView))
			{
			}
			if (ImGui::Checkbox("Perspective View", &PerspectiveView))
			{

			}

			Camera_Number = scene.camerainput;
			if (scene.GetCameraCount() > 0 && Camera_Number > -1)
			{
				if (local)
				{
					glm::mat4x4 CameraTranslation = scene.GetActiveCamera().GetTranslation();

					//translation and rotations of camera indivually - no effect from world rotation.
					if (ImGui::SliderFloat("Translate X", &CameraTranslation[3][0], -10, 10))
					{
						scene.GetCamera(Camera_Number).SetTranslation(CameraTranslation);
						scene.GetCamera(Camera_Number).CalculateTotalTransform();
					}
					if (ImGui::SliderFloat("Translate Y", &CameraTranslation[3][1], -10, 10))
					{
						scene.GetCamera(Camera_Number).SetTranslation(CameraTranslation);
						scene.GetCamera(Camera_Number).CalculateTotalTransform();
					}
					if (ImGui::SliderFloat("Translate Z", &CameraTranslation[3][2], -10, 10))
					{
						scene.GetCamera(Camera_Number).SetTranslation(CameraTranslation);
						scene.GetCamera(Camera_Number).CalculateTotalTransform();
					}
					if (ImGui::SliderFloat("Rotate X", &scene.GetCamera(Camera_Number).GetAlphaX(), -360, 360))
					{
						scene.GetCamera(Camera_Number).SetAlphaX(scene.GetCamera(Camera_Number).GetAlphaX());
						scene.GetCamera(Camera_Number).RotationX();
						scene.GetCamera(Camera_Number).CalculateTotalRotation();
						scene.GetCamera(Camera_Number).CalculateTotalTransform();
					}
					if (ImGui::SliderFloat("Rotate Y", &scene.GetCamera(Camera_Number).GetAlphaY(), -360, 360))
					{
						scene.GetCamera(Camera_Number).SetAlphaY(scene.GetCamera(Camera_Number).GetAlphaY());
						scene.GetCamera(Camera_Number).RotationY();
						scene.GetCamera(Camera_Number).CalculateTotalRotation();
						scene.GetCamera(Camera_Number).CalculateTotalTransform();
					}
					if (ImGui::SliderFloat("Rotate Z", &scene.GetCamera(Camera_Number).GetAlphaZ(), -360, 360))
					{
						scene.GetCamera(Camera_Number).SetAlphaZ(scene.GetCamera(Camera_Number).GetAlphaZ());
						scene.GetCamera(Camera_Number).RotationZ();
						scene.GetCamera(Camera_Number).CalculateTotalRotation();
						scene.GetCamera(Camera_Number).CalculateTotalTransform();
					}
				}

				if (world)
				{
					if (ImGui::SliderFloat("World Translate X", &scene.GetCamera(Camera_Number).World_Translate[3][0], -10, 10))
					{
						scene.GetCamera(Camera_Number).CalculateTotalTransform();
					}
					if (ImGui::SliderFloat("World Translate Y", &scene.GetCamera(Camera_Number).World_Translate[3][1], -10, 10))
					{
						scene.GetCamera(Camera_Number).CalculateTotalTransform();
					}
					if (ImGui::SliderFloat("World Translate Z", &scene.GetCamera(Camera_Number).World_Translate[3][2], -10, 10))
					{
						scene.GetCamera(Camera_Number).CalculateTotalTransform();
					}
					if (ImGui::SliderFloat("World Rotate X", &scene.GetCamera(Camera_Number).World_AlphaX, -360, 360))
					{
						scene.GetCamera(Camera_Number).World_RotationX();
						scene.GetCamera(Camera_Number).CalculateWorldTotalRotation();
						scene.GetCamera(Camera_Number).CalculateTotalTransform();
					}
					if (ImGui::SliderFloat("World Rotate Y", &scene.GetCamera(Camera_Number).World_AlphaY, -360, 360))
					{
						scene.GetCamera(Camera_Number).World_RotationY();
						scene.GetCamera(Camera_Number).CalculateWorldTotalRotation();
						scene.GetCamera(Camera_Number).CalculateTotalTransform();
					}
					if (ImGui::SliderFloat("World Rotate Z", &scene.GetCamera(Camera_Number).World_AlphaZ, -360, 360))
					{
						scene.GetCamera(Camera_Number).World_RotationZ();
						scene.GetCamera(Camera_Number).CalculateWorldTotalRotation();
						scene.GetCamera(Camera_Number).CalculateTotalTransform();
					}
				}

				if (OrthogonalView)
				{
					PerspectiveView = false;
					scene.GetCamera(Camera_Number).Orthograhpic_View = true;
					scene.GetCamera(Camera_Number).Perspective_View = false;

					PrintMatrix(scene.GetCamera(Camera_Number).OrthographicMatrix);


					if (ImGui::SliderFloat("Left", &scene.GetCamera(Camera_Number).left, -10, 10))
					{
						scene.GetCamera(Camera_Number).SetOrthographicView();
					}
					if (ImGui::SliderFloat("Up", &scene.GetCamera(Camera_Number).up, -10, 10))
					{
						scene.GetCamera(Camera_Number).SetOrthographicView();
					}
					if (ImGui::SliderFloat("Right", &scene.GetCamera(Camera_Number).right, -10, 10))
					{
						scene.GetCamera(Camera_Number).SetOrthographicView();
					}
					if (ImGui::SliderFloat("Down", &scene.GetCamera(Camera_Number).down, -10, 10))
					{
						scene.GetCamera(Camera_Number).SetOrthographicView();
					}
					if (ImGui::SliderFloat("Far", &scene.GetCamera(Camera_Number).far_cam, -10, 10))
					{
						scene.GetCamera(Camera_Number).SetOrthographicView();
					}
					if (ImGui::SliderFloat("Near", &scene.GetCamera(Camera_Number).near_cam, -10, 10))
					{
						scene.GetCamera(Camera_Number).SetOrthographicView();
					}
					if (ImGui::Button("Reset Values"))
					{
						scene.GetCamera(Camera_Number).far_cam = -1;
						scene.GetCamera(Camera_Number).near_cam = 1;
						scene.GetCamera(Camera_Number).left = -1;
						scene.GetCamera(Camera_Number).right = 1;
						scene.GetCamera(Camera_Number).up = 1;
						scene.GetCamera(Camera_Number).down = -1;
						scene.GetCamera(Camera_Number).SetFrustumMatrix();
					}
				}

				if (PerspectiveView)
				{
					OrthogonalView = false;
					scene.GetCamera(Camera_Number).Perspective_View = true;
					scene.GetCamera(Camera_Number).Orthograhpic_View = false;

					PrintMatrix(scene.GetCamera(Camera_Number).PerspectiveMatrix);

					if (ImGui::SliderFloat("Far", &scene.GetCamera(Camera_Number).far_cam, -1, 1))
					{
						scene.GetCamera(Camera_Number).SetPerspectiveView();
					}
					if (ImGui::SliderFloat("Near", &scene.GetCamera(Camera_Number).near_cam, -1, 1))
					{
						scene.GetCamera(Camera_Number).SetPerspectiveView();
					}
					if (ImGui::SliderFloat("FOV", &scene.GetCamera(Camera_Number).FOV, -90, 90))
					{
						scene.GetCamera(Camera_Number).SetPerspectiveView();
					}
					if (ImGui::Button("Reset Values"))
					{
						scene.GetCamera(Camera_Number).far_cam = 1;
						scene.GetCamera(Camera_Number).near_cam = -1;
						scene.GetCamera(Camera_Number).FOV = 45;
						scene.GetCamera(Camera_Number).SetPerspectiveView();

					}
				}
				
			}
			else
			{
				ImGui::Text("Add Camera to control over!");
			}

			if (ImGui::Button("Close"))
			{
				transformation_settings = false;
			}
			ImGui::End();
		}
	}
	//___________________________________________________________________________________________
	if (MeshLocal_window && ModelNumber < scene.GetModelCount())
	{
		if (ModelNumber != -1 && ModelNumber < scene.GetModelCount())
		{
			scene.meshmodelinput = ModelNumber;
			ImGui::Begin("Transformation :");
			ImGui::SameLine();
			ImGui::Text("Mesh Model Number : %d", ModelNumber);
			ImGui::NewLine();
			static float ScaleSize = 1;
			glm::mat4x4 Transtemp = scene.GetModel(ModelNumber).Get_TranslateMeshModelMatrix();
			glm::mat4x4 Scaletemp = glm::mat4x4(1);
			//scene.GetModel(ModelNumber).Set_ScaleMeshModelMatrix(Scaletemp);

			if (ImGui::SliderFloat("Size", &ScaleSize, 0.1, 32))
			{
				Scaletemp = glm::mat4x4(1);
				Scaletemp *= ScaleSize;
				scene.GetModel(ModelNumber).Set_ScaleMeshModelMatrix(Scaletemp);
				scene.GetModel(ModelNumber).ApplyMatrix();
			}
			if (ImGui::SliderFloat("Translate X", &Transtemp[3][0], -10, 10))
			{
				scene.GetModel(ModelNumber).Set_TranslateMeshModelMatrix(Transtemp);
				scene.GetModel(ModelNumber).ApplyMatrix();
			}
			if (ImGui::SliderFloat("Translate Y", &Transtemp[3][1], -10, 10))
			{
				scene.GetModel(ModelNumber).Set_TranslateMeshModelMatrix(Transtemp);
				scene.GetModel(ModelNumber).ApplyMatrix();
			}
			if (ImGui::SliderFloat("Translate Z", &Transtemp[3][2], -10, 10))
			{
				scene.GetModel(ModelNumber).Set_TranslateMeshModelMatrix(Transtemp);
				scene.GetModel(ModelNumber).ApplyMatrix();
			}
			if (ImGui::SliderFloat("Rotate X", &scene.GetModel(ModelNumber).Get_AlphaXMeshModelMatrix(), -360, 360))
			{
				scene.GetModel(ModelNumber).Set_AlphaXMeshModelMatrix(scene.GetModel(ModelNumber).Get_AlphaXMeshModelMatrix());
				scene.GetModel(ModelNumber).RotationX();
				scene.GetModel(ModelNumber).CalculateRotation();
				scene.GetModel(ModelNumber).ApplyMatrix();
			}
			if (ImGui::SliderFloat("Rotate Y", &scene.GetModel(ModelNumber).Get_AlphaYMeshModelMatrix(), -360, 360))
			{
				scene.GetModel(ModelNumber).Set_AlphaYMeshModelMatrix(scene.GetModel(ModelNumber).Get_AlphaYMeshModelMatrix());
				scene.GetModel(ModelNumber).RotationY();
				scene.GetModel(ModelNumber).CalculateRotation();
				scene.GetModel(ModelNumber).ApplyMatrix();
			}
			if (ImGui::SliderFloat("Rotate Z", &scene.GetModel(ModelNumber).Get_AlphaZMeshModelMatrix(), -360, 360))
			{
				scene.GetModel(ModelNumber).Set_AlphaZMeshModelMatrix(scene.GetModel(ModelNumber).Get_AlphaZMeshModelMatrix());
				scene.GetModel(ModelNumber).RotationZ();
				scene.GetModel(ModelNumber).CalculateRotation();
				scene.GetModel(ModelNumber).ApplyMatrix();
			}
			ImGui::NewLine();
			if (ImGui::Button("Reset"))
			{
				scene.GetModel(ModelNumber).ResetMeshModelRotation();
				scene.GetModel(ModelNumber).ApplyMatrix();
				scene.GetModel(ModelNumber).Set_AlphaXMeshModelMatrix(0);
				scene.GetModel(ModelNumber).Set_AlphaYMeshModelMatrix(0);
				scene.GetModel(ModelNumber).Set_AlphaZMeshModelMatrix(0);
			}

			if (ImGui::Button("Close"))
			{
				MeshLocal_window = false;
			}
			ImGui::End();

			//___________________________________________________________________________________________
		}
	}
	if (MeshWorld_window && ModelNumber < scene.GetModelCount())
	{
		if (scene.GetModel(ModelNumber).GetStatus())
		{
			static float WorldSize = 1;
			static float world_translate_X = 0, world_translate_Y = 0, world_translate_Z = 0;
			static float worldrotateX = 0, worldrotateY = 0, worldrotateZ = 0;
			ImGui::Begin("World Transformations");
			ImGui::NewLine();
			if (scene.GetModelCount())
			{
				static float World_RotationX = 0, World_RotationY = 0, World_RotationZ = 0;
				if (ImGui::SliderFloat("World Size", &WorldSize, 0.1, 32))
				{
					for (int model_numbertemp = 0; model_numbertemp < scene.GetModelCount(); model_numbertemp++)
					{
						scene.GetModel(model_numbertemp).WorldScale = glm::mat4x4(1);
						scene.GetModel(model_numbertemp).WorldScale = WorldSize * scene.GetActiveModel().WorldScale;
						scene.GetModel(model_numbertemp).ApplyMatrix();
					}

				}
				if (ImGui::SliderFloat("World Translate X", &world_translate_X, -10, 10))
				{
					for (int model_numbertemp = 0; model_numbertemp < scene.GetModelCount(); model_numbertemp++)
					{
						scene.GetModel(model_numbertemp).WorldTranslate[3][0] = world_translate_X;
						scene.GetModel(model_numbertemp).ApplyMatrix();
						scene.GetModel(model_numbertemp).ApplyWorldMatrix();

					}
				}
				if (ImGui::SliderFloat("World Translate Y", &world_translate_Y, -10, 10))
				{
					for (int model_numbertemp = 0; model_numbertemp < scene.GetModelCount(); model_numbertemp++)
					{
						scene.GetModel(model_numbertemp).WorldTranslate[3][1] = world_translate_Y;
						scene.GetModel(model_numbertemp).ApplyMatrix();
						scene.GetModel(model_numbertemp).ApplyWorldMatrix();
					}
				}
				if (ImGui::SliderFloat("World Translate Z", &world_translate_Z, -10, 10))
				{
					for (int model_numbertemp = 0; model_numbertemp < scene.GetModelCount(); model_numbertemp++)
					{
						scene.GetModel(model_numbertemp).WorldTranslate[3][2] = world_translate_Z;
						scene.GetModel(model_numbertemp).ApplyMatrix();
					}
				}

				if (ImGui::SliderFloat("World Rotate X", &World_RotationX, -360, 360))
				{

					for (int c = 0; c < scene.GetModelCount(); c++)
					{
						scene.GetModel(c).WorldAlphaX = World_RotationX;
						scene.GetModel(c).WorldRotationX();
						scene.GetModel(c).CalculateWorldRotation();
						scene.GetModel(c).ApplyMatrix();
						scene.GetModel(c).ApplyWorldMatrix();


					}
				}
				if (ImGui::SliderFloat("World Rotate Y", &World_RotationY, -360, 360))
				{
					for (int c = 0; c < scene.GetModelCount(); c++)
					{
						scene.GetModel(c).WorldAlphaY = World_RotationY;
						scene.GetModel(c).WorldRotationY();
						scene.GetModel(c).CalculateWorldRotation();
						scene.GetModel(c).ApplyMatrix();

					}
				}
				if (ImGui::SliderFloat("World Rotate Z", &World_RotationZ, -360, 360))
				{
					for (int c = 0; c < scene.GetModelCount(); c++)
					{
						scene.GetModel(c).WorldAlphaZ = World_RotationZ;
						scene.GetModel(c).WorldRotationZ();
						scene.GetModel(c).CalculateWorldRotation();
						scene.GetModel(c).ApplyMatrix();

					}
				}
			}


			if (ImGui::Button("Close"))
			{
				MeshWorld_window = false;
			}

			ImGui::End();
		}
	}
	if (MeshModel_Effects && ModelNumber < scene.GetModelCount())
	{

		ImGui::Begin("Effects");
		if (ModelNumber != -1 && ModelNumber < scene.GetModelCount())
		{
			ImGui::SliderInt("SubDivison Level is :", &scene.GetModel(ModelNumber).SubDivision, 1, 8);
			if (ImGui::ColorEdit3("Mesh Model Color", (float*)&MeshModel_Color))
			{
				scene.GetModel(ModelNumber).Mesh_Model_Color = glm::vec3(MeshModel_Color);
			}
			if (ImGui::Checkbox("Draw Vertices Box Mesh Model", &scene.GetModel(ModelNumber).VerticesColorful))
			{
			}
			if (ImGui::Checkbox("Fill Mesh Model Color", &scene.GetModel(ModelNumber).FillColor))
			{
				scene.GetModel(ModelNumber).Color_MeshModel_Triangles = false;
				scene.GetModel(ModelNumber).FillBlackGray = false;

			}
			if (ImGui::Checkbox("Fill Mesh Model Colorful", &scene.GetModel(ModelNumber).Color_MeshModel_Triangles))
			{
				scene.GetModel(ModelNumber).FillColor = false;
				scene.GetModel(ModelNumber).FillBlackGray = false;

			}
			if (ImGui::Checkbox("Fill Mesh Model GreyScale", &scene.GetModel(ModelNumber).FillBlackGray))
			{
				scene.GetModel(ModelNumber).FillColor = false;
				scene.GetModel(ModelNumber).Color_MeshModel_Triangles = false;

			}
			if (ImGui::Checkbox("Remove Grid", &scene.GetModel(ModelNumber).OffGrid))
			{

			}
			if (ImGui::Checkbox("Apply Texture", &scene.GetModel(ModelNumber).ApplyTexture))
			{
			}

		}

		if (ImGui::Button("Close"))
		{
			MeshModel_Effects = false;
		}
		ImGui::End();

	}
	if (Extensions && ModelNumber < scene.GetModelCount())
	{
		ImGui::Begin("Extensions For Mesh Model");
		ImGui::NewLine();
		if (ModelNumber != -1 && ModelNumber < scene.GetModelCount())
		{
			if (ImGui::Checkbox("Draw Mesh Model Axis", &scene.GetModel(ModelNumber).AxisStatus))
			{
			}
			if (ImGui::Checkbox("Draw World Axis", &scene.GetModel(ModelNumber).WorldAxisStatus))
			{
			}
			if (ImGui::Checkbox("Normals for vertices", &scene.GetModel(ModelNumber).NormalStatus))
			{
			}
			if (ImGui::Checkbox("Normals for faces", &scene.GetModel(ModelNumber).NormalStatusPerFaces))
			{
			}
			if (ImGui::Checkbox("Draw Rectangle of Mesh Model", &scene.GetModel(ModelNumber).RectangleStatus))
			{
			}
			if (ImGui::ColorEdit3("Mesh Model Color", (float*)&MeshModel_Color))
			{
				scene.GetModel(ModelNumber).Mesh_Model_Color = glm::ivec4(MeshModel_Color);
			}
			if (ImGui::ColorEdit3("Normals Color", (float*)&MeshModel_Normals))
			{
				scene.GetModel(ModelNumber).Normals_Mesh_Model_Color = MeshModel_Normals;
			}

		}
		if (ImGui::Button("Close"))
		{
			Extensions = false;
		}
		ImGui::End();
	}
	//___________________________________________________________________________________________
	if (Lights_Controls)
	{
		scene.Lightinput = Light_Number;
		if (scene.Lightinput > -1 && scene.Lightinput < scene.GetLightsCount())
		{
			ImGui::Begin("Lighting & Shading");

			ImGui::Text("Light Source #%d", scene.Lightinput);
			static bool Colors_Options = false, Mesh_Model_Options = false;
			ImGui::NewLine();
			if (ImGui::Checkbox("Show Reflected Rays", &scene.GetLight(scene.Lightinput).ShowReflectedRays))
			{
			}
			if (ImGui::Checkbox("Flat Shade", &scene.GetLight(scene.Lightinput).Flat_Shade))
			{
			}
			if (scene.GetLight(scene.Lightinput).Flat_Shade)
			{
				ImGui::SliderFloat("Intensity Level : ", &scene.GetLight(scene.Lightinput).FlatIntensity, 0, 100000.0f);
			}

			if (ImGui::Checkbox("Gouraud Shade", &scene.GetLight(scene.Lightinput).Gouraud_Shade))
			{
			}
			if (scene.GetLight(scene.Lightinput).Gouraud_Shade)
			{
				ImGui::SliderFloat("Intensity Level : ", &scene.GetLight(scene.Lightinput).GouraudIntensity, 0, 100000.0f);
			}

			if (ImGui::Checkbox("Phong Shade", &scene.GetLight(scene.Lightinput).Phong_Shade))
			{
			}
			if (scene.GetLight(scene.Lightinput).Phong_Shade)
			{
				ImGui::SliderFloat("Intensity Level : ", &scene.GetLight(scene.Lightinput).PhongIntensity, 0, 100000.0f);
			}

			if (ImGui::Checkbox("Lights Colors Options", &Colors_Options))
			{
			}
			if (ImGui::Checkbox("Mesh Model Options", &Mesh_Model_Options))
			{
			}

			ImGui::NewLine();
			if (Colors_Options)
			{
				const char* listbox_items[] = { "Ambient", "Diffuse", "Specular" };
				static int listbox_item_current = -1;
				if (ImGui::ListBox("Select Light Reflection", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 3))
				{

				}
				if (listbox_item_current == 0)
				{
					static float RedAmbient = 1.0f, BlueAmbient = 1.0f, GreenAmbient = 1.0f;
					//cout << scene.GetLight(scene.Lightinput).Ambient_Color.x << " " << scene.GetLight(scene.Lightinput).Ambient_Color.y << " " << scene.GetLight(scene.Lightinput).Ambient_Color.z << endl;
					if (ImGui::SliderFloat("Ambient Red Level", &RedAmbient, 0, 1))
					{
						scene.GetLight(scene.camerainput).Set_RedAmbient(RedAmbient);
						scene.GetLight(scene.camerainput).Ambient();
					}
					if (ImGui::SliderFloat("Ambient Green Level", &GreenAmbient, 0, 1))
					{
						scene.GetLight(scene.camerainput).Set_GreenAmbient(GreenAmbient);
						scene.GetLight(scene.camerainput).Ambient();
					}
					if (ImGui::SliderFloat("Ambient Blue Level", &BlueAmbient, 0, 1))
					{
						scene.GetLight(scene.camerainput).Set_BlueAmbient(BlueAmbient);
						scene.GetLight(scene.camerainput).Ambient();
					}
					if (ImGui::ColorEdit3("Ambient Light Color", (float*)&scene.GetLight(scene.Lightinput).Ambient_Color))
					{
					}

				}
				if (listbox_item_current == 1)
				{
					static float RedDiffuse = 1.0f, BlueDiffuse = 1.0f, GreenDiffuse = 1.0f;
					if (ImGui::SliderFloat("Diffuse Red Level", &RedDiffuse, 0, 1))
					{
						scene.GetLight(scene.camerainput).Set_RedDiffuse(RedDiffuse);
						scene.GetLight(scene.camerainput).Diffuse();
					}
					if (ImGui::SliderFloat("Diffuse Green Level", &GreenDiffuse, 0, 1))
					{
						scene.GetLight(scene.camerainput).Set_GreenDiffuse(GreenDiffuse);
						scene.GetLight(scene.camerainput).Diffuse();
					}
					if (ImGui::SliderFloat("Diffuse Blue Level", &BlueDiffuse, 0, 1))
					{
						scene.GetLight(scene.camerainput).Set_BlueDiffuse(BlueDiffuse);
						scene.GetLight(scene.camerainput).Diffuse();
					}
					if (ImGui::ColorEdit3("Diffuse Light Color", (float*)&scene.GetLight(scene.Lightinput).Diffuse_Color))
					{
					}

				}
				if (listbox_item_current == 2)
				{
					static float RedSpecular = scene.GetLight(scene.camerainput).Get_RedSpecular(), BlueSpecular = scene.GetLight(scene.camerainput).Get_BlueSpecular(), GreenSpecular = scene.GetLight(scene.camerainput).Get_GreenSpecular();
					static float SpecularDegree = scene.GetLight(scene.camerainput).Get_SpecularDegree();
					
					if (ImGui::SliderFloat("Specular Red Level", &RedSpecular, 0, 1))
					{
						scene.GetLight(scene.camerainput).Set_RedSpecular(RedSpecular);
						scene.GetLight(scene.camerainput).Specular();
					}
					if (ImGui::SliderFloat("Specular Green Level", &GreenSpecular, 0, 1))
					{
						scene.GetLight(scene.camerainput).Set_GreenSpecular(GreenSpecular);
						scene.GetLight(scene.camerainput).Specular();
					}
					if (ImGui::SliderFloat("Specular Blue Level", &BlueSpecular, 0, 1))
					{
						scene.GetLight(scene.camerainput).Set_BlueSpecular(BlueSpecular);
						scene.GetLight(scene.camerainput).Specular();
					}
					if (ImGui::SliderFloat("Specular Degree Level", &SpecularDegree, 0, 216, "%.3f", 2))
					{
						scene.GetLight(scene.camerainput).Set_SpecularDegree(SpecularDegree);
					}
					if (ImGui::ColorEdit3("Specular Light Color", (float*)&scene.GetLight(scene.Lightinput).Specular_Color))
					{
					}
				}
			}
			if (Mesh_Model_Options && scene.GetModelCount())
			{
				ImGui::Text("Mesh Model is : %d", scene.meshmodelinput);
				const char* listbox_items[] = { "Ambient", "Diffuse", "Specular" };
				static int listbox_item_current = -1;

				if (ImGui::ListBox("Select Mesh Model Reflection", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 1))
				{
				}
				if (listbox_item_current == 0)
				{
					if (ImGui::SliderFloat("Mesh Model Ambient Red Level", &scene.GetModel(scene.meshmodelinput).Ambient_Red, 0, 1))
					{
						scene.GetModel(scene.meshmodelinput).Ambient();
					}
					if (ImGui::SliderFloat("Mesh Model Ambient Green Level", &scene.GetModel(scene.meshmodelinput).Ambient_Green, 0, 1))
					{
						scene.GetModel(scene.meshmodelinput).Ambient();
					}
					if (ImGui::SliderFloat("Mesh Model Ambient Blue Level", &scene.GetModel(scene.meshmodelinput).Ambient_Blue, 0, 1))
					{
						scene.GetModel(scene.meshmodelinput).Ambient();
					}
				}
				if (listbox_item_current == 1)
				{
					if (ImGui::SliderFloat("Mesh Model Diffuse Red Level", &scene.GetModel(scene.meshmodelinput).Diffuse_Red, 0, 1))
					{
						scene.GetModel(scene.meshmodelinput).Diffuse();
					}
					if (ImGui::SliderFloat("Mesh Model Diffuse Green Level", &scene.GetModel(scene.meshmodelinput).Diffuse_Green, 0, 1))
					{
						scene.GetModel(scene.meshmodelinput).Diffuse();
					}
					if (ImGui::SliderFloat("Mesh Model Diffuse Blue Level", &scene.GetModel(scene.meshmodelinput).Diffuse_Blue, 0, 1))
					{
						scene.GetModel(scene.meshmodelinput).Diffuse();
					}
				}
				if (listbox_item_current == 2)
				{
					if (ImGui::SliderFloat("Mesh Model Specular Red Level", &scene.GetModel(scene.meshmodelinput).Specular_Red, 0, 1))
					{
						scene.GetModel(scene.meshmodelinput).Specular();
					}
					if (ImGui::SliderFloat("Mesh Model Specular Green Level", &scene.GetModel(scene.meshmodelinput).Specular_Green, 0, 1))
					{
						scene.GetModel(scene.meshmodelinput).Specular();
					}
					if (ImGui::SliderFloat("Mesh Model Specular Blue Level", &scene.GetModel(scene.meshmodelinput).Specular_Blue, 0, 1))
					{
						scene.GetModel(scene.meshmodelinput).Specular();
					}
				
				}
			}
			if (ImGui::Button("Close"))
			{
				Lights_Controls = false;
			}

			ImGui::End();
		}
	}
		
	if (Lights_Transformation)
	{
		scene.Lightinput = Light_Number;
		static bool local = false, world = false;
		ImGui::Begin("Transformation");
		scene.Lightinput = Light_Number;

		if (scene.Lightinput > -1 && scene.Lightinput < scene.GetLightsCount())
		{
			ImGui::Text("Light Source Shape:");
			if (ImGui::Checkbox("Rectangle Light", &scene.GetLight(scene.camerainput).LightRectangle))
			{
				scene.GetLight(scene.camerainput).LightLine = false;
				scene.GetLight(scene.camerainput).LightRound = false;
			}
			if(ImGui::Checkbox("Line Light", &scene.GetLight(scene.camerainput).LightLine))
			{
				scene.GetLight(scene.camerainput).LightRound = false;
				scene.GetLight(scene.camerainput).LightRectangle = false;
			}
			if (ImGui::Checkbox("Round Light", &scene.GetLight(scene.camerainput).LightRound))
			{
				scene.GetLight(scene.camerainput).LightLine = false;
				scene.GetLight(scene.camerainput).LightRectangle = false;
			}
			if (ImGui::ColorEdit3("Light Source Main Color", (float*)&scene.GetLight(scene.Lightinput).LightColor))
			{
			}
			if (scene.GetLight(scene.camerainput).LightRound)
			{
				ImGui::InputFloat("Radius Length", &scene.GetLight(scene.camerainput).Radius, 1, 100, 10);
				glm::vec3 new_position = glm::vec3(scene.GetLight(scene.camerainput).TotalTransformation * glm::vec4(scene.GetLight(scene.camerainput).Position, 1));
				ImGui::Text("Position Point x = %lf , y = %lf , z = %lf ", new_position.x, new_position.y, new_position.z);
			}
			if (scene.GetLight(scene.camerainput).LightLine)
			{
				ImGui::InputFloat3("Light Point #1 :", &scene.GetLight(scene.Lightinput).LinePosition1.x,0);
				ImGui::InputFloat3("Light Point #2 :", &scene.GetLight(scene.Lightinput).LinePosition2.x,0);
				ImGui::InputFloat3("Orientation :", &scene.GetLight(scene.Lightinput).LightLineVector.x, 0);

			}
			if (scene.GetLight(scene.camerainput).LightRectangle)
			{
				ImGui::InputFloat("Radius Length", &scene.GetLight(scene.camerainput).Radius, 1, 100, 10);
				glm::vec3 new_position = glm::vec3(scene.GetLight(scene.camerainput).TotalTransformation * glm::vec4(scene.GetLight(scene.camerainput).Position, 1));
				ImGui::Text("Position Point x = %lf , y = %lf , z = %lf ", new_position.x, new_position.y, new_position.z);
			}
			if (ImGui::Checkbox("Local Translation", &local))
			{
			}
			if (ImGui::Checkbox("World Translation", &world))
			{
			}
			if (local)
			{
				//translation and rotations of camera indivually - no effect from world rotation.
				if (ImGui::SliderFloat("Translate X", &scene.GetLight(scene.Lightinput).Translate[3][0], -1000, 1000))
				{
					scene.GetLight(scene.Lightinput).CalculateTransformation();
					scene.GetLight(scene.Lightinput).CalculateWorldTransformation();
				}
				if (ImGui::SliderFloat("Translate Y", &scene.GetLight(scene.Lightinput).Translate[3][1], -1000, 1000))
				{
					scene.GetLight(scene.Lightinput).CalculateTransformation();
					scene.GetLight(scene.Lightinput).CalculateWorldTransformation();
				}
				if (ImGui::SliderFloat("Translate Z", &scene.GetLight(scene.Lightinput).Translate[3][2], -1000, 1000))
				{
					scene.GetLight(scene.Lightinput).CalculateTransformation();
					scene.GetLight(scene.Lightinput).CalculateWorldTransformation();
				}
			}
			if (world)
			{
				//translation and rotations of camera indivually - no effect from world rotation.
				if (ImGui::SliderFloat("World Translate X", &scene.GetLight(scene.Lightinput).Position.x, -1000, 1000))
				{
					scene.GetLight(scene.Lightinput).CalculateTransformation();
				}
				if (ImGui::SliderFloat("World Translate Y", &scene.GetLight(scene.Lightinput).Position.y, -1000, 1000))
				{
					scene.GetLight(scene.Lightinput).CalculateTransformation();
				}
				if (ImGui::SliderFloat("WorldTranslate Z", &scene.GetLight(scene.Lightinput).Position.z, -1000, 1000))
				{
					scene.GetLight(scene.Lightinput).CalculateTransformation();
				}

			}
		}

		if (ImGui::Button("Close"))
		{
			Lights_Transformation = false;
		}
		ImGui::End();
	}
	//___________________________________________________________________________________________
	if (statics_window)
	{
		ImGui::Begin("Statics", &statics_window);
		ImGui::Text("All Data Regarding Coordinations :");
		ImGui::NewLine();
		if (scene.GetModelCount() && ModelNumber > -1)
		{
			ImGui::Text("Active Mesh Model Matrix %d : ", ModelNumber);
			PrintMatrix(scene.GetActiveModel().Get_CurrentMeshModelMatrix());
			ImGui::NewLine();
			ImGui::Text("World Mesh Model Matrix : ");
			PrintMatrix(scene.GetActiveModel().World_matrix);
			ImGui::NewLine();
			ImGui::Text("Active Camera Matrix : ");
			PrintMatrix(scene.GetActiveCamera().GetCameraTotalTransform());
			ImGui::NewLine();
			ImGui::Text("World Camera Matrix : ");
			PrintMatrix(scene.GetActiveCamera().World_TotalTransform);
			ImGui::NewLine();
			//scene.GetModel(ModelNumber).Set_ZeroPoint();
			ImGui::Text("Center Point is : %f %f %f", scene.GetModel(ModelNumber).Get_ZeroPoint().x, scene.GetModel(ModelNumber).Get_ZeroPoint().y, scene.GetModel(ModelNumber).Get_ZeroPoint().z);


		}
		if (ImGui::Button("Close"))
		{
			statics_window = false;
		}
		ImGui::End();
	}
	//___________________________________________________________________________________________
	if (BackGround_Settings)
	{
		ImGui::Begin("Background Color Settings");
		ImGui::NewLine();
		ImGui::ColorEdit3("Background Color", (float*)&Background_Color);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		if (ImGui::Button("Close"))
		{
			BackGround_Settings = false;
		}
		ImGui::End();
	}
	//___________________________________________________________________________________________
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow();
	}

}




