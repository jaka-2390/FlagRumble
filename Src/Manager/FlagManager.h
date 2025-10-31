#pragma once
#include <vector>
#include <memory>
#include "..//Object/Flag/Flag.h"

class FlagManager 
{

public:

    void Init();
    void Update(const VECTOR& playerPos, bool allEnemyDefeated);
    void Draw();

    VECTOR GetFlagPosition(int index) const;

    bool AllFlagsCleared() const;

    int GetClearedFlagCount() const;

    std::vector<int> GetSpawnFlag(const VECTOR& playerPos); // 敵を出す旗のインデックスを返す

private:

    std::vector<std::unique_ptr<Flag>> flags_;

    int nextFlagIndex_ = 0;
};