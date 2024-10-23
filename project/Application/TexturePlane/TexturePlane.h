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
public:
	const ModelManager::ModelData& GetModelData()const { return planeModel_; }
	const Object3D* GetPlaneObject()const { return planeObject_.get(); }
	Object3D* GetPlaneObject() { return planeObject_.get(); }

	const Float4& GetLtPos()const { return planeModel_.vertices[0].position; }
	const Float4& GetLbPos()const { return planeModel_.vertices[4].position; }
	const Float4& GetRtPos()const { return planeModel_.vertices[2].position; }
	const Float4& GetRbPos()const { return planeModel_.vertices[1].position; }

	Matrix GetWorldMatrix()const{return planeObject_->transform_.MakeAffineMatrix();}
};

