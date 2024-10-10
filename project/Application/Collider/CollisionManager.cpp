#include "CollisionManager.h"
//#include"GlobalVariables.h"

void CollisionManager::Reset() {
	// リストを空っぽにする
	colliders_.clear();
}

CollisionManager::~CollisionManager() {

	
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {

	// コライダーAの座標を取得
	Float3 posA = colliderA->GetCenterPosition();
	// コライダーBの座標を取得
	Float3 posB = colliderB->GetCenterPosition();
	// 座標の差分ベクトル
	Float3 subtract = Subtract(posB, posA);
	// 座標AとBの距離を求める
	float distance = Length(subtract);
	if (distance <= colliderA->GetRadius() + colliderB->GetRadius()) {
		// コライダーAの衝突判時コールバックを呼び出す
		colliderA->OnCollision(colliderB);
		// コライダーBの衝突判時コールバックを呼び出す
		colliderB->OnCollision(colliderA);
	}
}

void CollisionManager::CheckCollisionAABBPair(Collider* colliderA, Collider* colliderB) {

	// コライダーAの座標を取得
	Float3 posA = colliderA->GetCenterPosition();
	// コライダーBの座標を取得
	Float3 posB = colliderB->GetCenterPosition();


	// AABBに使用するための各軸の半径取得
	Float3  collARadAABB = colliderA->GetAABBRadius();

	// AABBにしようするたの各軸の半径取得
	Float3 collBRadAABB = colliderB->GetAABBRadius();

	// colliderAのAABB
	AABB aabbA;
	aabbA.max = Add(posA, collARadAABB);
	aabbA.min = Subtract(posA, collARadAABB);
	
	// AABBの情報を知るために
	colliderA->SetAABB(aabbA);

	// colliderBのAABB
	AABB aabbB;
	aabbB.max = Add(posB, collBRadAABB);
	aabbB.min = Subtract(posB, collBRadAABB);

	// AABBの情報を知るために
	colliderB->SetAABB(aabbB);
	
	// AABBでの衝突判定
	if (IsCollision(aabbA,aabbB)) {
		// コライダーAの衝突判時コールバックを呼び出す
		colliderA->OnCollision(colliderB);
		// コライダーBの衝突判時コールバックを呼び出す
		colliderB->OnCollision(colliderA);
	}
}

void CollisionManager::CheckAllCollisions() {
	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;

		// イテレータBはi\イテレータAの次の要素から回す(重複判定を回避)
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;

			// ペアの当たり判定
			CheckCollisionAABBPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::AddCollider(Collider* collider) {
	// NULLポインタでないことを確認
	if (collider != nullptr) {
		colliders_.push_back(collider);
	}
}

void CollisionManager::Initialize() {

	
	

	//GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	//const char* gropName = "CollisionManager";
	//// グループを追加する
	//GlobalVariables::GetInstance()->CreateGroup(gropName);
	//globalVariables->AddItem(gropName, "isCollider", isCollider);
}

void CollisionManager::UpdateWorldTransform() {

	//ApplyGlobalVariables();

	// 非表示なら抜ける
	if (isCollider == false) {
		return;
	}

	// 全てのコライダーについて
	for (Collider* collider : colliders_) {
		//更新
		collider->UpdateWorldTransform();
	}

}

void CollisionManager::Draw() {

	// 非表示なら抜ける
	if (isCollider == false) {
		return;
	}
	

	// 全てのコライダーについて
	for (Collider* collider : colliders_) {
		// 更新
		collider->Draw();
	}

}

//void CollisionManager::ApplyGlobalVariables() {
//
//	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
//	const char* gropName = "CollisionManager";
//	// グループを追加する
//	GlobalVariables::GetInstance()->CreateGroup(gropName);
//	isCollider = globalVariables->GetBoolValue(gropName, "isCollider");
//
//}
