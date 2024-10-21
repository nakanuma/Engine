#include "TexturePlane.h"

#include "TextureManager.h"
#include "GlobalVariables.h"

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif // _DEBUG

void TexturePlane::Initialize(const std::string& sceneName,const std::string& groupName,uint32_t textureIndex,ID3D12Device* device)
{
	const DirectX::TexMetadata& metaData = TextureManager::GetInstance().GetMetaData(textureIndex);

	Float2 textureHalfSize = {static_cast<float>(metaData.width) * 0.05f,static_cast<float>(metaData.height) * 0.05f};

	planeModel_ = ModelManager::LoadModelFile("resources/Models","plane.obj",device);
	planeModel_.material.textureHandle = textureIndex;

	planeObject_ = std::make_unique<Object3D>();
	planeObject_->model_ = &planeModel_;

	planeModel_.vertices[0].position = {-textureHalfSize.x,-textureHalfSize.y, 0.0f,1.0f};
	planeModel_.vertices[1].position = { textureHalfSize.x,textureHalfSize.y, 0.0f,1.0f};
	planeModel_.vertices[2].position = { textureHalfSize.x,-textureHalfSize.y,0.0f,1.0f};
	planeModel_.vertices[3].position = {-textureHalfSize.x,-textureHalfSize.y,0.0f,1.0f};
	planeModel_.vertices[4].position = {-textureHalfSize.x,textureHalfSize.y,0.0f,1.0f};
	planeModel_.vertices[5].position = {textureHalfSize.x,textureHalfSize.y,0.0f,1.0f};

	 // 頂点リソースにデータを書き込む
	ModelManager::VertexData* vertexData = nullptr;
	// 書き込むためのアドレスを取得
	planeModel_.vertexResource->Map(0,nullptr,reinterpret_cast<void**>(&vertexData));
	// 頂点データをリソースにコピー
	std::memcpy(vertexData,planeModel_.vertices.data(),sizeof(ModelManager::VertexData) * planeModel_.vertices.size());

	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue(sceneName,groupName,"scale",planeObject_->transform_.scale);
	variables->addValue(sceneName,groupName,"rotate",planeObject_->transform_.rotate);
	variables->addValue(sceneName,groupName,"translate",planeObject_->transform_.translate);
	planeObject_->UpdateMatrix();
}

void TexturePlane::Update()
{
	planeObject_->UpdateMatrix();
}

void TexturePlane::Draw()
{
	planeObject_->Draw();
}
