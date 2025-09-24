#pragma once
#include <memory>
#include <map>
#include <functional>
#include <DxLib.h>
#include "EnemyBase.h"

class AnimationController;
class Collider;
class Capsule;

class Player : public ActorBase
{

public:

	// スピード
	static constexpr float SPEED_MOVE = 5.0f;
	static constexpr float SPEED_RUN = 10.0f;
	
	//ステータス変更用の値
	static constexpr float STATUS_UP = 2.0f;

	// 回転完了までの時間
	static constexpr float TIME_ROT = 1.0f;

	// ジャンプ力
	static constexpr float POW_JUMP = 35.0f;
	// ジャンプ受付時間
	static constexpr float TIME_JUMP_IN = 0.5f;

	//影の大きさ
	static constexpr float PLAYER_SHADOW_SIZE = 100.0f;
	static constexpr float PLAYER_SHADOW_HEIGHT = 300.0f;

	// 煙エフェクト発生間隔
	static constexpr float TERM_FOOT_SMOKE = 0.3f;

	//ステ関連
	static constexpr int HP = 10;
	static constexpr int D_COUNT = 600;
	static constexpr int WATER_MAX = 10;

	//ステータスアップ
	static constexpr int POWER_UP_TIME = 1200;
	static constexpr int SPEED_UP_TIME = 1200;

	// 状態
	enum class STATE
	{
		NONE,
		PLAY,
		END
	};

