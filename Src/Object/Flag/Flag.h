#pragma once
#include "../FlagBase.h"

class Flag : public FlagBase
{

public:

	Flag(VECTOR pos);							//コンストラクタ
	~Flag(void);						//デストラクタ

	void Init(void);												//初期化処理
	void Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies);	//更新処理
	void Draw(void);												//描画処理

	bool IsFlagClear() const;

	VECTOR GetPosition() const;

	bool SpawnEnemies(const VECTOR& playerPos) const; // 敵を出すべきか判定

	void SetEnemySpawned(bool spawned); // 出現済みをセット

private:

	//旗を立てる処理
	void CheckCircle(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies);

	//円との距離
	float DistanceSqXZ(const VECTOR& a, const VECTOR& b) const;

	//円のデバッグ
	void DrawCircleOnMap(VECTOR center, float radius, int color);
	
	void DrawGauge3D(VECTOR center, float gaugeRate);

	int modelIdB_;		//モデルの格納(苗木)

	VECTOR scl_;	//おおきさ
	VECTOR pos_;	//位置
	VECTOR rot_;	//回転
	VECTOR dir_;	//移動用

	bool flagVisible_;		//flagの描画
	bool flagClear_;		//奪還したか
	bool enemySpawned_;		//敵をすでに出したか
	bool playerInRange_;	//プレイヤーが円内にいるか
	bool enemyNear_;		//敵がflagの近くにいるか

	float clearGauge_;
	float clearGaugeMax_;
	float flagRadius_;
	float enemyCheckRadius_;
};