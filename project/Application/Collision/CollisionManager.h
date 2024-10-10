#pragma once

#include <list>

class Collider;

class CollisionManager
{
public:
	CollisionManager() = default;
	~CollisionManager() = default;

	void Update();
private:
	void CheckCollisionPair(Collider* colliderA,Collider* colliderB);
	//void CheckCollisionAABBPair(Collider* colliderA,Collider* colliderB);
private:
	std::list<Collider*> colliders_;
public:
	void Reset();
	void AddCollider(Collider* collider){
		if(collider != nullptr)
		{
			colliders_.push_back(collider);
		}
	}
};