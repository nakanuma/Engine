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

const float kGravity = 9.8f;

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
}

void MapChipField::Update()
{
	// 波
	for(auto& worldTransformBlockLine : mapWorld_)
	{
		for(auto& worldTransformBlock : worldTransformBlockLine)
		{
			if(!worldTransformBlock)
			{
				continue;
			}
			worldTransformBlock->isStop = false;
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
			worldTransformBlock->WaveSpawn();
			worldTransformBlock->Update();
			j++;
		}
		i++;
	}
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
			worldTransformBlock->Draw();
		}
	}
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

	for(int32_t row = 1; row < 2; row++)
	{
		IndexSet currentIndex;
		currentIndex.zIndex = colliderIndex.zIndex + row;
		if(currentIndex.zIndex < 0 || currentIndex.zIndex >= mapWorld_.size())
		{
			continue;
		}
		for(int32_t col = -1; col < 2; col++)
		{
			currentIndex.xIndex = colliderIndex.xIndex + row;
			// 自分自身をスキップ
			if(row == 0 && col == 0)
			{
				continue;
			}
			// 列が範囲外なら早期にスキップ
			if(currentIndex.xIndex < 0 || currentIndex.xIndex >= mapWorld_[row].size())
			{
				continue;
			}

			auto& aabb = mapWorld_[currentIndex.zIndex][currentIndex.xIndex]->collAABB_;
			Float3 closestPoint = {
				std::clamp<float>(colliderPos.x,aabb.min.x,aabb.max.x),
				std::clamp<float>(colliderPos.y,aabb.min.y,aabb.max.y),
				std::clamp<float>(colliderPos.z,aabb.min.z,aabb.max.z)
			};

			float distance = Float3::Length(closestPoint - colliderPos);

			if(distance <= collider->GetRadius())
			{
				collider->OnCollisionMapChip(mapWorld_[currentIndex.zIndex][currentIndex.xIndex].get());
			}
		}
	}
}

void MapChipField::MapObject::Init()
{
	worldTransformBlocks_ = std::make_unique<Object3D>();
	worldTransformBlocks_->model_ = &host_->model_;
	worldTransformBlocks_->transform_.translate = host_->GetMapChipPositionByIndex(address_.xIndex,address_.zIndex);
	collAABB_.max = Add(GetTranslate(),host_->rad_);
	collAABB_.min = Subtract(GetTranslate(),host_->rad_);
}

void MapChipField::MapObject::Update()
{
	prePos_ = worldTransformBlocks_->transform_.translate;
	Wave();

	velocity_ = worldTransformBlocks_->transform_.translate - prePos_;

	// AABBの max と min を 設定
	collAABB_.max = Add(worldTransformBlocks_->transform_.translate,host_->rad_);
	collAABB_.min = Subtract(worldTransformBlocks_->transform_.translate,host_->rad_);

	worldTransformBlocks_->UpdateMatrix();
}

void MapChipField::MapObject::Draw()
{
	worldTransformBlocks_->Draw();
}

void MapChipField::MapObject::StartWaveOrigin(float amplitude)
{
	for(int row = -1; row < 2; row++)
	{
		int newRow = address_.xIndex + row;
		// 行が範囲外なら早期にスキップ
		if(newRow < 0 || newRow >= host_->mapWorld_.size())
		{
			continue;
		}
		for(int col = -1; col < 2; col++)
		{
			// 自分自身をスキップ
			if(row == 0 && col == 0)
			{
				continue;
			}

			int newCol = address_.zIndex + col;

			// 列が範囲外なら早期にスキップ
			if(newCol < 0 || newCol >= host_->mapWorld_[newRow].size())
			{
				continue;
			}

			host_->mapWorld_[newRow][newCol]->StartWave(
				{col,row},
				amplitude * 0.87f
			);
		};
	}
}

