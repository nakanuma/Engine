#pragma once
#include "Object3D.h"
#include "fstream"
#include "Camera.h"
#include "Model/ModelManager.h"
#include "MyMath.h"


// マップチップタイプ
enum class MapChipType 
{
	kBlank, // 空白
	kBlock, // ブロック
};

// マップチップデータ
struct  MapChipData 
{
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField
{
public:

	// マップ単位XZ
	struct IndexSet 
	{
		uint32_t xIndex;
		uint32_t zIndex;
	};

	// マップチップ1つの
	struct MapObject
	{
		std::unique_ptr<Object3D> worldTransformBlocks_;
		AABB collAABB_;
	};

public:
	
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ModelManager::ModelData model);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// マップチップデータのリセット
	void ResetMapChipData();
	//読み込み
	void LoadMapChipCsv(const std::string& filePath);
	//マップチップ種別の取得
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t zIndex);
	//マップチップ座標の取得
	Float3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t zIndex);

	//座標からマップチップ番号を計算
	IndexSet GetMapChipIndexSetByPosition(const Float3& position);

	

	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; }

	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }


	// マップチップ番号の位置との当たり判定(バグあり)
	bool IsMapAABB(AABB& charcter, IndexSet& index);
	
	// マップチップ全体の当たり判定
	bool IsMapAABB(AABB& charcter);

	// マップチップ番号のY座標の押し戻し処理(バグあり)
	void IsMapY(float& posY, float radY, IndexSet& index);
	
	// マップチップ全体のAABBと当たり判定をとり当たったAABBとY座標の押し戻し処理(isJampがtrueでonGround_がfalse)
	void IsMapY(AABB& charcter, float& posY, float radY);
	// マップチップ全体のAABBと当たり判定をとり当たったAABBとY座標の固定処理(onGround_がtrue)のとき
	void IsMapY2(AABB& charcter, float& posY, float radY);
private:
	
	// モデルデータ
	ModelManager::ModelData model_;
	
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	// 1ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 20;

	//
	MapChipData mapChipData_;

	

	// ブロック用のワールドトランスフォームをユニークポインタで管理。
	// 複数並べるために配列にする
	std::vector<std::vector<std::unique_ptr<MapObject>>> mapWorld_;

	//AABBの半径
	Float3 rad_ = { 0.5f, 0.5f, 0.5f };
};


