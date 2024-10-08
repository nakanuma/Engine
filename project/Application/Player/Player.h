#pragma once

#include <memory>

#include "ModelManager.h"
#include "Object3D.h"
#include "PlayerState.h"

class Player
{
public:
	Player() = default;
	~Player() = default;

	void Initialize(uint32_t uvCheckerGH);
	void Update();
	void Draw();
private:
	std::unique_ptr<IPlayerState> currentState_;

	ModelManager::ModelData bodyModelData_;
	std::unique_ptr<Object3D> bodyObject_;

	ModelManager::ModelData handModelData_;
	std::unique_ptr<Object3D> handObject_;
public:
	void TransitionState(IPlayerState* state);

	const Float3& GetBodyTranslate()const { return bodyObject_->transform_.translate; }
	void SetBodyTranslate(const Float3& translate) { bodyObject_->transform_.translate = translate; }
	const Float3& GetHandTranslate()const { return handObject_->transform_.translate; }
	void SetHandTranslate(const Float3& translate) { handObject_->transform_.translate = translate; }

	Float3 GetWorldPosition()const
	{
		return Float3(
			bodyObject_->wvpCB_.data_->World.r[3][0],
			bodyObject_->wvpCB_.data_->World.r[3][1],
			bodyObject_->wvpCB_.data_->World.r[3][2]
		);
	}

	const Float3& GetBodyRotate()const { return bodyObject_->transform_.rotate; }
	void SetBodyRotate(const Float3& rotate) { bodyObject_->transform_.rotate = rotate; }
	const Float3& GetHandRotate()const { return handObject_->transform_.rotate; }
	void SetHandRotate(const Float3& rotate) { handObject_->transform_.rotate = rotate; }

};

