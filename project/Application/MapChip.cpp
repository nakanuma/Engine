#include "MapChip.h"


namespace
{

	std::map<std::string, MapChipType> mapChipTable = {
	{"0", MapChipType::kBlank},
	{"1", MapChipType::kBlock},
	};
}

void MapChipField::Initialize(ModelManager::ModelData model)
{

	model_ = model;

	// マップの初期化
	mapWorld_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		mapWorld_[i].resize(kNumBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i)
	{
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) 
		{
			if (GetMapChipTypeByIndex(j, i) == MapChipType::kBlock)
			{
				// MapObjectの生成
				auto mapObject = std::make_unique<MapObject>();
				mapObject->worldTransformBlocks_ = std::make_unique<Object3D>();
				mapObject->worldTransformBlocks_->model_ = &model_;
				mapObject->worldTransformBlocks_->transform_.translate = GetMapChipPositionByIndex(j, i);
				mapObject->collAABB_.max = Add(mapObject->worldTransformBlocks_->transform_.translate, rad_);
				mapObject->collAABB_.min = Subtract(mapObject->worldTransformBlocks_->transform_.translate, rad_);

				// マップワールドに格納
				mapWorld_[i][j] = std::move(mapObject);
			}
		}
	}

}

void MapChipField::Update()
{
	
	// ブロックの更新
	int i = 0;
	for (auto& worldTransformBlockLine : mapWorld_) 
	{
		int j = 0;
		for (auto& worldTransformBlock : worldTransformBlockLine)
		{
			if (!worldTransformBlock)
				continue;
			// AABBのmaxとminを設定
			worldTransformBlock->collAABB_.max = Add(worldTransformBlock->worldTransformBlocks_->transform_.translate, rad_);
			worldTransformBlock->collAABB_.min = Subtract(worldTransformBlock->worldTransformBlocks_->transform_.translate, rad_);
			
			worldTransformBlock->worldTransformBlocks_->UpdateMatrix();
			j++;
		}
		i++;
	}
	
}

void MapChipField::Draw()
{
	// ブロック描画
	for (auto& worldTransformBlockLine : mapWorld_) {
		for (auto& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			worldTransformBlock->worldTransformBlocks_ ->Draw();
		}
	}
}

void MapChipField::ResetMapChipData() 
{

	// マップチップデータをリセット
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) 
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
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) 
	{

		std::string line;
		getline(mapChipCsv, line);

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) 
		{

			std::string word;
			getline(line_stream, word, ',');

			if (mapChipTable.contains(word)) 
			{
				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}

	}

}

//マップチップ種別の取得
MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t zIndex)
{


	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex)
	{
		return MapChipType::kBlank;
	}
	if (zIndex < 0 || kNumBlockVirtical - 1 < zIndex)
	{
		return MapChipType::kBlank;
	}

	return mapChipData_.data[zIndex][xIndex];
}

//マップチップ座標の取得
//縦横の指定してその位置のマップチップワールド座標を取得する
Float3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex)
{

	return Float3(kBlockWidth * xIndex, 0, kBlockHeight * (kNumBlockVirtical - 1 - yIndex));

}

//マップチップ番号を計算
MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Float3& position) 
{
	MapChipField::IndexSet indexSet = {};
	indexSet.xIndex = uint32_t((position.x + kBlockWidth / 2) / kBlockWidth);
	indexSet.zIndex = uint32_t(kNumBlockVirtical - 1 - (int(position.z + kBlockHeight / 2.0f) / kBlockHeight));

	return indexSet;
}

bool MapChipField::IsMapAABB(AABB& charcter, IndexSet& index) { return IsCollision(charcter, mapWorld_[index.xIndex][index.zIndex]->collAABB_); }

bool MapChipField::IsMapAABB(AABB& charcter) 
{

	for (auto& worldTransformBlockLine : mapWorld_) 
	{
		for (auto& worldTransformBlock : worldTransformBlockLine) 
		{
			if (!worldTransformBlock)
				continue;
			if (IsCollision(charcter, worldTransformBlock->collAABB_))
			{
				return true;
			}
		}
	}

	return false;
}

void MapChipField::IsMapY(float& posY, float radY, IndexSet& index) 
{

	if (mapWorld_[index.xIndex][index.zIndex]->worldTransformBlocks_->transform_.translate.y >= posY)
	{
		posY = mapWorld_[index.xIndex][index.zIndex]->worldTransformBlocks_->transform_.translate.y + radY + rad_.y;
	}
}


void MapChipField::IsMapY(AABB& charcter, float& posY, float radY) 
{

	for (auto& worldTransformBlockLine : mapWorld_)
	{
		for (auto& worldTransformBlock : worldTransformBlockLine) 
		{
			if (!worldTransformBlock)
				continue;
			if (IsCollision(charcter, worldTransformBlock->collAABB_)) 
			{
				if (worldTransformBlock->worldTransformBlocks_->transform_.translate.y >= posY) 
				{
					posY = worldTransformBlock->worldTransformBlocks_->transform_.translate.y + radY + rad_.y;
				}
			}
		}
	}

}

void MapChipField::IsMapY2(AABB& charcter, float& posY, float radY) {


	for (auto& worldTransformBlockLine : mapWorld_) 
	{
		for (auto& worldTransformBlock : worldTransformBlockLine) 
		{
			if (!worldTransformBlock)
				continue;
			if (IsCollision(charcter, worldTransformBlock->collAABB_)) 
			{


				posY = worldTransformBlock->worldTransformBlocks_->transform_.translate.y + radY + rad_.y;

			}
		}
	}
}

