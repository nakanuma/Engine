#include "MyRandom.h"

std::mt19937& GenerateRandomMT(){
	static std::mt19937 sGen;
	return sGen;
}


void MyRandom::StaticInitialize(){
	std::random_device device{};
	GenerateRandomMT().seed(device());
}

float MyRandom::operator()(){
#ifdef _DEBUG
	if(isDebug){
		dist.param(std::uniform_real_distribution<float>::param_type(min_,max_));
	}
#endif // _DEBUG

	return dist(GenerateRandomMT());
}