#pragma once
#include "MyMath.h"

class DebugCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

private:
	// カメラの情報を保持
	Transform transform_;

	// ビュー行列
	Matrix viewMatrix_;
	// 射影行列
	Matrix projectionMatrix_;

	Matrix rotateMatrix_;
	Matrix translateMatrix_;
	Matrix WorldMatrix_;
};
