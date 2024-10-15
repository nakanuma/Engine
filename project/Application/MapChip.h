#pragma once

#include <fstream>
#include <functional>
#include <queue>

#include "Camera.h"
#include "Model/ModelManager.h"
#include "MyMath.h"
#include "Object3D.h"
#include "InstancedObject.h"
#include "Camera.h"

#include "Int2.h"

class Collider;
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

// マップチップ全体
class MapChipField
{
public:
	// マップ単位XZ
	struct IndexSet
	{
		int32_t xIndex;
		int32_t zIndex;
	};

	// マップチップ1つの
	class MapObject
	{
	public:
		MapObject(MapChipField* host,IndexSet address):host_(host),address_(address) {}
		~MapObject() = default;

		void Init();
		void Update();

		
		float waveDelay;		// ウェーブの広がり遅延
		bool isWeve = false;	// ウェーブしているかどうかのフラグ
		AABB collAABB_;			// AABB
		Float3 velocity_;		// マップの速度
	private:
		Float3 prePos_;
		IndexSet address_;

		Transform transform_; // 上記のオブジェクトが必要無くなったのでTransformをそのまま持たせる

		MapChipField* host_;

	public:
		const AABB& GetCollider()const { return collAABB_; }
		const Float3& GetTranslate()const { return transform_.translate; }
		void SetTranslate(Float3 &trans) { transform_.translate = trans; } // translateの設定
		IndexSet GetIndexSet()const { return address_; }
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
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex,uint32_t zIndex);
	//マップチップ座標の取得
	Float3 GetMapChipPositionByIndex(uint32_t xIndex,uint32_t zIndex);

	//座標からマップチップ番号を計算
	IndexSet GetMapChipIndexSetByPosition(const Float3& position);

	uint32_t GetNumBlockVertical() { return kNumBlockVirtical; }

	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }

	// マップチップ番号の位置との当たり判定(バグあり)
	bool IsMapAABB(AABB& charcter,IndexSet& index);

	// マップチップ全体の当たり判定
	bool IsMapAABB(AABB& charcter);

	// マップチップ番号のY座標の押し戻し処理(バグあり)
	void IsMapY(float& posY,float radY,IndexSet& index);

	// マップチップ全体のAABBと当たり判定をとり当たったAABBとY座標の押し戻し処理(isJampがtrueでonGround_がfalse)
	void IsMapY(AABB& charcter,float& posY,float radY);
	// マップチップ全体のAABBと当たり判定をとり当たったAABBとY座標の固定処理(onGround_がtrue)のとき
	void IsMapY2(AABB& charcter,float& posY,float radY);

	void CheckCollision_Collider(Collider* collider);

	// 衝突時にウェーブを発生させるための関数(マップ番号、マップ番号、ウェーブ範囲、Y軸の速度)
	void TriggerWave(int hitX, int hitZ, float waveRange, float initialYVelocity);


private:

	// モデルデータ
	ModelManager::ModelData model_;

	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 20;

	//
	MapChipData mapChipData_;

	// ブロック用のワールドトランスフォームをユニークポインタで管理。
	// 複数並べるために配列にする
	std::vector<std::vector<std::unique_ptr<MapObject>>> mapWorld_;

	//AABBの半径
	Float3 rad_ = {0.5f,0.5f,0.5f};


	// マップのウェーブ範囲
	float maxWaveRange_; //MAX
	float minWaveRange_; //MIN
	// マップのY軸速度
	float maxVelocityY_; //MAX
	float minVelocityY_; //MIN


public:


private:
	// Instancing用オブジェクト
	InstancedObject mapObjIns_;

	void InitInstancing();
	void UpdateInstancedObjects();
};


