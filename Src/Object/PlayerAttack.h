#pragma once
#include <DxLib.h>
#include "../Manager/InputManager.h"

class Player;
class EnemyBase;
class AnimationController;

class PlayerAttack
{

public:

	//攻撃
	static constexpr int NORMAL_ATTACK = 2;				//通常攻撃
	static constexpr float ATTACK_FRAME = 6.5f;			//攻撃がヒットするフレーム
	static constexpr float ATTACK_FORWARD = 100.0f;		//通常攻撃位置の前方オフセット
	static constexpr float ATTACK_RADIUS = 100.0f;		//通常攻撃判定の球半径
	static constexpr float HIT_STOP = 4.0f;				//攻撃時のヒットストップ
	static constexpr float EFFECT_SCALE = 20.0f;		//エフェクトのスケール

	static constexpr  float ANIM_SPEED = 15.0f;			//アニメーションスピード
	static constexpr int ANIM_NORMALATTACK_INDEX = 5;	//アニメーション番号

	enum class ATTACK_TYPE
	{
		NONE,
		NORMALATTACK = 5
	};

	//コンストラクタ
	explicit PlayerAttack(Player* owner);	//explicit：PlayerからPlayerAttackへの自動変換を禁止する

	//初期化
	void Init(void);

	//更新
	void Update(void);

	bool IsAttacking(void) const;

private:

	//入力受付
	void ProcessInput(void);

	//攻撃の更新
	void UpdateNormalAttack(void);

	//当たり判定
	void CollisionNormalAttack(void);

	//ヒットストップ
	void StartHitStop(void);

	//エフェクト
	void EffectSword(const VECTOR& pos);

	InputManager& ins = InputManager::GetInstance();

	//所有者
	Player* owner_;   //Playerへの参照

	//フラグ
	bool isAttack_;
	bool hasHit_;

	//ダメージ
	int normalAttack_;

	//ヒットストップ
	bool isHitStop_;
	int hitStopFrame_;

	//エフェクト
	int effectSwordResId_;
	int effectSwordPlayId_;

};

