//#pragma once
//#include <memory>
//#include "ActorBase.h"
#include"../Common/Vector2.h"

class GameScene;
class SceneManager;
class Player;

class Tree
{
public:

	static constexpr float ZERO = 0.0f;				//初期化の値用
	static constexpr int GROW_UP = 4;				//成長段階
	static constexpr int INVINCIBLE_TIME = 600;		//無敵

	// ヘッダーかファイル冒頭で定数定義（必要に応じて static const や constexpr にしてもOK）
	static constexpr float TREE_GROW_RADIUS_INCREMENT = 10.0f;
	static constexpr float ENEMY_SAFE_MARGIN = 30.0f;
	static constexpr float MIN_DISTANCE_THRESHOLD = 0.001f;

	//成長度合い
	enum class GROW
	{
		BABY,
		KID,
		ADULT,
		OLD,
		MAX
	};

	Tree(void);							//コンストラクタ
	~Tree(void);						//デストラクタ

	bool Init(void);					//初期化処理
	void Update(void);					//更新処理
	void Draw(void);					//描画処理
	void DrawDebug(void);				//デバッグ用
	void DrawDebugTree2Player(void);	//対プレイヤー

	int GetHp(void);//体力
	int GetLv(void);//レベル

	void LvUp(void);		//レベルを上げる
	void ChangeGrow(void);	//成長させる

	void SetPlayer(Player* player);					//プレイヤーのポインタをセットする関数

	//const Capsule& GetCapsule(void) const;		//衝突用カプセルの取得
	//void SetCollisionPos(const VECTOR collision);	//衝突判定用の球体
	VECTOR GetCollisionPos(void)const;				//衝突用の中心座標の取得
	float GetCollisionRadius(void);					//衝突用の球体半径の取得
	float collisionRadius_;							//衝突判定用の球体半径
	VECTOR collisionLocalPos_;						//衝突判定用の球体中心の調整座標
	VECTOR collisionPos_;
	
	void Muteki(void);	//無敵化
	void eHit(void);	//エネミー達との判定
	void pHit(void);	//プレイヤーとの判定
private:
	float viewRange_ = 100.0f;	//円の半径（任意の値）
	int circleSegments_ = 120;  //円の分割数

	Player* player_;
	GameScene* gameScene_;
	SceneManager* sceneManager_;

	int modelIdB_;	//モデルの格納(苗木)
	int modelIdK_;	//モデルの格納(幼木)
	int modelIdA_;	//モデルの格納(成木)
	int modelIdO_;	//モデルの格納(老木)
	VECTOR scl_;	//おおきさ
	VECTOR pos_;	//位置
	VECTOR rot_;	//廻天
	VECTOR dir_;	//移動せんよ

	int imgMutekiIcon_;	//無敵アイコン

	int lv_;
	int hp_;
	int water_;
	bool isAlive_;
	bool isD_;
	GROW grow_;

	// 無敵状態
	bool invincible_;
	int mutekiCnt_;

	//エフェクト
	int effectTreeResId_;
	int effectTreePlayId_;

	void PushEnemy(void);		//木にめり込んだら押し出す
	void MutekiTimer(void);		//無敵時間
	void EffectTreeRange(void);	//木の視野範囲
};