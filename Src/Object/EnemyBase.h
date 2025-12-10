#pragma once
#include <memory>
#include<map>
#include <functional>
#include <vector>
#include "ActorBase.h"
#include "Item.h" 

class AnimationController;
class GameScene;
class DemoScene;
class Player;

class EnemyBase : public ActorBase
{

public:

	//ダメージ関連
	static constexpr int DAMAGE_CNT = 120;
	static constexpr int DAMAGE_POS = 100;
	static constexpr float DAMAGE_IMG_SCL = 0.5f;
	static constexpr int VALUE_DAMAGE_1 = 1;
	static constexpr int VALUE_DAMAGE_2 = 2;
	static constexpr int VALUE_DAMAGE_4 = 4;
	static constexpr int VALUE_DAMAGE_8 = 8;
	static constexpr int VALUE_DAMAGE_16 = 16;
	static constexpr int VALUE_DAMAGE_32 = 32;
	static constexpr int VALUE_DAMAGE_64 = 64;

	//デバッグ &　汎用
	static constexpr float VALUE_ZERO = 0.0f;	//初期化の値用
	static constexpr float VALUE_ONE = 1.0f;
	static constexpr float VALUE_TWO = 2.0f;
	static constexpr int  VALUE_SIXTY = 60;

	//向き
	static constexpr float DEGREE = 180.0f;

	//敵の範囲用
	static constexpr float VIEW_RANGE = 500.0f;	// 視野の広さ
	static constexpr float VIEW_ANGLE = 15.0f;	// 視野角

	//敵の共通用球体座標
	static constexpr float COLLOSION_RADIUS = 100.0f;
	static constexpr VECTOR COLLISION_POS = { 0.0f, 60.0f, 0.0f };

	//攻撃の位置オフセット
	static constexpr float ATTACK_FORWARD_OFFSET = 55.0f;	// 前方向
	static constexpr float ATTACK_HEIGHT_OFFSET = 100.0f;	// 高さ
	static constexpr float ATTACK_RADIUS_SIZE = 35.0f;	// 攻撃範囲

	//ドロップアイテムのサイズと距離しきい値
	static constexpr float DROP_SCALE_SMALL = 0.1f;
	static constexpr float DROP_SCALE_MEDIUM = 0.15f;
	static constexpr float DROP_SCALE_LARGE = 0.2f;
	static constexpr float DROP_DISTANCE_MEDIUM = 3000.0f;
	static constexpr float DROP_DISTANCE_LARGE = 6000.0f;

	//ランダムの数値
	const int RANDOM_VALUE = 1;

	//原点復帰時の最小移動距離（移動制御）
	static constexpr float MIN_MOVE_DISTANCE = 0.01f;

	//アニメーション関係
	static constexpr float ANIM_SPEED = 20.0f;

	//アニメーション番号
	static constexpr int   ANIM_IDLE_INDEX = 0;
	static constexpr int   ANIM_RUN_INDEX = 1;
	static constexpr int   ANIM_ATTACK_INDEX = 2;
	static constexpr int   ANIM_DAMAGE_INDEX = 3;
	static constexpr int   ANIM_DEATH_INDEX = 4;

	static constexpr float WANDER_CHANGE_TIME = 2.0f;		//徘徊時の方向変更間隔(秒)
	static constexpr float WANDER_SPEED_SCALE = 0.3f;		//徘徊時の移動速度倍率
	static constexpr float FRAME_DELTATIME = 1.0f / 60.0f;	//タイマーのフレーム基準(60fps想定)
	static constexpr float MAX_RANGE = 500.0f;				//徘徊の最大範囲

	//色
	int white = 0xffffff; //白
	int black = 0x000000; //黒
	int red = 0xff0000;	  //赤
	int green = 0x00ff00; //緑
	int blue = 0x0000ff;  //青
	int yellow = 0xffff00;//黄
	int purpl = 0x800080; //紫

	//敵の種類
	enum class TYPE
	{
		DOG,	//犬
		SABO,	//サボテン
		MIMIC,	//ミミック
		MUSH,	//キノコ
		ONION,	//玉ねぎ
		TOGE,	//トゲゾー
		VIRUS,	//ウィルス
		BOSS,	//ボス
		MAX
	};

	//状態管理
	enum class STATE
	{
		NONE,
		IDLE,
		PLAY,
		ATTACK,
		DAMAGE,
		DEATH,
		MAX
	};

	//アニメーション種別
	enum class ANIM_TYPE
	{
		NONE,
		IDLE,
		RUN,
		ATTACK,
		KICK,
		DAMAGE,
		DEATH,
		DEFENSE,
		DEFENSE_HIT,
		MAX
	};

	EnemyBase();	//コンストラクタ
	virtual ~EnemyBase(void);	//デストラクタ

