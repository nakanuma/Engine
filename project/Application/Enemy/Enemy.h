#pragma once

#include <memory>

#include <stdint.h>

#include "ModelManager.h"
#include "Object3D.h"
#include "Float2.h"


class Enemy
{
public:
	Enemy() = default;
	~Enemy();

	void Initialize(ModelManager::ModelData* modelData);
	void Update();
	void Draw();
private:
	std::unique_ptr<Object3D> object_;

	Float2 moveDirection_;
	float speed_;
};