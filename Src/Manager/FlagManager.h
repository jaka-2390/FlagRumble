#pragma once
#include <vector>
#include <memory>
#include "..//Object/Flag/Flag.h"

class FlagManager 
{

public:

    void Init();
    void Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies);
    void Draw();

    VECTOR GetFlagPosition(int index) const;

    int GetClearedFlagCount() const;

    std::vector<int> GetSpawnFlag(const VECTOR& playerPos); // 敵を出す旗のインデックスを返す

    int GetFlagMax() const;

private:

    std::vector<std::unique_ptr<Flag>> flags_;

    int nextFlagIndex_ = 0;

    int flagMax_ = 0;
};