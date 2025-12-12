#pragma once
#include "../FlagBase.h"

class EnemyFlag : public FlagBase
{
public:

    EnemyFlag(VECTOR pos, ENEMY_TYPE type, STATE state);

    void Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies) override;

private:

    float spawnTimer_;

};

