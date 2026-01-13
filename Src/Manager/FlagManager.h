#pragma once
#include <vector>
#include <memory>
#include "..//Object/Flag/Flag.h"
#include "..//Object/Flag/PlayerFlag.h"
#include "..//Object/Flag/EnemyFlag.h"
#include "..//Object/FlagBase.h"

class FlagManager 
{

public:

    static constexpr VECTOR FLAG1_POS = { -2000.0f, 254.0f, 2000.0f };
    static constexpr VECTOR FLAG2_POS = { -250.0f, 254.0f, 4000.0f };
    static constexpr VECTOR FLAG3_POS = { 2300.0f, 254.0f, 2000.0f };
    static constexpr VECTOR FLAG4_POS = { -250.0f, 254.0f, 1000.0f };

    static constexpr VECTOR PLAYER_FLAG_POS = { -2500.0f, 254.0f, 0.0f };
    static constexpr VECTOR ENEMY_FLAG_POS = { 2500.0f, 254.0f, 4700.0f };

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