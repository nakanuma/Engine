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

	void Initialize();
	void Update();
	void Draw();
private:
	std::unique_ptr<IPlayerState> currentState_;

	ModelManager::ModelData modelData_;
	std::unique_ptr<Object3D> object_;
public:
	void TransitionState(IPlayerState* state);

	const Float3& GetTranslate()const { return object_->transform_.translate; }
	void SetTranslate(const Float3& translate) { object_->transform_.translate = translate; }
	Float3 GetWorldPosition()const
	{
		return Float3(
			object_->wvpCB_.data_->World.r[3][0],
			object_->wvpCB_.data_->World.r[3][1],
			object_->wvpCB_.data_->World.r[3][2]
		);
	}
};

