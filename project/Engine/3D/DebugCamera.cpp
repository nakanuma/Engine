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

void DebugCamera::Update(Input* input) {
	///
	///	入力によるカメラの移動や回転
	/// 
	
	const float kMoveSpeed = 0.1f;

	///
	///	左右移動
	/// 
	
	// 右
	if (input->PushKey(DIK_D)) {
		transform_.translate.x += kMoveSpeed;
	}
	// 左
	if (input->PushKey(DIK_A)) {
		transform_.translate.x -= kMoveSpeed;
	}

	///
	///	上下移動
	///	
	
	// 上
	if (input->PushKey(DIK_W)) {
		transform_.translate.y += kMoveSpeed;
	}
	// 下
	if (input->PushKey(DIK_S)) {
		transform_.translate.y -= kMoveSpeed;
	}

	///
	///	ビュー行列の更新
	/// 

	// 角度から回転行列を計算
	rotateMatrix_ = Matrix::RotationRollPitchYaw(transform_.rotate.x, transform_.rotate.y, transform_.rotate.z);

	// 座標から平行移動行列を計算
	translateMatrix_ = Matrix::Translation(transform_.translate);

	// 回転行列と平行移動行列からワールド行列を計算する
	WorldMatrix_ = rotateMatrix_ * translateMatrix_;

	// ワールド行列の逆行列をビュー行列に代入する
	viewMatrix_ = Matrix::Inverse(WorldMatrix_);
}
