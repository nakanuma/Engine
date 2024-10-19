#include "PlayerMoveEmitter.h"
#include <random>

PlayerMoveEmitter::PlayerMoveEmitter()
{
}

PlayerMoveEmitter::~PlayerMoveEmitter()
{
}

void PlayerMoveEmitter::Initialize(ModelManager::ModelData* model, uint32_t textureHandle)
{
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
}

void PlayerMoveEmitter::Update()
{
	timer_++;

	for (auto itr = particleList_.begin(); itr != particleList_.end();) {
		(*itr)->Update();

		if ((*itr)->GetDelFlag()) {
			itr = particleList_.erase(itr);
		} else {
			itr++;
		}
	}
}

void PlayerMoveEmitter::Draw()
{
	for (auto& particle : particleList_) {
		particle->Draw();
	}
}

void PlayerMoveEmitter::Emit(Float3 translation)
{
	if (timer_ % interval == 0) {
		// 乱数生成器を初期化
		std::random_device rd;
		std::mt19937 rng(rd());

		// 各移動速度を決める
		const float vel = 0.01f;
		std::uniform_real_distribution<float> distVel(-vel, vel);
		// 速度ベクトルをランダムに生成
		Float3 velocity(distVel(rng), distVel(rng), distVel(rng));

		// プレイヤーの中心からどれくらいずらすか
		const float offset = 0.7f;
		std::uniform_real_distribution<float> distPos(-offset, offset);
		// 生成位置をランダムに生成
		Float3 position(
			translation.x + distPos(rng),
			translation.y + distPos(rng),
			translation.z + distPos(rng)
		);

		std::unique_ptr<PlayerMoveParticle> newParticle =
			std::make_unique<PlayerMoveParticle>(model_, textureHandle_, position, velocity);
		particleList_.push_back(std::move(newParticle));
	}
}
