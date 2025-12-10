#pragma once
#include "../FlagBase.h"

class Flag : public FlagBase
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

	Flag(VECTOR pos, ENEMY_TYPE type, STATE state);	//コンストラクタ
	~Flag(void);						//デストラクタ

	void Init(void);												//初期化処理
	void Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies);	//更新処理
	void Draw(void);												//描画処理

	VECTOR GetPosition() const;

	ENEMY_TYPE GetEnemyType() const { return enemyType_; }

	bool SpawnEnemies(const VECTOR& playerPos) const; // 敵を出すべきか判定

	void SetEnemySpawned(bool spawned); // 出現済みをセット

	void SetState(STATE state) { state_ = state; }
	STATE GetState() const { return state_; }

	bool IsOwnedByPlayer() const { return state_ == STATE::PLAYER; }
	bool IsOwnedByEnemy() const { return state_ == STATE::ENEMY; }

private:

	//旗を立てる処理
	void CheckCircle(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies);

	//円との距離
	float DistanceSqXZ(const VECTOR& a, const VECTOR& b) const;

	//エリアエフェクト
	void EffectAreaRange(void);

	STATE state_;
	ENEMY_TYPE enemyType_;

	Transform flag_;	//旗のモデル

	VECTOR scl_;	//おおきさ
	VECTOR pos_;	//位置
	VECTOR rot_;	//回転
	VECTOR dir_;	//移動用

	bool enemySpawned_;		//敵をすでに出したか
	bool playerInRange_;	//プレイヤーが円内にいるか
	bool enemyNear_;		//敵がflagの近くにいるか

	float clearGauge_;
	float clearGaugeMax_;
	float flagRadius_;
	float enemyCheckRadius_;

	//エフェクト
	int effectEnemyAreaResId_;
	int effectEnemyAreaPlayId_;
	
	int effectPlayerAreaResId_;
	int effectPlayerAreaPlayId_;
	
	// リソース管理
	ResourceManager& resMng_;

};