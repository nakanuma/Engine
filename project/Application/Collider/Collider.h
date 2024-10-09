#pragma once
#include"Object3D.h"
#include"CollisionTypeIdDef.h"
#include"MyMath.h"
#include"Float3.h"

class Collider {
public:

	// 初期化
	void Initialize();

	// ワールドトランスフォームの初期化
	void UpdateWorldTransform();
	// 描画
	void Draw();

	// 半径取得
	float GetRadius() { return radius_; }
	// 半径設定
	void SetRadius(float radius) { radius_ = radius; }

	// AABB半径取得
	Float3 GetAABBRadius() { return aabbRadius_; }
	// AABB半径設定
	void SetAABBRabius(Float3 aabbRad) { aabbRadius_ = aabbRad; }
	// AABBの取得
	AABB GetAABB() { return aabb_; }
	// AABBの設定(確認用)です
	void SetAABB(AABB aabb) { aabb_ = aabb; }

	// 衝突時に呼ばれる関数
	virtual void OnCollision([[maybe_unused]] Collider* other){};

	virtual Float3 GetCenterPosition() const = 0;

	virtual ~Collider() = default; 

	// 種別IDの取得
	uint32_t GetTypeID() const { return typeID_; };
	// 種別IDの設定
	void SetTypeID(uint32_t typeID);
	// オブジェクトスケール
	void SetScale(Float3 scale) { object.transform_.scale = scale; }

private:

	// 衝突モデル
	ModelManager::ModelData modelCollosion_;

	// AABB用の半径
	Float3 aabbRadius_ = {0.5f, 0.5f, 0.5f};

	//AABB
	AABB aabb_{};

	// 球用衝突判定
	float radius_ = 1.5f;
	
	//コライダーの数
	int count = 0;

	// オブジェクト3D
	Object3D object;

	// 種別ID
	uint32_t typeID_ = 0u;

};

