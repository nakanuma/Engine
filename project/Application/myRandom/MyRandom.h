#pragma once

#include <random>

#include <string>

#include "GlobalVariables.h"

std::mt19937& GenerateRandomMT();

class MyRandom{
public:
	MyRandom() = default;
	MyRandom(float _min,float _max):dist(_min,_max){
		isDebug = false;
	};
	MyRandom(const std::string& _scene,const std::string& _group,const std::string& _name){
		min_ = 0.0f;
		max_ = 0.0f;
		GlobalVariables* variables = GlobalVariables::getInstance();
		variables->addValue(_scene,_group,_name + "Min",min_);
		variables->addValue(_scene,_group,_name + "Max",max_);

		scene_ = _scene;
		group_ = _group;
		itemName_ = _name;

		dist = std::uniform_real_distribution<float>(min_,max_);
		isDebug = true;
	};
	~MyRandom(){
		if(isDebug == true)
		{
			GlobalVariables* variables = GlobalVariables::getInstance();
			variables->DestroyItem(scene_,group_,itemName_ + "Min");
			variables->DestroyItem(scene_,group_,itemName_ + "Max");
		}
	};

	static void StaticInitialize();

	float operator()();
private:

	std::uniform_real_distribution<float> dist;
	bool isDebug = false;

	std::string scene_,group_,itemName_;
#ifdef _DEBUG
	float min_,max_;
#endif // _DEBUG
#ifndef _DEBUG
	float min_, max_;
#endif // !_DEBUG

};

using MyRand = MyRandom;
/*
MyRandom 使い方
 MyRandom myRand(min,max);

 myRand(); // min ~ max のランダムな値が返ってくる

 // Debug
 MyRandom debugRand ("Scene","randSpeed",min,max);

 debugRand(); // min ~ max のランダムな値が返ってくる
 GlobalVariables から min と max の 変更 が できる
*/