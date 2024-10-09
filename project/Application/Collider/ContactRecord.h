#pragma once
#include<vector>
#include <algorithm> // std::any_of を使用するために必要

/// <summary>
/// 接触記録
/// </summary>
class ContactRecord 
{
public:
	// 履歴追加
	void AddHistory(uint32_t number);
	// 履歴チェック
	bool CheckHistory(uint32_t number);

	// 履歴抹消
	void Clear();

private:
// 履歴
	std::vector<uint32_t> history_;


};
