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
	
	const float kMoveSpeed = 0.2f;

	// カメラの向きに基づいた前方向ベクトルを取得
	Float3 forward = Float3(
		-std::sin(transform_.rotate.y), 
		std::sin(transform_.rotate.x), 
		-std::cos(transform_.rotate.y)
	);
	// カメラの向きに基づいた右方向ベクトルを取得
	Float3 right = Float3(
		std::cos(transform_.rotate.y),
		0.0f, 
		-std::sin(transform_.rotate.y)
	);

	/* WASDキーで上下左右の移動 */
	// 上
	if (input->PushKey(DIK_W)) {
		transform_.translate -= forward * kMoveSpeed;
	}
	// 下
	if (input->PushKey(DIK_S)) {
		transform_.translate += forward * kMoveSpeed;
	}

	// 右
	if (input->PushKey(DIK_D)) {
		transform_.translate += right * kMoveSpeed;
	}
	// 左
	if (input->PushKey(DIK_A)) {
		transform_.translate -= right * kMoveSpeed;
	}

	/* 左クリックを押しながらドラッグで回転 */
	if (input->IsPressMouse(0)) {
		// マウスの移動量を取得
		int32_t dx, dy;
		dx = input->GetMouseMove().x;
		dy = input->GetMouseMove().y;

		transform_.rotate.x += static_cast<float>(dy) * 0.001f;
		transform_.rotate.y += static_cast<float>(dx) * 0.001f;
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
