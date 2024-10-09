#include "ContactRecord.h"

void ContactRecord::AddHistory(uint32_t number) 
{

history_.push_back(number);

}

bool ContactRecord::CheckHistory(uint32_t number) 
{
	return std::any_of(history_.begin(), history_.end(), [number](uint32_t n) { return n == number; });
}

void ContactRecord::Clear() { history_.clear();}
