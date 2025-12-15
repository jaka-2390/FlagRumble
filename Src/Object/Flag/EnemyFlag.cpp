#include "../../Manager/SceneManager.h"
#include "EnemyFlag.h"

EnemyFlag::EnemyFlag(VECTOR pos, ENEMY_TYPE type, STATE state) : FlagBase(pos, type, state)/*, spawnTimer_(0.0f)*/
{
}

void EnemyFlag::Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies)
{
    //‹¤’Êˆ—
    FlagBase::Update(playerPos, enemies);

    //“G‚ð5•b‚²‚Æ‚É‚Ð‚Æ‚è¶¬
    //spawnTimer_ += scnMng_.GetDeltaTime();  //Œo‰ß•b‚ðŽæ“¾

    //if (spawnTimer_ >= 8.0f)
    //{
    //    spawnTimer_ = 0.0f;

    //    enemySpawned_ = true;
    //}
}
