#pragma once

#include <memory>
#include <list>

#include <stdint.h>

#include "ModelManager.h"
#include "Object3D.h"

#include "Float2.h"
#include "Float3.h"

class Collider;
class Stage;
class Enemy
{
public:
	Enemy();
	~Enemy();

	void Initialize(Float3 spawnPos,Float2 moveDirection,ModelManager::ModelData* modelData);
	void Update(std::list<std::unique_ptr<Enemy>>& enemies);
	void Draw();
private:
	void CloneInitialize(Float3 spawnPos,Float2 moveDirection,ModelManager::ModelData* modelData,int32_t numberOfClones2Create);
	Enemy* CreateClone();
private:
	Stage* stage_;
	// 奪うエネルギー
	float stealEnergy_;
	// 奪ったエネルギー
	float stolenEnergy_;

	bool isAlive_;
	std::unique_ptr<Object3D> object_;
	std::unique_ptr<Collider> collider_;

	float scalePerStolenEnergy_;

	Float2 moveDirection_;
	float speed_;
	Float3 velocity_;

	// Wave / Jump
	float mapObjectWaveDistance_;
	float waveRange_;

	float minJumpPower_;
	float maxJumpPower_;
	
	bool onWavingMapChip_;
	bool isOnGround_;
	bool preOnGround_;

	// Clone
	bool isHurt_;
	bool isClone_;
	int32_t numberOfClones2Create_;
	Float3 cloneOffset_;
public:
	void SetStage(Stage* stage){stage_ = stage;}
	bool IsAlive()const{return isAlive_;}
	bool IsHurt()const{return isHurt_;}

	Collider* GetCollider()const { return collider_.get(); }
// nakanuma追加分
public:
	// 着地した瞬間を取得
	bool GetLanding() { return !preOnGround_ && isOnGround_; }
	// 位置を取得
	Float3 GetTranslate() { return object_->transform_.translate; }

	// クローンを生成したフレームのみ立てるフラグ（分裂時パーティクル発生のトリガーに使用）
	bool isCloneThisFrame_ = false;
	// クローンを生成した瞬間を取得
	bool GetIsCloneThisFrame() { return isCloneThisFrame_; }
	void SetIsCloneThisFrame(bool flag) { isCloneThisFrame_ = flag; }
};