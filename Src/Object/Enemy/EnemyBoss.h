#pragma once
#include "../EnemyBase.h"

class EnemyBoss : public EnemyBase
{
public:

	//ボスの最大HP
	static constexpr int BOSS_MAX_HP = 30;

	// HPバー表示
	static constexpr int BOSS_HP_BAR_WIDTH = 600;
	static constexpr int BOSS_HP_BAR_HEIGHT = 20;
	static constexpr int BOSS_HP_BAR_Y = 80;
	static constexpr int BOSS_LABEL_OFFSET_X = -120;
	static constexpr int BOSS_LABEL_OFFSET_Y = -20;

	EnemyBoss();

	//アニメーションロード用
	void InitAnimation(void) override;

	// パラメータ設定(純粋仮想関数)
	void SetParam(void) override;

	//ドロップアイテム
	//Item::TYPE GetDropItemType() const override;

	void DrawBossHpBar(void) override;
};
