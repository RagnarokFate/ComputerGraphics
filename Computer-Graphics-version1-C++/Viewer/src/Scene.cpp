#include "Scene.h"
#include "MeshModel.h"
#include "Utils.h"
#include "Face.h"
#include<iostream>
#include<iterator> // for iterators
#include<vector> // for vectors

using namespace std;
#include <string>


Scene::Scene() :
	active_camera_index(0),
	active_model_index(0),
	active_light_index(0)
{
	
}

void Scene::AddModel(const std::shared_ptr<MeshModel>& mesh_model)
{
	mesh_models.push_back(mesh_model);
	active_model_index = mesh_models.size() - 1;
}

int Scene::GetModelCount() const
{
	return mesh_models.size();
}

MeshModel& Scene::GetModel(int index) const
{
	return *mesh_models[index];
}

MeshModel& Scene::GetActiveModel() const
{
	return *mesh_models[active_model_index];
}

void Scene::AddCamera(const std::shared_ptr<Camera>& camera)
{
	cameras.push_back(camera);
	active_camera_index = cameras.size() - 1;
}

int Scene::GetCameraCount() const
{
	return cameras.size();
}

Camera& Scene::GetCamera(int index)
{
	return *cameras[index];
}

Camera& Scene::GetActiveCamera()
{
	return *cameras[active_camera_index];
}

void Scene::SetActiveCameraIndex(int index)
{
	active_camera_index = index;
}

int Scene::GetActiveCameraIndex() const
{
	return active_camera_index;
}

void Scene::SetActiveModelIndex(int index)
{
	active_model_index = index;
}

int Scene::GetActiveModelIndex() const
{
	return active_model_index;
}

void Scene::AddLight(const std::shared_ptr<Light>& Input_Light)
{
	Lights.push_back(Input_Light);
	active_light_index = Lights.size() - 1;
}
Light& Scene::GetLight(int index)
{
	return *Lights[index];
}
Light& Scene::GetActiveLight(int index)
{
	return *Lights[active_light_index];
}
int Scene::GetActiveLightIndex() const
{
	return active_light_index;
}
void Scene::SetActiveLightIndex(int index)
{
	active_light_index = index;
}
int Scene::GetLightsCount() const
{
	return Lights.size();
}