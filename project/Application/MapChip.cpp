#include "MapChip.h"


namespace {

	std::map<std::string, MapChipType> mapChipTable = {
	{"0", MapChipType::kBlank},
	{"1", MapChipType::kBlock},
	};
}

void MapChipField::ResetMapChipData() {

	// マップチップデータをリセット
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}
//読み込み
void MapChipField::LoadMapChipCsv(const std::string& filePath) {

	// マップチップデータをリセット
	ResetMapChipData();

	//ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	//マップチップCSV
	std::stringstream mapChipCsv;
	//ファイルの内容を文字列にストリームにコピー
	mapChipCsv << file.rdbuf();
	//ファイルを閉じる
	file.close();


	// CSVからマップチップデータを読み込む
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {

		std::string line;
		getline(mapChipCsv, line);

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {

			std::string word;
			getline(line_stream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}

	}

}

//マップチップ種別の取得
MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t zIndex) {


	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (zIndex < 0 || kNumBlockVirtical - 1 < zIndex) {
		return MapChipType::kBlank;
	}

	return mapChipData_.data[zIndex][xIndex];
}

//マップチップ座標の取得
//縦横の指定してその位置のマップチップワールド座標を取得する
Float3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {

	return Float3(kBlockWidth * xIndex,0 ,kBlockHeight * (kNumBlockVirtical - 1 - yIndex));

}

//マップチップ番号を計算
MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Float3& position) {
	MapChipField::IndexSet indexSet = {};
	indexSet.xIndex = uint32_t((position.x + kBlockWidth / 2) / kBlockWidth);
	indexSet.zIndex = uint32_t(kNumBlockVirtical - 1 - (int(position.z + kBlockHeight / 2.0f) / kBlockHeight));

	return indexSet;
}

//ブロックの範囲取得関数
MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t zIndex) {
	// マップチップ座標の取得
	Float3 center = GetMapChipPositionByIndex(xIndex, zIndex);

	//各頂点
	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.top = center.z - kBlockHeight / 2.0f;
	rect.bottom = center.z + kBlockHeight / 2.0f;


	return rect;

}