void MapChipField::MapObject::StartWave(Int2 waveDirection,float amplitude)
{
	currentAmplitude_ += amplitude;

	if(currentAmplitude_ <= 1.0f && waveDirection == Int2(0,0))
	{
		currentAmplitude_ = 0.0f;
		return;
	}

	// 与える振幅 (waveDirection.x方向)
	int row = 0;
	int col = 0;

	float propagationAmplitude = amplitude * 0.89f;

	if(propagationAmplitude <= 1.0f)
	{
		return;
	}

	if(waveDirection.x != 0)
	{
		bool isStop = waveDirection.x < 0;
		// 与える振幅 (waveDirection.x方向)
		row = address_.zIndex + 1;
		col = address_.xIndex + waveDirection.x;

	   // 範囲チェック (行と列がそれぞれ有効な範囲に収まっているか)
		if(row >= 0 && row < host_->mapWorld_.size() &&
		   col >= 0 && col < host_->mapWorld_[row].size())
		{
			if(host_->mapWorld_[row][col]->startWaveTaskQueue_.empty())
			{
				host_->mapWorld_[row][col]->isStop |= isStop;
				auto start = [this,row,col,waveDirection,propagationAmplitude]()
				{
					host_->mapWorld_[row][col]->StartWave({waveDirection.x,1},propagationAmplitude);
				};
				host_->mapWorld_[row][col]->startWaveTaskQueue_.push(start);
			}
		}


	// 与える振幅 (waveDirection.y方向)
		row = address_.zIndex + 0;

		// 範囲チェック
		if(row >= 0 && row < host_->mapWorld_.size() &&
		   col >= 0 && col < host_->mapWorld_[row].size())
		{
			if(host_->mapWorld_[row][col]->startWaveTaskQueue_.empty())
			{
				host_->mapWorld_[row][col]->isStop |= isStop;
				auto start = [this,row,col,waveDirection,propagationAmplitude]()
				{
					host_->mapWorld_[row][col]->StartWave({waveDirection.x,0},propagationAmplitude);
				};
				host_->mapWorld_[row][col]->startWaveTaskQueue_.push(start);
			}
		}


// 与える振幅 (waveDirection.x, waveDirection.y 両方方向、斜め)
		row = address_.xIndex + -1;
		isStop = false;
		// 範囲チェック
		if(row >= 0 && row < host_->mapWorld_.size() &&
		   col >= 0 && col < host_->mapWorld_[row].size())
		{
			if(host_->mapWorld_[row][col]->startWaveTaskQueue_.empty())
			{
				host_->mapWorld_[row][col]->isStop |= isStop;
				auto start = [this,row,col,waveDirection,propagationAmplitude]()
				{
					host_->mapWorld_[row][col]->StartWave({waveDirection.x,-1},propagationAmplitude);
				};
				host_->mapWorld_[row][col]->startWaveTaskQueue_.push(start);
			}
		}
	}
	if(waveDirection.y != 0)
	{
		bool isStop = waveDirection.y < 0;
		row = address_.zIndex;
		col = address_.xIndex + 1;

	   // 範囲チェック (行と列がそれぞれ有効な範囲に収まっているか)
		if(row >= 0 && row < host_->mapWorld_.size() &&
		   col >= 0 && col < host_->mapWorld_[row].size())
		{
			if(host_->mapWorld_[row][col]->startWaveTaskQueue_.empty())
			{
				host_->mapWorld_[row][col]->isStop |= isStop;
				auto start = [this,row,col,waveDirection,propagationAmplitude]()
				{
					host_->mapWorld_[row][col]->StartWave({1,waveDirection.y},propagationAmplitude);
				};
				host_->mapWorld_[row][col]->startWaveTaskQueue_.push(start);
			}
		}

		col = address_.xIndex + 0;

		// 範囲チェック
		if(row >= 0 && row < host_->mapWorld_.size() &&
		   col >= 0 && col < host_->mapWorld_[row].size())
		{
			if(host_->mapWorld_[row][col]->startWaveTaskQueue_.empty())
			{
				host_->mapWorld_[row][col]->isStop |= isStop;
				auto start = [this,row,col,waveDirection,propagationAmplitude]()
				{
					host_->mapWorld_[row][col]->StartWave({0,waveDirection.y},propagationAmplitude);
				};
				host_->mapWorld_[row][col]->startWaveTaskQueue_.push(start);
			}
		}

   // 与える振幅 (waveDirection.x, waveDirection.y 両方方向、斜め)
		col = address_.xIndex + -1;
		isStop = true;
		// 範囲チェック
		if(row >= 0 && row < host_->mapWorld_.size() &&
		   col >= 0 && col < host_->mapWorld_[row].size())
		{
			if(host_->mapWorld_[row][col]->startWaveTaskQueue_.empty())
			{
				host_->mapWorld_[row][col]->isStop |= isStop;
				auto start = [this,row,col,waveDirection,propagationAmplitude]()
				{
					host_->mapWorld_[row][col]->StartWave({-1,waveDirection.y},propagationAmplitude);
				};
				host_->mapWorld_[row][col]->startWaveTaskQueue_.push(start);
			}
		}
	}
}

void MapChipField::MapObject::WaveSpawn()
{
	if(isStop)
	{
		return;
	}
	if(!startWaveTaskQueue_.empty())
	{
		startWaveTaskQueue_.back()();
		startWaveTaskQueue_.pop();
	}
}

void MapChipField::MapObject::Wave()
{
	if(currentAmplitude_ * currentAmplitude_ <= 0.0f)
	{
		return;
	}
	currentAmplitude_ -= kGravity * DeltaTime::getInstance()->getDeltaTime();

	worldTransformBlocks_->transform_.translate.y += currentAmplitude_ ;

	if(GetTranslate().y <= 0.01f)
	{
		currentAmplitude_ = 0.0f;
		worldTransformBlocks_->transform_.translate.y = 0.0f;
		return;
	}
}