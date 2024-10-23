#include "EnemyDivideEmitter.h"
#include <numbers>
#include <cmath>

EnemyDivideEmitter::EnemyDivideEmitter()
{
}

EnemyDivideEmitter::~EnemyDivideEmitter()
{
}

void EnemyDivideEmitter::Initialize(ModelManager::ModelData* model, uint32_t textureHandle)
{
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
}

void EnemyDivideEmitter::Update()
{
	for (auto itr = particleList_.begin(); itr != particleList_.end();) {
		(*itr)->Update();

		if ((*itr)->GetDelFlag()) {
			itr = particleList_.erase(itr);
		} else {
			itr++;
		}
	}
}

void EnemyDivideEmitter::Draw()
{
	for (auto& particle : particleList_) {
		particle->Draw();
	}
}

void EnemyDivideEmitter::Emit(Float3 translation)
{
	float angleStep = std::numbers::pi_v<float> *2.0f / kParticleNum;

	for (uint32_t i = 0; i < kParticleNum; i++) {
		float angle = angleStep * i;

		Float3 velocity = {
			cosf(angle),
			0.0f,
			sinf(angle)
		};

		std::unique_ptr<EnemyDivideParticle> newParticle =
			std::make_unique<EnemyDivideParticle>(model_, textureHandle_, translation, velocity);
		particleList_.push_back(std::move(newParticle));
	}
}