	virtual void Init(void);			//初期処理(最初の１回のみ実行)
	virtual void InitAnimation(void) {};//アニメーションロード用
	virtual void SetParam(void) {};		//パラメータ設定(純粋仮想関数)
	virtual void Update(void);			//更新処理(毎フレーム実行)
	virtual void Draw(void);			//描画処理(毎フレーム実行)
	virtual void DrawBossHpBar(void) {};	//ボスのHPバー
	virtual void Release(void);			//解放処理(最後の１回のみ実行)
	virtual void ChasePlayer(void);		//プレイヤーを追いかける

	void SetPos(VECTOR pos);	//座標の設定
	STATE GetState(void);		//状態獲得

	bool IsAlive(void);			//生存判定
	void SetAlive(bool alive);	//生存判定

	void Damage(int damage);	//ダメージを受ける


	TYPE GetEnemyType(void) const;//敵のタイプを取得
	TYPE enemyType_;	//敵のタイプ

	// 死亡時のドロップアイテムを決める関数（デフォルトはNONE）
	//virtual Item::TYPE GetDropItemType(void) const;

	void SetCollisionPos(const VECTOR collision);//衝突判定用の球体
	VECTOR GetCollisionPos(void)const;	//衝突用の中心座標の取得
	float GetCollisionRadius(void);		//衝突用の球体半径の取得

	void SetGameScene(GameScene* scene);
	
	void SetDemoScene(DemoScene* demoScene);

	void DrawDebug(void);	//デバッグ用
	void DrawDebugSearchRange(void); //視野内に入ったか見る

	void SetPlayer(std::shared_ptr<Player> player);

protected:

	//ダメージ描画判定
	bool is1damage_;
	bool is2damage_;
	bool is4damage;
	bool is8damage;
	bool is16damage;
	bool is32damage;
	bool is64damage;

	//ダメージ画像
	int damage1img_;
	int damage2img_;
	int damage4img_;
	int damage8img_;
	int damage16img_;
	int damage32img_;
	int damage64img_;

	int damageCnt_;

	std::shared_ptr<Player> player_;
	GameScene* scene_;
	DemoScene* demoScene_;

	//アニメーション
	std::unique_ptr<AnimationController> animationController_;

	std::map<STATE, std::function<void(void)>> stateChanges_;//状態管理(状態遷移時初期処理)
	std::function<void(void)> stateUpdate_;					 //状態管理(更新ステップ)

	float speed_;	//移動速度

	VECTOR movePow_;	//移動量
	VECTOR movedPos_;	//移動後の座標
	VECTOR moveDiff_;	//フレームごとの移動値
	VECTOR collisionPos_;		//赤い球体の移動後座標
	VECTOR attackCollisionPos_; //紫の球体の移動後座標

	int hp_;	// 体力
	int maxHp_;	//最大体力
	int attackPow_; //攻撃力

	bool isAlive_;	//生存判定
	bool isAttack_ = false;		//攻撃判定
	bool isAttack_P = false;	//攻撃判定
	bool isAttack_T = false;	//攻撃判定

	STATE state_;	//状態管理
	ANIM_TYPE animtype_; //アニメーションのタイプ確認用

	float collisionRadius_;		//衝突判定用の球体半径
	VECTOR collisionLocalPos_;	//衝突判定用の球体中心の調整座標

	float attackCollisionRadius_;	 //攻撃判定用と攻撃範囲の球体半径
	VECTOR attackCollisionLocalPos_; //攻撃判定用と攻撃範囲の調整座標

	VECTOR playerCenter_;	//プレイヤーの球体の座標を取得
	float playerRadius_;	//プレイヤーの球体の半径を取得
	VECTOR p_Diff_;			//プレイヤーの位置差分
	float p_Dis_;			//プレイヤーまでの距離
	float p_RadiusSum_;		//プレイヤーとの衝突半径の合計
	bool encounter_;		//プレイヤーと接敵

	VECTOR startPos_;							//出現時の位置
	float changeDirTimer_ = 0.0f;				//向き変更タイマー
	VECTOR wanderDir_ = { 1.0f, 0.0f, 0.0f };	//現在の移動方向

	//更新系
	void UpdateNone(void) {};		//更新ステップ
	virtual void UpdateIdle(void);	//待機状態の更新
	virtual void UpdatePlay(void);	//移動時の更新処理
	virtual void UpdateAttack(void);//アタック時の更新処理
	virtual void UpdateDamage(void);//ダメージ時の更新処理
	virtual void UpdateDeath(void);	//死んだ時の更新処理

	void DrawDamage();	//ダメージ画像の描画
	void DrawHpGauge3D(VECTOR center, float gaugeRate);	//HPバー

	//攻撃関係
	void AttackCollisionPos(void);	//攻撃用関数
	void EnemyToPlayer(void);		//敵がプレイヤー攻撃
	void CheckHitAttackHit(void);	//攻撃が当たったか確認する

	//状態遷移
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStateIdle(void);
	void ChangeStatePlay(void);
	void ChangeStateAttack(void);
	void ChangeStateDamage(void);
	void ChangeStateDeath(void);

	void Collision(void);	//衝突判定

};