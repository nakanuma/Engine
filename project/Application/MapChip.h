#pragma once
#include "Object3D.h"
#include "fstream"
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

struct  MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField
{
public:
	struct IndexSet {
		uint32_t xIndex;
		uint32_t zIndex;
	};

	struct Rect {
		float left;   // 左端
		float right;  // 右端
		float bottom; // 下端
		float top;    // 上端
	};

public:
	
	//void Initialize();

	void ResetMapChipData();
	//読み込み
	void LoadMapChipCsv(const std::string& filePath);
	//マップチップ種別の取得
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t zIndex);
	//マップチップ座標の取得
	Float3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t zIndex);

	//座標からマップチップ番号を計算
	IndexSet GetMapChipIndexSetByPosition(const Float3& position);

	//ブロックの範囲取得関数
	Rect GetRectByIndex(uint32_t xIndex, uint32_t zIndex);


	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; }

	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }
private:
	
	// モデルデータ
	ModelManager::ModelData model_;
	// 3Dオブジェクト
	std::unique_ptr<Object3D> object_;

	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	// 1ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 20;


	MapChipData mapChipData_;


};


