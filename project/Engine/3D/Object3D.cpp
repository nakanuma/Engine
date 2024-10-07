#include "Object3D.h"
#include "Camera.h"
#include "SRVManager.h"

#include <numbers>

Object3D::Object3D()
{
	transform_.translate = { 0.0f, 0.0f, 0.0f };
	transform_.rotate = { 0.0f, 0.0f, 0.0f };
	transform_.scale = { 1.0f, 1.0f, 1.0f };

	// 白を書き込む
	materialCB_.data_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// ライティング有効化
	materialCB_.data_->enableLighting = true;
	// 単位行列で初期化
	materialCB_.data_->uvTransform = Matrix::Identity();
	// 光沢を初期化
	materialCB_.data_->shininess = 50.0f;

	// 平行光源のデフォルト値を書き込む
	directionalLightCB_.data_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightCB_.data_->direction = { 0.0f, -1.0f, 0.0f };
	/*directionalLightCB_.data_->intensity = 1.0f;*/   // 平行光源有効
	directionalLightCB_.data_->intensity = 0.0f;   // 平行光源無効

	// ポイントライトのデフォルト値を書き込む
	pointLightCB_.data_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	pointLightCB_.data_->position = { 0.0f, 2.0f, 0.0f };
	/*pointLightCB_.data_->intensity = 1.0f; */   // ポイントライト有効
	pointLightCB_.data_->intensity = 0.0f;    // ポイントライト無効
	pointLightCB_.data_->radius = 5.0f;
	pointLightCB_.data_->decay = 1.0f;

	// スポットライトのデフォルト値を書き込む
	spotLightCB_.data_->color = {1.0f, 1.0f, 1.0f, 1.0f};
	spotLightCB_.data_->position = {2.0f, 1.25f, 0.0f};
	spotLightCB_.data_->distance = 7.0f;
	spotLightCB_.data_->direction = {-1.0f, -1.0f, 0.0f};
	spotLightCB_.data_->intensity = 4.0f;
	spotLightCB_.data_->decay = 2.0f;
	spotLightCB_.data_->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	spotLightCB_.data_->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 4.0f);
}

void Object3D::UpdateMatrix()
{
	Matrix worldMatrix = transform_.MakeAffineMatrix();
	Matrix viewMatrix = Camera::GetCurrent()->MakeViewMatrix();
	Matrix projectionMatrix = Camera::GetCurrent()->MakePerspectiveFovMatrix();
	Matrix worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
	wvpCB_.data_->WVP = worldViewProjectionMatrix;
	wvpCB_.data_->World = worldMatrix;

	// 逆転置行列を求める
	Matrix worldInverseMatrix = Matrix::Inverse(worldMatrix);
	Matrix worldInverseTransposeMatrix = Matrix::Transpose(worldInverseMatrix);

	wvpCB_.data_->WorldInverseTranspose = worldInverseTransposeMatrix;
}

void Object3D::Draw()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 平行光源の定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightCB_.resource_->GetGPUVirtualAddress());
	// ポイントライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLightCB_.resource_->GetGPUVirtualAddress());
	// スポットライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(7, spotLightCB_.resource_->GetGPUVirtualAddress());

	// commandListにVBVを設定
	dxBase->GetCommandList()->IASetVertexBuffers(0, 1, &model_->vertexBufferView);
	// commandListにIBVを設定
	dxBase->GetCommandList()->IASetIndexBuffer(&model_->indexBufferView);
	// プリミティブトポロジーの設定
	dxBase->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialCB_.resource_->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpCB_.resource_->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定（Textureの設定）
	TextureManager::SetDescriptorTable(2, dxBase->GetCommandList(), model_->material.textureHandle); // モデルデータに格納されたテクスチャを使用する
	// 描画を行う（DrawCall/ドローコール）
	dxBase->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(model_->indices.size()), 1, 0, 0, 0);
}

void Object3D::Draw(const int TextureHandle)
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 平行光源の定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightCB_.resource_->GetGPUVirtualAddress());
	// ポイントライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLightCB_.resource_->GetGPUVirtualAddress());
	// スポットライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(7, spotLightCB_.resource_->GetGPUVirtualAddress());

	// commandListにVBVを設定
	dxBase->GetCommandList()->IASetVertexBuffers(0, 1, &model_->vertexBufferView);
	// マテリアルCBufferの場所を設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialCB_.resource_->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpCB_.resource_->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定（Textureの設定）
	TextureManager::SetDescriptorTable(2, dxBase->GetCommandList(), TextureHandle); // 指定したテクスチャを使用する
	// 描画を行う（DrawCall/ドローコール）
	dxBase->GetCommandList()->DrawInstanced(UINT(model_->vertices.size()), 1, 0, 0);
}

void Object3D::Draw(ModelManager::SkinCluster skinCluster)
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 平行光源の定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightCB_.resource_->GetGPUVirtualAddress());
	// ポイントライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(6, pointLightCB_.resource_->GetGPUVirtualAddress());
	// スポットライトの定数バッファをセット
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(7, spotLightCB_.resource_->GetGPUVirtualAddress());

	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		model_->vertexBufferView, // VertexDataのVBV
		skinCluster.influenceBufferView // InfluenceのVBV
	};

	// 配列を渡す（開始Slot番号、使用Slot番号、VBV配列へのポインタ）
	dxBase->GetCommandList()->IASetVertexBuffers(0, 2, vbvs);
	// commandListにIBVを設定
	dxBase->GetCommandList()->IASetIndexBuffer(&model_->indexBufferView);
	// プリミティブトポロジーの設定
	dxBase->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialCB_.resource_->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpCB_.resource_->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定（Textureの設定）
	TextureManager::SetDescriptorTable(2, dxBase->GetCommandList(), model_->material.textureHandle); // モデルデータに格納されたテクスチャを使用する
	// PaletteのSRVを設定
	dxBase->GetCommandList()->SetGraphicsRootDescriptorTable(5, skinCluster.paletteSrvHandle.second);
	// 描画を行う（DrawCall/ドローコール）
	dxBase->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(model_->indices.size()), 1, 0, 0, 0);
}

void Object3D::DrawInstancing(StructuredBuffer<ParticleForGPU>& structuredBuffer, uint32_t numInstance, const uint32_t TextureHandle)
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// パーティクル用ルートシグネチャを設定
	dxBase->GetCommandList()->SetGraphicsRootSignature(dxBase->GetRootSignatureParticle());
	// パーティクル用PSOを設定
	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineStateParticle());
	// commandListにVBVを設定
	dxBase->GetCommandList()->IASetVertexBuffers(0, 1, &model_->vertexBufferView);
	// マテリアルCBufferの場所を設定
	dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialCB_.resource_->GetGPUVirtualAddress());
	// instancing用のDataを読むためにStructuredBufferのSRVを設定する
	dxBase->GetCommandList()->SetGraphicsRootDescriptorTable(1, SRVManager::GetInstance()->descriptorHeap.GetGPUHandle(structuredBuffer.heapIndex_));
	// SRVのDescriptorTableの先頭を設定（Textureの設定）
	TextureManager::SetDescriptorTable(2, dxBase->GetCommandList(), TextureHandle); // 引数で指定したテクスチャを使用する
	// 描画を行う（DrawCall/ドローコール）
	dxBase->GetCommandList()->DrawInstanced(UINT(model_->vertices.size()), numInstance, 0, 0);
}
