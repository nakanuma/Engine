#pragma once
#include "MyMath.h"
#include "ConstBuffer.h"

struct CameraCBData {
	Float3 position;
};

class Camera
{
public:
	Camera(Float3 translate, Float3 rotate = Float3(0.0f, 0.0f, 0.0f), float fov = PIf / 2.0f);
	static void TransferConstantBuffer();

	// カメラの情報を保持
	Transform transform;
	// 視野角（ラジアン）
	float fov;
	// クリップの設定
	float nearZ = 0.1f, farZ = 1000.0f;

	Matrix MakeViewMatrix();
	Matrix MakePerspectiveFovMatrix();

	static void Set(Camera* camera) { current_ = camera; }
	static Camera* GetCurrent() { return current_; }
private:
	inline static Camera* current_;

	ConstBuffer<CameraCBData> cameraCB_;

/// <summary>
/// シェイク
/// </summary>
public:
	/// <summary>
	/// シェイクを開始する際のパラメーター設定
	/// </summary>
	/// <param name="intensity">強さ</param>
	/// <param name="duration">シェイクするフレーム</param>
	void ApplyShake(float intensity, float duration);

	/// <summary>
	/// シェイクを更新
	/// </summary>
	void UpdateShake();

	/// <summary>
	/// カメラの初期値を格納（カメラ初期化後でないと位置を取得できないため、改めて現在位置を格納するために使用）
	/// </summary>
	/// <param name="originalPosition"></param>
	void SetOriginalPosition(Float3 currentCameraTranslate) { originalPosition_ = currentCameraTranslate; }

public:
	float shakeIntensity_ = 0.0f;
	float shakeDuration_ = 0.0f;
	float shakeTimer_ = 0.0f;
	Float3 originalPosition_;
};

