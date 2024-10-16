#include "MapChip.h"

#include <algorithm>

#include "Collision/Collider.h"
#include "DeltaTime/DeltaTime.h"

namespace
{
	std::map<std::string,MapChipType> mapChipTable = {
		{"0",MapChipType::kBlank},
		{"1",MapChipType::kBlock},
	};
}

void MapChipField::Initialize(ModelManager::ModelData model)
{
	model_ = model;

	// マップの初期化
	mapWorld_.resize(kNumBlockVirtical);
	for(uint32_t i = 0; i < kNumBlockVirtical; ++i)
	{
		mapWorld_[i].resize(kNumBlockHorizontal);
	}

	// ブロックの生成
	for(uint32_t i = 0; i < kNumBlockVirtical; ++i)
	{
		for(uint32_t j = 0; j < kNumBlockHorizontal; ++j)
		{
			if(GetMapChipTypeByIndex(j,i) == MapChipType::kBlock)
			{
				// MapObjectの生成
				auto mapObject = std::make_unique<MapObject>(this,IndexSet(j,i));
				mapObject->Init();

				// マップワールドに格納
				mapWorld_[i][j] = std::move(mapObject);
			}
		}
	}

	InitInstancing();
}

void MapChipField::Update()
{
	const float& deltaTime = DeltaTime::getInstance()->getDeltaTime();
	// 波
	// ブロックの更新
	for(int i = 0; i < mapWorld_.size(); ++i)
	{
		for(int j = 0; j < mapWorld_[i].size(); ++j)
		{
			auto& worldTransformBlock = mapWorld_[i][j];
			if(!worldTransformBlock)
				continue;

			// ウェーブが発生中の場合
			if(worldTransformBlock->isWeve)
			{
				// 遅延がまだ残っている場合はカウントダウン
				if(worldTransformBlock->waveDelay > 0)
				{
					// 速ければすぐ伝播する
					worldTransformBlock->waveDelay -= deltaTime; // フレーム時間分減少（約60FPSの場合）
					continue;                                 // 遅延中なので処理をスキップ
				}

				// 重力を適用してY軸を下降させる
				worldTransformBlock->velocity_.y -= 0.098f;

				// マップチップの位置情報の取得
				Float3 pos = worldTransformBlock->GetTranslate();

				// ｙ座標にマップの速度を加算
				pos.y += worldTransformBlock->velocity_.y;

				// その値を設定する
				worldTransformBlock->SetTranslate(pos);
			}

			// ブロックが下まで落ちたらリセット
			if(worldTransformBlock->GetTranslate().y <= 0)
			{
				// マップのウェーブを止める 
				worldTransformBlock->isWeve = false;
				worldTransformBlock->addressOfWaveOrigin_ = {0,0};
				// 座標を取得
				Float3 pos = worldTransformBlock->GetTranslate();

				// Y座標を0に設定
				pos.y = 0;

				// 値を代入
				worldTransformBlock->SetTranslate(pos);

				// 速度を0に設定
				worldTransformBlock->velocity_.y = 0;
			}
		}
	}
	// ブロックの更新
	int i = 0;
	for(auto& worldTransformBlockLine : mapWorld_)
	{
		int j = 0;
		for(auto& worldTransformBlock : worldTransformBlockLine)
		{
			if(!worldTransformBlock)
			{
				continue;
			}
			worldTransformBlock->Update();
			j++;
		}
		i++;
	}
	// 描画用オブジェクトの更新
	UpdateInstancedObjects();
}

void MapChipField::Draw()
{
	// ブロック描画
	for(auto& worldTransformBlockLine : mapWorld_)
	{
		for(auto& worldTransformBlock : worldTransformBlockLine)
		{
			if(!worldTransformBlock)
				continue;
			/*worldTransformBlock->Draw();*/
		}
	}

	mapObjIns_.InstancedDraw();
}

void MapChipField::ResetMapChipData()
{
	// マップチップデータをリセット
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for(std::vector<MapChipType>& mapChipDataLine : mapChipData_.data)
	{
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}
//読み込み
void MapChipField::LoadMapChipCsv(const std::string& filePath)
{
	// マップチップデータをリセット
	ResetMapChipData();

	//ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	//マップチップCSV
	std::stringstream mapChipCsv;
	//ファイルの内容を文字列にストリームにコピー
	mapChipCsv << file.rdbuf();
	//ファイルを閉じる
	file.close();

	// CSVからマップチップデータを読み込む
	for(uint32_t i = 0; i < kNumBlockVirtical; ++i)
	{
		std::string line;
		getline(mapChipCsv,line);

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		for(uint32_t j = 0; j < kNumBlockHorizontal; ++j)
		{
			std::string word;
			getline(line_stream,word,',');

			if(mapChipTable.contains(word))
			{
				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}
	}
}

//マップチップ種別の取得
MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex,uint32_t zIndex)
{
	if(xIndex < 0 || kNumBlockHorizontal - 1 < xIndex)
	{
		return MapChipType::kBlank;
	}
	if(zIndex < 0 || kNumBlockVirtical - 1 < zIndex)
	{
		return MapChipType::kBlank;
	}

	return mapChipData_.data[zIndex][xIndex];
}

//マップチップ座標の取得
//縦横の指定してその位置のマップチップワールド座標を取得する
Float3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex,uint32_t yIndex)
{
	return Float3(kBlockWidth * xIndex,0,kBlockHeight * (kNumBlockVirtical - 1 - yIndex));
}

