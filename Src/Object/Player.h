#pragma once
#include <memory>
#include <map>
#include <functional>
#include <DxLib.h>
#include "../Application.h"
#include "EnemyBase.h"

class AnimationController;
class Collider;
class Capsule;

class Player : public ActorBase
{

public:

	//プレイヤー
	static constexpr VECTOR PLAYER_POS = { 0.0f, 300.0f, -600.0f };	//初期位置
	static constexpr VECTOR CAPSULE_TOP = { 0.0f, 110.0f, 0.0f };	//カプセルの頂点
	static constexpr VECTOR CAPSULE_BOTTOM = { 0.0f,  30.0f, 0.0f };//カプセルの足元
	static constexpr float PLAYER_ROT_Y = 180.0f;					//Y軸回転(度数)
	static constexpr float COLLISION_RADIUS = 100.0f;				//衝突判定の半径
	static constexpr float CAPSULE_RADIUS = 20.0f;					//カプセルの半径
	static constexpr float ROT_FORWARD_DEG = 0.0f;					//プレイヤー角度(前)
	static constexpr float ROT_BACK_DEG = 180.0f;					//プレイヤー角度(後ろ)
	static constexpr float ROT_RIGHT_DEG = 90.0f;					//プレイヤー角度(右)
	static constexpr float ROT_LEFT_DEG = -90.0f;					//プレイヤー角度(左)

	//アニメーション関係
	static constexpr  float IDLE_SPEED = 60.0f;
	static constexpr  float ANIM_SPEED = 15.0f;

	//アニメーション番号
	static constexpr int   ANIM_IDLE_INDEX = 1;
	static constexpr int   ANIM_RUN_INDEX = 2;
	static constexpr int   ANIM_FAST_RUN_INDEX = 3;
	static constexpr int   ANIM_SLASHATTACK_INDEX = 4;
	static constexpr int   ANIM_NORMALATTACK_INDEX = 5;
	static constexpr int   ANIM_DOWN_INDEX = 7;
	static constexpr int   ANIM_EXATTACK_INDEX = 8;

	//スピード
	static constexpr float SPEED_MOVE = 5.0f;
	static constexpr float SPEED_RUN = 10.0f;

	//回転完了までの時間
	static constexpr float TIME_ROT = 1.0f;

	//木のレベル
	static constexpr int LV_OLD = 75;		//木の成長段階
	static constexpr int LV_ADULT = 50;		//木の成長段階
	static constexpr int LV_KID = 25;		//木の成長段階

	//影関係
	static constexpr float PLAYER_SHADOW_SIZE = 100.0f;
	static constexpr float PLAYER_SHADOW_HEIGHT = 300.0f;
	static constexpr float SHADOW_LIFT = 0.5f;
	static constexpr int   SHADOW_MAX_ALPHA = 128;
	static constexpr float SHADOW_UV_SCALE = 2.0f;
	static constexpr float SHADOW_UV_CENTER = 0.5f;
	static constexpr int SHADOW_COLLER = 255;

	//煙エフェクト
	static constexpr float TERM_FOOT_SMOKE = 0.3f;		//煙エフェクト発生間隔
	static constexpr float FOOT_SMOKE_SCALE = 5.0f;		//煙エフェクトのスケール

	//ステ関連
	static constexpr float DOWN_DELTATIME = 1.0f;
	static constexpr int HP = 10;
	static constexpr int D_COUNT = 600;
	static constexpr int WATER_MAX = 10;

	//水のサイズ
	static constexpr float WATER_SCALE_BIG = 0.2f;
	static constexpr float WATER_SCALE_MID = 0.15f;
	static constexpr int WATER_SMALL = 1;
	static constexpr int WATER_MID = 2;
	static constexpr int WATER_BIG = 3;

	//ステータスアップ
	static constexpr int POWER_UP_TIME = 1200;			//パワーアップ時間
	static constexpr int SPEED_UP_TIME = 1200;			//スピードアップ時間
	static constexpr int EX_TIME = 10000;				//無敵時間
	static constexpr float STATUS_UP = 2.0f;			//ステータス変更用の値
	static constexpr float STATUS_EFFECT_SCALE = 20.0f;	//ステータスアップエフェクトのスケール
	static constexpr int HALF_DIVISOR = 2;				//÷2

