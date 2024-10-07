#pragma once
#include "Transform.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "ConstBuffer.h"
#include "StructuredBuffer.h"

class Object3D
{
public:
	struct Material {
		Float4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix uvTransform;
		float shininess;
	};

	struct TransformationMatrix {
		Matrix WVP;
		Matrix World;
		Matrix WorldInverseTranspose;
	};

	struct DirectionalLight {
		Float4 color; // ライトの色
		Float3 direction; // ライトの向き
		float intensity; // 輝度
	};

	struct PointLight {
		Float4 color; // ライトの色
		Float3 position; // ライトの位置
		float intensity; // 輝度
		float radius; // ライトの届く最大距離
		float decay; // 減衰率
		float padding[2];
	};

	struct SpotLight {
		Float4 color;     // ライトの色
		Float3 position;  // ライトの位置
		float intensity;  // 輝度
		Float3 direction; // スポットライトの方向
		float distance;   // ライトの届く最大距離
		float decay;      // 減衰率
		float cosAngle;   // スポットライトの余弦
		float cosFalloffStart; // Falloff開始の角度
		float padding[1];
	};

	struct ParticleForGPU {
		Matrix WVP;
		Matrix World;
		Float4 color;
	};

	Object3D();

	// マトリックス情報の更新
	void UpdateMatrix();

	// 描画（モデル内のテクスチャを参照 / テクスチャを指定して描画）
	void Draw();

	void Draw(const int TextureHandle);

	void Draw(ModelManager::SkinCluster skinCluster);

	void DrawInstancing(StructuredBuffer<ParticleForGPU>& structuredBuffer, uint32_t numInstance, const uint32_t TextureHandle);

	// マテリアルの定数バッファ
	ConstBuffer<Material>materialCB_;

	// トランスフォームの定数バッファ
	ConstBuffer<TransformationMatrix>wvpCB_;

	// モデル情報
	ModelManager::ModelData* model_ = nullptr;

	// トランスフォーム情報
	Transform transform_;

	// 平行光源の定数バッファ
	ConstBuffer<DirectionalLight> directionalLightCB_;
	// ポイントライトの定数バッファ
	ConstBuffer<PointLight> pointLightCB_;
	// スポットライトの定数バッファ
	ConstBuffer<SpotLight> spotLightCB_;

};