	// PLAY中の状態
	enum class PlayerState {
		NORMAL,
		DOWN,
		// 他にも Jumping, Attacking など必要なら追加
	};
	PlayerState pstate_ = PlayerState::NORMAL;

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		FAST_RUN,
		JUMP,
		DOWN,
		ATTACK1,
		ATTACK2,
		EXATTACK,
	};

	// コンストラクタ
	Player(void);

	// デストラクタ
	~Player(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	// --- ゲームステート処理 ---
	bool IsPlay(void) const;	//状態確認

	// --- 当たり判定 / 衝突 ---
	// 衝突判定に用いられるコライダ制御
	void AddCollider(std::weak_ptr<Collider> collider);
	void ClearCollider(void);
	const Capsule& GetCapsule(void) const;// 衝突用カプセルの取得

	VECTOR GetCollisionPos(void)const;	// 衝突用の中心座標の取得
	float GetCollisionRadius(void);		// 衝突用の球体半径の取得
	const std::vector<std::shared_ptr<EnemyBase>>& GetEnemyCollision(void) const;

	// --- 外部インターフェース ---
	void SetEnemy(const std::vector<std::shared_ptr<EnemyBase>>* enemys);//敵の情報を取得
	VECTOR GetPos() const;
	void SetPos(const VECTOR& pos);

	// --- インタラクション（ヒット系） ---
	void wHit(float scale);	//水を取得
	void tHit(void);		//木にヒット

	// --- ステータス操作 ---
	int GetWater(void) const;

	//水獲得量特別増加
	bool IsMax(void);
	void SetIsMax(void);
	
	void Damage(int damage);//ダメージ
	void PowerUp(void);		//パワーアップ
	void SpeedUp(void);		//スピードアップ
	void Heal(void);		//回復
	void Muteki(void);		//無敵

private:

	// --- アニメーション ---
	std::unique_ptr<AnimationController> animationController_;
	void InitAnimation(void);

	// --- 状態管理 ---
	STATE state_;
	std::map<STATE, std::function<void(void)>> stateChanges_;// 状態管理(状態遷移時初期処理)
	std::function<void(void)> stateUpdate_;// 状態管理(更新ステップ)

	// 状態遷移
	void ChangeState(STATE state);
	void ChangeStatePlay(void);

	void UpdatePlay(void);//stateがplayの状態のupdate
	void UpdateDown(float deltaTime);//ダウン中の処理

	// --- 衝突 ---
	std::unique_ptr<Capsule> capsule_;
	std::vector <std::weak_ptr<Collider>> colliders_;// 衝突判定に用いられるコライダ

	// 衝突チェック 衝突用線分
	VECTOR gravHitPosDown_;
	VECTOR gravHitPosUp_;

	// 衝突判定
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);

	// 攻撃判定
	void CollisionAttack(void);
	void CollisionAttack2(void);
	void CollisionAttackEx(void);

	// プレイヤーが持つ判定
	VECTOR collisionPos_;		//プレイヤーの当たり判定移動後座標
	float collisionRadius_;		// 衝突判定用の球体半径
	VECTOR collisionLocalPos_;	// 衝突判定用の球体中心の調整座標

	// --- モーション・操作 ---
	void ProcessMove(void);		// 移動
	void ProcessJump(void);		//ジャンプモーション
	void ProcessAttack(void);	//攻撃モーション

	// 回転
	void SetGoalRotate(double rotRad);
	void Rotate(void);

	// モーション終了
	bool IsEndLanding(void);		// ジャンプ終了
	bool IsEndLandingA(void);		// アタック終了
	bool IsExAttackReady() const;	// 回転斬りリセット

	// --- ステータス変化管理 ---
	void PowerUpTimer(void);//パワーアップの制限時間
	void SpeedUpTimer(void);//スピードアップの制限時間

	//復活処理
	void StartRevival();
	void Revival();
	float revivalTimer_;// 復活までの時間

	// --- エフェクト --
	void EffectFootSmoke(void);	// 足煙エフェクト
	void EffectPower(void);		// パワーアップエフェクト
	void EffectSpeed(void);		// スピードアップエフェクト
	void EffectHeal(void);		//回復エフェクト

	// --- 移動関連 ---
	VECTOR moveDir_;	// 移動方向
	VECTOR movePow_;	// 移動量
	VECTOR movedPos_;	// 移動後の座標
	VECTOR moveDiff_;	// フレームごとの移動値
	float speed_;		// 移動スピード
	bool canMove_;		// 移動が可能かどうか
	float stepJump_;	// ジャンプの入力受付時間
	float stepRotTime_;	// 回転補間の進行を管理するタイマー(残り時間)
	VECTOR jumpPow_;	// ジャンプ量
	void CalcGravityPow(void);// 移動量の計算

	// 回転
	Quaternion playerRotY_;
	Quaternion goalQuaRot_;

	// --- ステータス値 ---
	int hp_;			// プレイヤーの体力
	int water_;			// 水の所持量

	//攻撃力
	int normalAttack_;	//2ダメージ
	int slashAttack_;	//1ダメージ
	int exrAttack_;		//2ダメージ

	// アイテム効果
	bool powerUpFlag_;	//パワーが上がったている間treu
	bool speedUpFlag_;	//スピードが上がったている間treu
	int powerUpCnt_;	//2パワーアップの時間(20秒)
	int speedUpCnt_;	//スピードアップの時間(20秒)
	bool invincible_;	// 無敵状態

	// 水
	bool isMax_;//水の所持上限

	// --- 攻撃フラグ ---
	// ジャンプ判定
	bool isJump_;

	//攻撃の判定
	bool isAttack_;		//縦斬り
	bool isAttack2_;	//横斬り
	bool exAttack_;		//回転斬り
	int exTimer_;		//クールタイム 10秒（ミリ秒）
	int lastExTime_;	//exが解放されたらすぐに使えるようにする
	
	// --- アイコンUI ---
	int imgPowerIcon_;			// パワー
	int imgSpeedIcon_;			// スピード
	int imgRotateAttackIcon_;	// 回転切り

	// --- 演出 ---
	int imgShadow_;// 丸影

	// 足煙エフェクト
	float stepFootSmoke_;
	int effectSmokeResId_;
	int effectSmokePleyId_;

	// パワーアップエフェクト
	int effectPowerResId_;
	int effectPowerPleyId_;

	// スピードアップエフェクト
	int effectSpeedResId_;
	int effectSpeedPleyId_;

	// 回復エフェクト
	float stepHeal_;
	int effectHealResId_;
	int effectHealPleyId_;

	// --- ポインタ ---
	const std::vector<std::shared_ptr<EnemyBase>>* enemy_;// 敵の一覧への参照(攻撃や当たり判定に使用)

	// --- デバッグ用 ---
	// 更新ステップ
	void DrawShadow(void);

	// 描画系
	void DrawDebug(void);

};
