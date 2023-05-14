#include "scene.h"

Scene::Scene() {

	PlayerCreateInfo playerInfo;
	playerInfo.eulers = { 0.0f, 90.0f,0.0f };
	playerInfo.position = { 0.0f, 0.0f, 1.0f };
	player = new Player(&playerInfo);

	CubeCreateInfo cubeInfo;
	cubeInfo.eulers = { 0.0f, 0.0f, 0.0f };
	cubeInfo.position = { 3.0f, 0.0f, 0.5f };
	cube = new Cube(&cubeInfo);

	BrightbillboardCreateInfo lightInfo;
	lightInfo.color = glm::vec3(1, 0, 0);
	lightInfo.position = glm::vec3(1, 0, 0);
	lightInfo.strength = 4.0f;
	lights.push_back(new BrightBillboard(&lightInfo));
	lightInfo.color = glm::vec3(0, 1, 0);
	lightInfo.position = glm::vec3(3, 2, 0);
	lights.push_back(new BrightBillboard(&lightInfo));
	lightInfo.color = glm::vec3(0, 1, 1);
	lightInfo.position = glm::vec3(3, 0, 2);
	lights.push_back(new BrightBillboard(&lightInfo));

	medkits.push_back(new Billboard(glm::vec3(2.5f, 0.0f, 0.5f)));
}

Scene::~Scene() {
	delete cube;
	delete player;
	for (BrightBillboard* light : lights) {
		delete light;
	}

	for (Billboard* medkit : medkits) {
		delete medkit;
	}
}

void Scene::update(float rate) {
	player->update();
	cube->update(rate);
	for (Billboard* medkit : medkits) {
		medkit->update(player->position);
	}
	for (BrightBillboard* light : lights) {
		light->update(player->position);
	}
}

void Scene::movePlayer(glm::vec3 dPos) {
	player->position += dPos;
}

void Scene::spinPlayer(glm::vec3 dEulers) {
	player->eulers += dEulers;

	if (player->eulers.z < 0) {
		player->eulers.z += 360;
	}
	else if (player->eulers.z > 360) {
		player->eulers.z -= 360;
	}

	player->eulers.y = std::max(std::min(player->eulers.y, 179.0f), 1.0f);
}