#pragma once
#include <vector>
#include <memory>
#include "..//Object/Flag/Flag.h"
#include "..//Object/Flag/PlayerFlag.h"
#include "..//Object/FlagBase.h"

class FlagManager 
{

public:

    void Init();
    void Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies);
    void Draw();

    VECTOR GetFlagPosition(int index) const;

    FlagBase* GetFlag(int index) const;

    int GetClearedFlagCount() const;

    std::vector<int> GetSpawnFlag(const VECTOR& playerPos); // 敵を出す旗のインデックスを返す

    int GetFlagMax() const;

    std::vector<FlagBase*> GetPlayerFlags() const;

    //チュートリアル用flag
    void Clear();
    void AddFlag(const VECTOR pos, Flag::ENEMY_TYPE type, Flag::STATE state);

private:

    std::vector<std::unique_ptr<FlagBase>> flags_;

    int nextFlagIndex_ = 0;

    int flagMax_ = 0;
};