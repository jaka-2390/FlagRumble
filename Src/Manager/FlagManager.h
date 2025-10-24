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

    bool AllFlagsCleared() const;

private:

    std::vector<std::unique_ptr<Flag>> flags_;

};