//マップチップ番号を計算
MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Float3& position)
{
	MapChipField::IndexSet indexSet = {};
	indexSet.xIndex = uint32_t((position.x + kBlockWidth / 2) / kBlockWidth);
	indexSet.zIndex = uint32_t(kNumBlockVirtical - 1 - (int(position.z + kBlockHeight / 2.0f) / kBlockHeight));

	return indexSet;
}

bool MapChipField::IsMapAABB(AABB& charcter,IndexSet& index) { return IsCollision(charcter,mapWorld_[index.xIndex][index.zIndex]->GetCollider()); }

bool MapChipField::IsMapAABB(AABB& charcter)
{
	for(auto& worldTransformBlockLine : mapWorld_)
	{
		for(auto& worldTransformBlock : worldTransformBlockLine)
		{
			if(!worldTransformBlock)
				continue;
			if(IsCollision(charcter,worldTransformBlock->GetCollider()))
			{
				return true;
			}
		}
	}
	return false;
}

void MapChipField::IsMapY(float& posY,float radY,IndexSet& index)
{
	if(mapWorld_[index.xIndex][index.zIndex]->GetTranslate().y >= posY)
	{
		posY = mapWorld_[index.xIndex][index.zIndex]->GetTranslate().y + radY + rad_.y;
	}
}

void MapChipField::IsMapY(AABB& charcter,float& posY,float radY)
{
	for(auto& worldTransformBlockLine : mapWorld_)
	{
		for(auto& worldTransformBlock : worldTransformBlockLine)
		{
			if(!worldTransformBlock)
				continue;
			if(IsCollision(charcter,worldTransformBlock->GetCollider()))
			{
				if(worldTransformBlock->GetTranslate().y >= posY)
				{
					posY = worldTransformBlock->GetTranslate().y + radY + rad_.y;
				}
			}
		}
	}
}

void MapChipField::IsMapY2(AABB& charcter,float& posY,float radY)
{
	for(auto& worldTransformBlockLine : mapWorld_)
	{
		for(auto& worldTransformBlock : worldTransformBlockLine)
		{
			if(!worldTransformBlock)
				continue;
			if(IsCollision(charcter,worldTransformBlock->GetCollider()))
			{
				posY = worldTransformBlock->GetTranslate().y + radY + rad_.y;
			}
		}
	}
}

void MapChipField::CheckCollision_Collider(Collider* collider)
{
	Float3 colliderPos = collider->GetPosition();
	IndexSet colliderIndex = GetMapChipIndexSetByPosition(colliderPos);

	for(auto& rowOfMap : mapWorld_)
	{
		for(auto& mapObject : rowOfMap)
		{
			auto& aabb = mapObject->collAABB_;
			Float3 closestPoint = {
				std::clamp<float>(colliderPos.x,aabb.min.x,aabb.max.x),
				std::clamp<float>(colliderPos.y,aabb.min.y,aabb.max.y),
				std::clamp<float>(colliderPos.z,aabb.min.z,aabb.max.z)
			};

			float distance = Float3::Length(closestPoint - colliderPos);

			if(distance <= collider->GetRadius())
			{
				collider->OnCollisionMapChip(mapObject.get());
			}
		}
	}


//for(int32_t row = 1; row < 2; row++)
//{
//	IndexSet currentIndex;
//	currentIndex.zIndex = colliderIndex.zIndex + row;
//	if(currentIndex.zIndex < 0 || currentIndex.zIndex >= mapWorld_.size())
//	{
//		continue;
//	}
//	for(int32_t col = -1; col < 2; col++)
//	{
//		currentIndex.xIndex = colliderIndex.xIndex + row;
//		// 自分自身をスキップ
//		if(row == 0 && col == 0)
//		{
//			continue;
//		}
//		// 列が範囲外なら早期にスキップ
//		if(currentIndex.xIndex < 0 || currentIndex.xIndex >= mapWorld_[row].size())
//		{
//			continue;
//		}

//		auto& aabb = mapWorld_[currentIndex.zIndex][currentIndex.xIndex]->collAABB_;
//		Float3 closestPoint = {
//			std::clamp<float>(colliderPos.x,aabb.min.x,aabb.max.x),
//			std::clamp<float>(colliderPos.y,aabb.min.y,aabb.max.y),
//			std::clamp<float>(colliderPos.z,aabb.min.z,aabb.max.z)
//		};

//		float distance = Float3::Length(closestPoint - colliderPos);

//		if(distance <= collider->GetRadius())
//		{
//			collider->OnCollisionMapChip(mapWorld_[currentIndex.zIndex][currentIndex.xIndex].get());
//		}
//	}
//}
}

