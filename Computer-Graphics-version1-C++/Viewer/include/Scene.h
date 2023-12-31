#pragma once

#include <vector>
#include <memory>
#include "Utils.h"
#include "Camera.h"
#include "Light.h"
#include "MeshModel.h"

using namespace std;

class Scene {
public:
	Scene();

	void AddModel(const shared_ptr<MeshModel>& mesh_model);
	int GetModelCount() const;
	MeshModel& GetModel(int index) const;
	MeshModel& GetActiveModel() const;
	
	void AddCamera(const shared_ptr<Camera>& camera);
	int GetCameraCount() const;
	Camera& GetCamera(int index);
	Camera& GetActiveCamera();

	void AddLight(const std::shared_ptr<Light>& Input_Light);
	Light& GetLight(int index);
	Light& Scene::GetActiveLight(int index);
	int GetActiveLightIndex() const;
	int GetLightsCount() const;
	void SetActiveLightIndex(int index);

	void SetActiveCameraIndex(int index);
	int GetActiveCameraIndex() const;

	void SetActiveModelIndex(int index);
	int GetActiveModelIndex() const;
	
	
	int meshmodelinput = -1;
	int camerainput = 0;
	int Lightinput = -1;
	glm::mat3x3 KernelMatrix = { 0.0596,0.0983,0.0596
							   , 0.0983,0.1621,0.0983
							   , 0.0596,0.0983,0.0596};
	bool BlurScene = false;
	bool FogScene = false;
	float FogFactor = 1.0f;


	glm::ivec3 main_color_buffer;
private:
	vector<shared_ptr<MeshModel>> mesh_models;
	vector<shared_ptr<Camera>> cameras;
	vector<shared_ptr<Light>> Lights;

	int active_camera_index;
	int active_model_index;
	int active_light_index;
};