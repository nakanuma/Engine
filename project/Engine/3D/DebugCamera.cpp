#include "DebugCamera.h"

void DebugCamera::Initialize() {
	// transformの初期化
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.translate = {0.0f, 0.0f, -50.0f};
	transform_.scale = {1.0f, 1.0f, 1.0f};

	// 各行列を単位行列で埋める
	viewMatrix_ = Matrix::Identity();
	projectionMatrix_ = Matrix::Identity();

	rotateMatrix_ = Matrix::Identity();
	translateMatrix_ = Matrix::Identity();
	WorldMatrix_ = Matrix::Identity();
}

void DebugCamera::Update() {
	
}
