#include "Collider.h"

void Collider::Init(const Float3& pos, float radius, std::function<void(Collider*)> onCollisionFunc) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// Texture読み込み
	uint32_t uvCheckerGH = TextureManager::Load("resources/Images/uvChecker.png", dxBase->GetDevice());

	// モデルの読み込みとテクスチャの設定
	model_ = ModelManager::LoadModelFile("resources/Models", "block.obj", dxBase->GetDevice());
	model_.material.textureHandle = uvCheckerGH;
	
	object.model_ = &model_;
	object.transform_.translate = pos;
	object.transform_.scale = { radius / 2.0f, radius / 2.0f, radius / 2.0f };
	object.UpdateMatrix();

	radius_ = radius;

	onCollision_ = onCollisionFunc;
}

void Collider::Update() {
	object.UpdateMatrix();

	if (invincibleTime_ <= 0.0f) {
		return;
	}
	invincibleTime_ -= 1.0f / 60.0f;
}

void Collider::Draw() {
	object.Draw();
}
