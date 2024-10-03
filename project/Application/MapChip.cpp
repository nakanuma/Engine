#include "MapChip.h"

void MapChip::Initialize()
{
}

Float3 MapChip::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex)
{

	return Float3(kBlockWidth * xIndex,0 ,kBlockHeight * (kNumBlockVirtical - 1 - yIndex));
}
