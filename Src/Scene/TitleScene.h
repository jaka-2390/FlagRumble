#pragma once
#include <memory>
#include "SceneBase.h"
#include "../Object/Common/Transform.h"

class SceneManager;
class SkyDome;
class AnimationController;

class TitleScene : public SceneBase
{
public:
	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void);

	void Init(void) override;
	void NewFunction();
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;
private:
	int cnt;
	int cn3;

	// 画像
	int imgTitle_;
	int img3D_;
	int imgBackTitle_;
	int imgUDCursor_;
	int imgConfirmEnd_;
	int imgYes_;
	int imgNo_;
	int imgYesSel_;
	int imgNoSel_;
	int imgP1_[2];
	int imgP2_[2];

	int selectedIndex_;

	int blinkFrameCount_ = 0;

	int enemyDirection_ = 1;		// 敵の向き（1:右, -1:左）
	int playerDirection_ = 1;		// プレイヤーの向き（1:右, -1:左）
	float enemySpeed_ = 5.0f;		// 敵の移動速度
	float playerSpeed_ = 4.0f;		// プレイヤーの移動速度

	// 画面の移動制限範囲(敵とplayerの追いかけっこ）
	const float leftLimit_ = -1000.0f;
	const float rightLimit_ = 1000.0f;

	// 修了確認用
	bool isConfirmingExit_ = false;			// ← 終了確認中かどうか
	int confirmIndex_ = 1;					// ← 0: はい, 1: いいえ
	int confirmAnimFrame_ = 0;				// 表示アニメーション用フレーム
	const int CONFIRM_ANIM_DURATION = 20;	// アニメーション時間

	// キャラクター
	Transform charactor_;

	// 敵キャラクター
	Transform enemy_;

	// アニメーション
	std::unique_ptr<AnimationController> animationControllerPlayer_;
	std::unique_ptr<AnimationController> animationControllerEnemy_;

	bool endLoadFlame_;
};