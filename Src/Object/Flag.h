#pragma once

class Flag
{

public:

	Flag(void);							//コンストラクタ
	~Flag(void);						//デストラクタ

	void Init(void);												//初期化処理
	void Update(const VECTOR& playerPos, bool allEnemyDefeated);	//更新処理
	void Draw(void);												//描画処理

	bool IsGameClear() const;

	VECTOR GetPosition() const;

private:

	//円のデバッグ
	void DrawCircleOnMap(VECTOR center, float radius, int color);
	
	void DrawGauge3D(VECTOR center, float gaugeRate);

	int modelIdB_;		//モデルの格納(苗木)
	bool flagAlive_;	//生存判定

	VECTOR scl_;	//おおきさ
	VECTOR pos_;	//位置
	VECTOR rot_;	//回転
	VECTOR dir_;	//移動用

	bool circleVisible_;

	bool flagVisible_;

	bool gameClear_;

	float clearGauge_;

	float clearGaugeMax_;

	float flagRadius_;
};