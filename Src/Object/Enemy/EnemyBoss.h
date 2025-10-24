#pragma once
#include "../EnemyBase.h"

class EnemyBoss : public EnemyBase
{

public:

	static constexpr VECTOR BOSS_SCALE = { 0.5f,0.5f,0.5f };
	static constexpr  float SPEED = 2.0f;

	//ボスの最大HP
	static constexpr int BOSS_MAX_HP = 10;

	// HPバー表示
	static constexpr int BOSS_HP_BAR_WIDTH = 600;
	static constexpr int BOSS_HP_BAR_HEIGHT = 20;
	static constexpr int BOSS_HP_BAR_Y = 80;
	static constexpr int BOSS_LABEL_OFFSET_X = -120;
	static constexpr int BOSS_LABEL_OFFSET_Y = -20;

	static constexpr  int ATTACK_POW = 3;

	EnemyBoss();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

	void DrawBossHpBar(void) override;

private:

	//フォントサイズ
	static constexpr int FONT_SIZE = 55;

	//ボスのアニメーション関連
	static constexpr float ANIM_SPEED = 20.0f;
	static constexpr float ANIM_ATK_SPEED = 15.0f;
	//アニメーション番号
	static constexpr int   ANIM_IDLE_INDEX = 2;
	static constexpr int   ANIM_RUN_INDEX = 4;
	static constexpr int   ANIM_ATTACK_INDEX = 0;
	static constexpr int   ANIM_DEATH_INDEX = 1;

};