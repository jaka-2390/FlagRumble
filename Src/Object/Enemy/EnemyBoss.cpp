#include "EnemyBoss.h"
#include "../../Application.h"
#include "../Common/AnimationController.h"
#include "../../Manager/ResourceManager.h"
#include "../../Utility/AsoUtility.h"

EnemyBoss::EnemyBoss() :EnemyBase()
{
}

void EnemyBoss::InitAnimation(void)
{
	std::string path = Application::PATH_MODEL + "Enemy/Boss/Boss.mv1";

	animationController_ = std::make_unique<AnimationController>(transform_.modelId);

	animationController_->Add((int)ANIM_TYPE::IDLE, path, ANIM_SPEED, ANIM_IDLE_INDEX);
	animationController_->Add((int)ANIM_TYPE::RUN, path, ANIM_SPEED, ANIM_RUN_INDEX);
	animationController_->Add((int)ANIM_TYPE::ATTACK, path, ANIM_ATK_SPEED, ANIM_ATTACK_INDEX);
	animationController_->Add((int)ANIM_TYPE::DEATH, path, ANIM_SPEED, ANIM_DEATH_INDEX);

	animationController_->Play((int)ANIM_TYPE::RUN);
}

void EnemyBoss::SetParam(void)
{
	// 使用メモリ容量と読み込み時間の削減のため
	// モデルデータをいくつもメモリ上に存在させない
	transform_.SetModel(ResourceManager::GetInstance().LoadModelDuplicate(ResourceManager::SRC::BOSS));

	transform_.scl = { BOSS_SCALE };						// 大きさの設定
	transform_.quaRotLocal = Quaternion::Euler(AsoUtility::Deg2RadF(0.0f)
		, AsoUtility::Deg2RadF(DEGREE), 0.0f);//クォータニオンをいじると向きが変わる
	transform_.dir = { AsoUtility::VECTOR_ZERO };						// 右方向に移動する

	speed_ = SPEED;		// 移動スピード

	isAlive_ = true;	// 初期は生存状態

	hp_ = BOSS_MAX_HP;	// HPの設定

	attackPow_ = ATTACK_POW;	//攻撃力

	collisionRadius_ = COLLOSION_RADIUS * VALUE_TWO;	// 衝突判定用の球体半径
	collisionLocalPos_ = COLLISION_POS;	// 衝突判定用の球体中心の調整座標

	attackCollisionRadius_ = COLLOSION_RADIUS * VALUE_TWO;	// 攻撃判定用と攻撃範囲の球体半径

	enemyType_ = TYPE::BOSS;

	// 初期状態
	ChangeState(STATE::PLAY);
}

void EnemyBoss::DrawBossHpBar(void)
{
	if (!isAlive_)
	{
		return;
	}

	int barX = (Application::SCREEN_SIZE_X - BOSS_HP_BAR_WIDTH) / static_cast<int>(VALUE_TWO); // 中央X
	int barY = BOSS_HP_BAR_Y; // 上から80px

	float hpRate = static_cast<float>(hp_) / BOSS_MAX_HP;
	int hpDrawWidth = static_cast<int>(BOSS_HP_BAR_WIDTH * hpRate);

	// ラベル
	SetFontSize(FONT_SIZE);
	DrawFormatString(barX + BOSS_LABEL_OFFSET_X, barY + BOSS_LABEL_OFFSET_Y, white, "BOSS");
	SetFontSize(DEFAULT_FONT_SIZE);

	// 背景バー（黒）
	DrawBox(barX, barY, barX + BOSS_HP_BAR_WIDTH, barY + BOSS_HP_BAR_HEIGHT, black, TRUE);

	// HPバー（赤）
	DrawBox(barX, barY, barX + hpDrawWidth, barY + BOSS_HP_BAR_HEIGHT, red, TRUE);

	// 枠線（白）
	DrawBox(barX, barY, barX + BOSS_HP_BAR_WIDTH, barY + BOSS_HP_BAR_HEIGHT, white, FALSE);
}