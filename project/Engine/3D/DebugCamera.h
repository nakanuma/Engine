#pragma once
#include "MyMath.h"
#include "Input.h"

class DebugCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Input* input);

	/// <summary>
	/// ビュー行列の取得
	/// </summary>
	/// <returns></returns>
	Matrix GetViewMatrix() const { return viewMatrix_; }


	// カメラの情報を保持
	Transform transform_;

private:
	// ビュー行列
	Matrix viewMatrix_;
	// 射影行列
	Matrix projectionMatrix_;

	Matrix rotateMatrix_;
	Matrix translateMatrix_;
	Matrix WorldMatrix_;
};
