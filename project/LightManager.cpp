#include "LightManager.h"
#include "DirectXBase.h"

#include <numbers>

LightManager* LightManager::GetInstance()
{
    static LightManager instance;
    return &instance;
}

void LightManager::Initialize()
{
	// 平行光源のデフォルト値を書き込む
	directionalLightCB_.data_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightCB_.data_->direction = { 0.0f, -1.0f, 0.0f };
	directionalLightCB_.data_->intensity = 0.1f;   // 平行光源有効
	/*directionalLightCB_.data_->intensity = 0.0f;*/   // 平行光源無効

	// ポイントライトのデフォルト値を書き込む
	pointLightCB_.data_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	pointLightCB_.data_->position = { 0.0f, 2.0f, 0.0f };
	/*pointLightCB_.data_->intensity = 1.0f; */   // ポイントライト有効
	pointLightCB_.data_->intensity = 0.0f;    // ポイントライト無効
	pointLightCB_.data_->radius = 5.0f;
	pointLightCB_.data_->decay = 1.0f;

	// スポットライトのデフォルト値を書き込む
	spotLightCB_.data_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	spotLightCB_.data_->position = { 2.0f, 1.25f, 0.0f };
	spotLightCB_.data_->distance = 7.0f;
	spotLightCB_.data_->direction = { -1.0f, -1.0f, 0.0f };
	spotLightCB_.data_->intensity = 4.0f; // スポットライト有効
	/*spotLightCB_.data_->intensity = 0.0f;*/ // スポットライト無効
	spotLightCB_.data_->decay = 2.0f;
	spotLightCB_.data_->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	spotLightCB_.data_->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 4.0f);
}

void LightManager::TransferContantBuffer()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 平行光源の定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightCB_.resource_->GetGPUVirtualAddress());
	// ポイントライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLightCB_.resource_->GetGPUVirtualAddress());
	// スポットライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(7, spotLightCB_.resource_->GetGPUVirtualAddress());
}
