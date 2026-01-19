#pragma once
#include <DxLib.h>

class Player;
class EnemyBase;
class AnimationController;

class PlayerAttack
{

public:

	//攻撃
	static constexpr int NORMAL_ATTACK = 2;				//通常攻撃


	//コンストラクタ
	explicit PlayerAttack(Player* owner);	//explicit：PlayerからPlayerAttackへの自動変換を禁止する

	//初期化
	void Init(void);

	//更新
	void Update(void);

	bool IsAttacking() const;

private:

	//入力受付
	void ProcessInput();

	//攻撃の更新
	void UpdateNormalAttack();

	//当たり判定
	void CollisionNormalAttack();

	//ヒットストップ
	void StartHitStop();

	//エフェクト
	void EffectSword(const VECTOR& pos);


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

