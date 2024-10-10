#include "Collider.h"

void Collider::Initialize() {

	DirectXBase* dxBase = DirectXBase::GetInstance();

	// Texture読み込み
	uint32_t uvCheckerGHBlock = TextureManager::Load("resources/Images/uvChecker.png", dxBase->GetDevice());

	// モデルの読み込みとテクスチャの設定(マップチップ)
	modelCollosion_ = ModelManager::LoadModelFile("resources/Models", "block.obj", dxBase->GetDevice());
	modelCollosion_.material.textureHandle = uvCheckerGHBlock;

	object.model_ = &modelCollosion_;
	count++;
}

void Collider::UpdateWorldTransform() {

	aabb_.max = Add(object.transform_.translate, aabbRadius_);
	aabb_.min = Subtract(object.transform_.translate, aabbRadius_);

	// ワールド座標をワールドトランスフォームに適応
	object.transform_.translate = GetCenterPosition();

	object.UpdateMatrix();
}

void Collider::Draw() {
	object.Draw();
}

void Collider::SetTypeID(uint32_t typeID) { typeID_ = typeID; };
