#include "Chs.h"
#include <unordered_map>
static const std::unordered_map<const char*, const char*> ChsList = {
	{"pcons",u8"�Զ���ҩ"}
};

inline const char* GetChsName(const char* ChsName) {
	return ChsList.at(ChsName);
}