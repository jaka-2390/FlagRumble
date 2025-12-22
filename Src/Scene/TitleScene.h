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

	static constexpr int ONE_SECOND_FRAME = 60;			//1秒

	static constexpr int MENU_SELECT = 3;

	static constexpr float PLAYER_POS_X = -900.0f;
	static constexpr float ENEMY_POS_X = -1100.0f;

	static constexpr float CHARACTER_POS_Y = -400.0f;
	static constexpr float CHARACTER_POS_Z = 100.0f;

	static constexpr float PLAYER_SIZE = 1.4f;
	static constexpr float ENEMY_SIZE = 1.3f;

	static constexpr float ANIMATION_TIME = 20.0f;

	static constexpr int PLAYER_ANIM_NUM = 2;
	static constexpr int ENEMY_ANIM_NUM = 1;

	static constexpr float PLAYER_SPEED = 5.0f;
	static constexpr float ENEMY_SPEED = 5.0f;
	static constexpr float LEFT_BOUND = -1150.0f;
	static constexpr float RIGHT_BOUND = 1150.0f;
	static constexpr float SAFE_DISTANCE = 250.0f;

	static constexpr float INVERSION = 90.0f;

	static constexpr float IMG_TITLE_SIZE = 0.6;
	static constexpr int IMG_TITLE_HEIGHT = 200;

	static constexpr int WHITE_ALPHA = 255;
	static constexpr int GRAY_ALPHA = 220;

	static constexpr int BUTTON_W = 400;
	static constexpr int BUTTON_H = 100;
	static constexpr int BASE_Y = 450;
	static constexpr int BUTTON_OFFSET = 120;

	static constexpr int CURSOR_1_WIDTH = 720;
	static constexpr int CURSOR_HEIGHT = 450;

	static constexpr int INDEX = 110;
	static constexpr float IMG_CURSOR_SIZE = 4.5;
	static constexpr float CURSOR_MOVE_SPEED = 0.03;

	static constexpr int TEXT_WIDTH = 780;

	static constexpr int TEXT_HEIGHT_1 = 420;
	static constexpr int TEXT_HEIGHT_2 = 530;
	static constexpr int TEXT_HEIGHT_3 = 640;

	static constexpr int END_STRING_WIDTH = 360;
	static constexpr int END_STRING_HEIGHT = 280;

	static constexpr int YES_STRING_WIDTH = 560;
	static constexpr int YES_STRING_HEIGHT = 640;

	static constexpr int NO_STRING_WIDTH = 1090;
	static constexpr int NO_STRING_HEIGHT = 640;

	//色
	int white = 0xffffff; //白
	int black = 0x000000; //黒
	int red = 0xff0000;	  //赤
	int green = 0x00ff00; //緑
	int blue = 0x0000ff;  //青
	int yellow = 0xffff00;//黄
	int purpl = 0x800080; //紫

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

	// 画像
	int imgTitle_;
	int imgBackTitle_;
	int imgUDCursor_;
	int imgConfirmEnd_;
	int imgYes_;
	int imgNo_;
	int imgYesSel_;
	int imgNoSel_;
	int imgP1_[2];
	int imgFlag_;

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