	//攻撃
	static constexpr int NORMAL_ATTACK = 2;				//通常攻撃
	static constexpr int SLASH_ATTACK = 1;				//スラッシュ
	static constexpr int EX_ATTACK = 2;					//回転斬り
	static constexpr float ATTACK_RADIUS = 100.0f;		//通常攻撃判定の球半径
	static constexpr float ATTACK_FORWARD = 100.0f;		//通常攻撃位置の前方オフセット
	static constexpr float ATTACK2_RADIUS = 140.0f;		//スラッシュ判定半径
	static constexpr float ATTACK2_FORWARD = 80.0f;		//スラッシュ位置前方オフセット
	static constexpr float ATTACK2_HEIGHT = 100.0f;		//スラッシュ位置高さ
	static constexpr float EX_RADIUS = 140.0f;			//回転斬り判定半径
	static constexpr float EX_HEIGHT = 100.0f;			//回転斬り位置高さ

	//ステータス関連
	static constexpr int NAME_X = 55;										//名前の位置X
	static constexpr int NAME_Y = Application::SCREEN_SIZE_Y - 95;			//名前の位置Y
	static constexpr int FRAME_START_X = 47;								//枠の最初X
	static constexpr int FRAME_START_Y = Application::SCREEN_SIZE_Y - 78;	//枠の最初Y
	static constexpr int FRAME_END_X = 653;									//枠の最後X
	static constexpr int FRAME_END_Y = Application::SCREEN_SIZE_Y - 37;		//枠の最後Y
	static constexpr int BAR_START_X = 50;									//バーの最初X
	static constexpr int BAR_START_HY = Application::SCREEN_SIZE_Y - 75;	//バーの最初体力Y
	static constexpr int BAR_START_WY = Application::SCREEN_SIZE_Y - 50;	//バーの最初水Y
	static constexpr int BAR_END_X = 650;									//バーの最後X
	static constexpr int BAR_END_HY = Application::SCREEN_SIZE_Y - 55;		//バーの最後体力Y
	static constexpr int BAR_END_WY = Application::SCREEN_SIZE_Y - 40;		//バーの最後水Y
	static constexpr int BAR_POINT = 60;									//バーの数値

	//しきい値
	static constexpr double SIKII = 0.1;

	//重力
	static constexpr float GRAVITY_POW = 10.0f;
	static constexpr float COLLISION_LINE_UP = 2.0f;
	static constexpr float COLLISION_PUSH_UP = 2.0f;

	//カプセル
	static constexpr int CAPSULE_CNT = 10;

	//色
	int white = 0xffffff; //白
	int black = 0x000000; //黒
	int red = 0xff0000;	  //赤
	int green = 0x00ff00; //緑
	int blue = 0x0000ff;  //青
	int yellow = 0xffff00;//黄
	int purpl = 0x800080; //紫
	int gray = 0xaaaaaa;  //灰

	//ステータス関連
	static constexpr double ICON_SIZE = 1.3;
	static constexpr int ICON_CY_HEIGHT = 115;

	static constexpr int GRAY_ALPHA = 180;

	static constexpr int ICON_CY = 965;
	static constexpr int TIMER_CY = 967;

	static constexpr float RADIUS = 32.0f;
	static constexpr int SEGMENTS = 60;

	static constexpr int POWER_CX = 150;
	static constexpr int SPEED_CX = 225;
	static constexpr int ROT_ATK_CX = 450;


	//状態
	enum class STATE
	{
		NONE,
		PLAY,
	};

	//PLAY中の状態
	enum class PlayerState {
		NORMAL,
		DOWN,
	};
	PlayerState pstate_ = PlayerState::NORMAL;

