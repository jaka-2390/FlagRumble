#pragma once
#include <DxLib.h>
#include "EnemyBase.h"

class FlagBase 
{

public:

    enum class ENEMY_TYPE
    {
        NONE,
        DOG,
        SABO,
        BOSS
    };

    enum class STATE
    {
        PLAYER,     // プレイヤーが所有
        ENEMY,      // 敵が所有
        NEUTRAL
    };
    
    FlagBase(VECTOR pos, ENEMY_TYPE type, STATE state);
    virtual ~FlagBase() {}

    virtual void Init();
    virtual void Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies);
    virtual void Draw();

    VECTOR GetPosition() const;

    ENEMY_TYPE GetEnemyType() const { return enemyType_; }

    void SetState(STATE state) { state_ = state; }
    STATE GetState() const { return state_; }

    bool SpawnEnemies(const VECTOR& playerPos) const; // 敵を出すべきか判定
    void SetEnemySpawned(bool spawned); // 出現済みをセット

    bool IsEnemySpawned() const { return enemySpawned_; }

    bool IsOwnedByPlayer() const { return state_ == STATE::PLAYER; }
    bool IsOwnedByEnemy() const { return state_ == STATE::ENEMY; }
    bool IsNeutral() const { return state_ == STATE::NEUTRAL; }

protected:

    SceneManager& scnMng_;

    VECTOR pos_;
    VECTOR scl_;
    VECTOR rot_;

    bool circleVisible_ = false;
    bool flagVisible_ = false;
    float clearGauge_ = 0.0f;
    float clearGaugeMax_ = 100.0f;
    float flagRadius_ = 100.0f;
    float enemyCheckRadius_;

    STATE state_;
    ENEMY_TYPE enemyType_;

    Transform flag_;	//旗のモデル

    bool enemySpawned_;		//敵をすでに出したか
    bool playerInRange_;	//プレイヤーが円内にいるか
    bool enemyNear_;		//敵がflagの近くにいるか

    //エフェクト
    int effectEnemyAreaResId_;
    int effectEnemyAreaPlayId_;

    int effectPlayerAreaResId_;
    int effectPlayerAreaPlayId_;
    
    int effectNeutralAreaResId_;
    int effectNeutralAreaPlayId_;

    //旗を立てる処理
    void CheckCircle(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies);

    void DrawCircleOnMap(VECTOR center, float radius, int color);
    void DrawGauge3D(VECTOR center, float gaugeRate);

    //円との距離
    float DistanceSqXZ(const VECTOR& a, const VECTOR& b) const;

    //エリアエフェクト
    void EffectAreaRange(void);

};