void MapChipField::InitInstancing()
{
	// ブロックの最大数分、StructuredBufferを用意して作成
	mapObjIns_.gTransformationMatrices.numMaxInstance_ = kNumBlockHorizontal * kNumBlockVirtical;
	mapObjIns_.gTransformationMatrices.Create();
	// モデルを指定
	mapObjIns_.model_ = &model_;

	// 全てのブロックの情報をStructuredBufferに格納
	for(uint32_t i = 0; i < kNumBlockVirtical; ++i)
	{
		for(uint32_t j = 0; j < kNumBlockHorizontal; ++j)
		{
			if(GetMapChipTypeByIndex(j,i) == MapChipType::kBlock)
			{
				Matrix world = Matrix::Translation(mapWorld_[i][j]->GetTranslate());
				Matrix view = Camera::GetCurrent()->MakeViewMatrix();
				Matrix projection = Camera::GetCurrent()->MakePerspectiveFovMatrix();

				// 初期座標をStructuredBufferに転送
				mapObjIns_.gTransformationMatrices.data_[i * kNumBlockHorizontal + j].WVP = world * view * projection;
				mapObjIns_.gTransformationMatrices.data_[i * kNumBlockHorizontal + j].World = world;
				mapObjIns_.gTransformationMatrices.data_[i * kNumBlockHorizontal + j].WorldInverseTranspose = Matrix::Inverse(world);
			}
		}
	}
}

void MapChipField::UpdateInstancedObjects()
{
	// 全てのブロックの情報をStructuredBufferに転送して更新する
	for(uint32_t i = 0; i < kNumBlockVirtical; ++i)
	{
		for(uint32_t j = 0; j < kNumBlockHorizontal; ++j)
		{
			if(GetMapChipTypeByIndex(j,i) == MapChipType::kBlock)
			{
				Matrix world = Matrix::Translation(mapWorld_[i][j]->GetTranslate());
				Matrix view = Camera::GetCurrent()->MakeViewMatrix();
				Matrix projection = Camera::GetCurrent()->MakePerspectiveFovMatrix();

				// 初期座標をStructuredBufferに転送
				mapObjIns_.gTransformationMatrices.data_[i * kNumBlockHorizontal + j].WVP = world * view * projection;
				mapObjIns_.gTransformationMatrices.data_[i * kNumBlockHorizontal + j].World = world;
				mapObjIns_.gTransformationMatrices.data_[i * kNumBlockHorizontal + j].WorldInverseTranspose = Matrix::Inverse(world);
			}
		}
	}
}

void MapChipField::MapObject::Init()
{
	transform_.translate = host_->GetMapChipPositionByIndex(address_.xIndex,address_.zIndex);
	collAABB_.max = Add(GetTranslate(),host_->rad_);
	collAABB_.min = Subtract(GetTranslate(),host_->rad_);
}

void MapChipField::MapObject::Update()
{
	prePos_ = transform_.translate;
	//Wave();
	// AABBのmaxとminを設定
	collAABB_.max = Add(transform_.translate,host_->rad_);
	collAABB_.min = Subtract(transform_.translate,host_->rad_);
}

// 衝突時にウェーブを発生させるための関数(マップ番号、マップ番号、ウェーブ範囲、Y軸の速度)
void MapChipField::TriggerWave(int hitX,int hitZ,float waveRange,float initialYVelocity)
{
	// 衝突した位置から円状にウェーブを広げる
	for(int r = 0; r < mapWorld_.size(); ++r)
	{
		for(int c = 0;c < mapWorld_[r].size(); ++c)
		{
			auto& worldTransformBlock = mapWorld_[r][c];

			// マップブロックがないなら動作させない
			if(!worldTransformBlock)
				continue;

			// 衝突位置のマップチップは動作させない
			if(r == hitX &&c == hitZ)
			{
				continue;
			}

			// ウェーブしている物は動作させない
			if(worldTransformBlock->isWeve)
				continue;

			// マップチップのXZ座標から衝突位置までの距離を計算
			float distanceX = static_cast<float>(c - hitX);
			float distanceZ = static_cast<float>(r - hitZ);
			float distance = sqrt(distanceX * distanceX + distanceZ * distanceZ);

			// 衝突位置から一定範囲内にある場合にウェーブを発生
			if(distance < waveRange)
			{
				worldTransformBlock->addressOfWaveOrigin_ = {hitX,hitZ};
				worldTransformBlock->isWeve = true; // ウェーブさせる
				worldTransformBlock->velocity_.y = initialYVelocity; // y軸の速度を代入
				worldTransformBlock->waveDelay = distance * 0.1f; // 距離に応じた遅延時間を設定
			}
		}
	}
}