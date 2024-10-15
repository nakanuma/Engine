#include "camera.h"
#include "MyWindow.h"
#include "DirectXBase.h"

#include "ImguiWrapper.h"

Camera::Camera(Float3 argTranslate, Float3 argRotate, float argFov)
{
	// 引数で受け取った位置、回転、視野角を設定
	transform.translate = argTranslate;
	transform.rotate = argRotate;
	transform.scale = { 1.0f, 1.0f, 1.0 };
	fov = argFov;

	// CBにカメラのポジションをセット
	cameraCB_.data_->position = argTranslate;
}

void Camera::TransferConstantBuffer()
{
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, current_->cameraCB_.resource_->GetGPUVirtualAddress());
}

Matrix Camera::MakeViewMatrix()
{
	// カメラのtransformからアフィン変換行列を作成
	Matrix affine = transform.MakeAffineMatrix();
	// 逆行列を計算して返す（ビューマトリックス）
	return Matrix::Inverse(affine);
}

Matrix Camera::MakePerspectiveFovMatrix()
{
	// 透視投影行列を生成して返す
	return Matrix::PerspectiveFovLH(fov, static_cast<float>(Window::GetWidth()) / static_cast<float>(Window::GetHeight()), nearZ, farZ);
}

void Camera::ApplyShake(float intensity, float duration) { 
	shakeIntensity_ = intensity; 
	shakeDuration_ = duration;
	shakeTimer_ = duration;
}

void Camera::UpdateShake() {
	if (shakeTimer_ > 0) {
		// シェイクの計算
		Float3 shakeOffset = {
			shakeIntensity_ * (rand() / static_cast<float>(RAND_MAX) - 0.5f), 
			shakeIntensity_ * (rand() / static_cast<float>(RAND_MAX) - 0.5f), 
			0.0f
		};

		// カメラの位置にシェイクを適用
		transform.translate = originalPosition_ + shakeOffset;

		// シェイク効果を徐々に減少させる
		float t = static_cast<float>(shakeTimer_) / static_cast<float>(shakeDuration_);
		float easeFactor = t * t * (3.0f - 2.0f * t);
		shakeIntensity_ *= easeFactor;

		// タイマーを更新
		shakeTimer_--;
	} else {
		// シェイク終了時に元の位置に戻す
		transform.translate = originalPosition_;
		shakeIntensity_ = 0.0f;
		shakeDuration_ = 0.0f;
		shakeTimer_ = 0.0f;
	}
}
