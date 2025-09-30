#pragma once

class Flag
{

public:

	Flag(void);							//コンストラクタ
	~Flag(void);						//デストラクタ

	void Init(void);					//初期化処理
	void Update(void);					//更新処理
	void Draw(void);					//描画処理

	// 位置
	VECTOR GetPosition() const;

	//円のデバッグ
	void DrawCircleOnMap(VECTOR center, float radius, int color);

private:

	int modelIdB_;	//モデルの格納(苗木)

	VECTOR scl_;	//おおきさ
	VECTOR pos_;	//位置
	VECTOR rot_;	//廻天
	VECTOR dir_;	//移動せんよ
};