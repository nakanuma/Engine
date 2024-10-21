#pragma once

#include <memory>

#include <stdint.h>
#include <string>

#include "Object3D.h"

class TexturePlane
{
public:
	void Initialize(const std::string& sceneName,const std::string& groupName,uint32_t textureIndex,ID3D12Device* device);
	void Update();
	void Draw();
private:
	ModelManager::ModelData planeModel_;
	std::unique_ptr<Object3D> planeObject_;
};