	//アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		FAST_RUN,
		DOWN,
		NORMALATTACK,
		SLASHATTACK,
		EXATTACK,
	};

	//コンストラクタ
	Player(void);

	//デストラクタ
	~Player(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	//衝突判定に用いられるコライダ制御
	void AddCollider(std::weak_ptr<Collider> collider);
	void ClearCollider(void);

	//敵の情報を取得
	void SetEnemy(const std::vector<std::shared_ptr<EnemyBase>>* enemys);

	//Posを取得
	VECTOR GetPos() const;
	void SetPos(const VECTOR& pos);

	//衝突用カプセルの取得
	const Capsule& GetCapsule(void) const;

	//衝突用の中心座標の取得
	VECTOR GetCollisionPos(void)const;

	//衝突用の球体半径の取得
	float GetCollisionRadius(void);

	//エネミーの衝突用座標
	const std::vector<std::shared_ptr<EnemyBase>>& GetEnemyCollision(void) const;

	//水の受け渡し
	void wHit(float scale);		//水を取得
	void tHit(void);			//木を与える

	//水の量を取得
	int GetWater(void) const;

	//水獲得量特別増加
	bool IsMax(void);
	void SetIsMax(void);

	void Damage(int damage);	//ダメージ
	void PowerUp(void);			//パワーアップ
	void SpeedUp(void);			//スピードアップ
	void Heal(void);			//回復
	void Muteki(void);			//無敵

private:

	//アニメーション
	std::unique_ptr<AnimationController> animationController_;
	void InitAnimation(void);

	//状態管理
	STATE state_;
	std::map<STATE, std::function<void(void)>> stateChanges_;	//状態管理(状態遷移時初期処理)
	std::function<void(void)> stateUpdate_;						//状態管理(更新)

	//状態遷移
	void ChangeState(STATE state);
	void ChangeStatePlay(void);

	void UpdatePlay(void);				//stateがplayの状態のupdate
	void UpdateDown(float deltaTime);	//ダウン中の処理

	//衝突
	std::unique_ptr<Capsule> capsule_;
	std::vector <std::weak_ptr<Collider>> colliders_;	//衝突判定に用いられるコライダ

	//衝突チェック
	VECTOR gravHitPosDown_;
	VECTOR gravHitPosUp_;

	//衝突判定
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);

	//攻撃判定
	void CollisionAttack(void);
	void CollisionAttack2(void);
	void CollisionAttackEx(void);

	//プレイヤーが持つ判定
	VECTOR collisionPos_;			//プレイヤーの当たり判定移動後座標
	float collisionRadius_;			//衝突判定用の球体半径
	VECTOR collisionLocalPos_;		//衝突判定用の球体中心の調整座標

	//プレイヤーの動き
	void ProcessMove(void);			//移動
	void ProcessAttack(void);		//攻撃モーション

	//回転
	void SetGoalRotate(double rotRad);
	void Rotate(void);

	//モーション終了
	bool IsEndLanding(void);		//アタック終了
	bool IsExAttackReady() const;	//回転斬りリセット

	//ステータス変化管理
	void PowerUpTimer(void);		//パワーアップの制限時間
	void SpeedUpTimer(void);		//スピードアップの制限時間

	//復活処理
	void StartRevival();
	void Revival();
	float revivalTimer_;			//復活までの時間

	//エフェクト
	void EffectFootSmoke(void);		//足煙エフェクト
	void EffectPower(void);			//パワーアップエフェクト
	void EffectSpeed(void);			//スピードアップエフェクト
	void EffectHeal(void);			//回復エフェクト

	//移動関連
	VECTOR moveDir_;				//移動方向
	VECTOR movePow_;				//移動量
	VECTOR movedPos_;				//移動後の座標
	VECTOR moveDiff_;				//フレームごとの移動値
	float speed_;					//移動スピード
	bool canMove_;					//移動が可能かどうか
	float stepRotTime_;				//回転補間の進行を管理するタイマー(残り時間)
	VECTOR jumpPow_;				//ジャンプ量
	void CalcGravityPow(void);		//移動量の計算

	//回転
	Quaternion playerRotY_;
	Quaternion goalQuaRot_;

	//ステータス値
	int hp_;		//プレイヤーの体力
	int water_;		//水の所持量

	//攻撃力
	int normalAttack_;	//2ダメージ
	int slashAttack_;	//1ダメージ
	int exrAttack_;		//2ダメージ

	//アイテム効果
	bool powerUpFlag_;	//パワーが上がったている間treu
	bool speedUpFlag_;	//スピードが上がったている間treu
	int powerUpCnt_;	//2パワーアップの時間(20秒)
	int speedUpCnt_;	//スピードアップの時間(20秒)
	bool invincible_;	//無敵状態

	//水の所持上限
	bool isMax_;

	//攻撃フラグ
	bool isAttack_;		//縦斬り
	bool isAttack2_;	//横斬り
	bool exAttack_;		//回転斬り
	int exTimer_;		//クールタイム 10秒（ミリ秒）
	int lastExTime_;	//exが解放されたらすぐに使えるようにする

	//アイコンUI
	int imgPowerIcon_;			//パワー
	int imgSpeedIcon_;			//スピード
	int imgRotateAttackIcon_;	//回転切り

	//丸影
	int imgShadow_;

	//足煙エフェクト
	float stepFootSmoke_;
	int effectSmokeResId_;
	int effectSmokePleyId_;

	//パワーアップエフェクト
	int effectPowerResId_;
	int effectPowerPleyId_;

	//スピードアップエフェクト
	int effectSpeedResId_;
	int effectSpeedPleyId_;

	//回復エフェクト
	float stepHeal_;
	int effectHealResId_;
	int effectHealPleyId_;

	//ポインタ
	const std::vector<std::shared_ptr<EnemyBase>>* enemy_;

	//丸影描画
	void DrawShadow(void);

	//デバッグ処理
	void DrawDebug(void);

};