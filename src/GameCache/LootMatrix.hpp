#ifndef __GAMECACHE__LOOTMATRIX_HPP__
#define __GAMECACHE__LOOTMATRIX_HPP__

#include "Interface/FastDatabase.hpp"
#include "Utils/Logger.hpp"
using namespace GameCache::Interface;
extern FDB::Connection Cache;

namespace CacheLootMatrix {
	inline FDB::RowInfo getRow(int32_t lootMatrixIndex) {
		FDB::RowTopHeader rth = Cache.getRows("LootMatrix");
		for (int i = 0; i < rth.getRowCount(); ++i) {
			try {
				if (!rth.isValid(i)) continue;
				FDB::RowInfo rowInfo = rth[i];
				if (rowInfo.isValid())
					if (*reinterpret_cast<int32_t*>(rowInfo[0].getMemoryLocation()) == lootMatrixIndex)
						return rth[i];
			}
			catch (std::runtime_error e) {
				Logger::log("Cache:LootMatrix", e.what(), LogType::ERR);
			}
		}
		return FDB::RowInfo();
	}

	inline int32_t GetLootTableIndex(FDB::RowInfo row) {
		return *reinterpret_cast<int32_t*>(row/**/[1]/**/.getMemoryLocation());
	}

	inline int32_t GetRarityTableIndex(FDB::RowInfo row) {
		return *reinterpret_cast<int32_t*>(row/**/[2]/**/.getMemoryLocation());
	}

	inline float GetPercent(FDB::RowInfo row) {
		return *reinterpret_cast<float*>(row/**/[3]/**/.getMemoryLocation());
	}

	inline int32_t GetMinToDrop(FDB::RowInfo row) {
		return *reinterpret_cast<int32_t*>(row/**/[4]/**/.getMemoryLocation());
	}

	inline int32_t GetMaxToDrop(FDB::RowInfo row) {
		return *reinterpret_cast<int32_t*>(row/**/[5]/**/.getMemoryLocation());
	}

	inline int32_t GetID(FDB::RowInfo row) {
		return *reinterpret_cast<int32_t*>(row/**/[6]/**/.getMemoryLocation());
	}

	inline int32_t GetFlagID(FDB::RowInfo row) {
		return *reinterpret_cast<int32_t*>(row/**/[7]/**/.getMemoryLocation());
	}

	inline FDB::PointerString GetGateVersion(FDB::RowInfo row) {
		return FDB::PointerString(&Cache, row/**/[8]/**/.getMemoryLocation());
	}
};

#endif // !__GAMECACHE__LOOTMATRIX_HPP__
