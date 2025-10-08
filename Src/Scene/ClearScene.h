#pragma once
#include <memory>
#include "SceneBase.h"
#include "../Object/Common/Transform.h"

class AnimationController;
class SceneManager;

class ClearScene : public SceneBase
{

public:

	ClearScene(void);	// コンストラクタ
	~ClearScene(void);	// デストラクタ

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	//汎用性の値
	static constexpr float VALUE_TWO = 2.0f;

	// アニメーション用定数
	static constexpr int MESSAGE_Y = 800;
	static constexpr int MASK_SPEED = 5;
	static constexpr int FADE_SPEED = 2;

	// PressKey アニメーション用
	static constexpr int PRESS_KEY_OFFSET_Y = 100;   // 画面下からの開始オフセット
	static constexpr int PRESS_KEY_TARGET_Y = 600;
	static constexpr int PRESS_KEY_SLIDE_SPEED = 6;
	static constexpr int PRESS_KEY_FADE_SPEED = 5;
	static constexpr float ENEMY_FLOAT_SPEED_Y = 4.0f;

	// 強制遷移までの時間
	static constexpr int AUTO_RETURN_FRAME = 3600;

	// アルファ値の最大値
	static constexpr int ALPHA_MAX = 255;

	// 敵の初期設定
	static constexpr VECTOR ENEMY_INIT_POS = { -490.0f, -600.0f, 50.0f };
	static constexpr VECTOR ENEMY_INIT_SCL = { 1.3f, 1.3f, 1.3f };
	static constexpr float ENEMY_INIT_ROT_Y_DEG = -20.0f;

	// Clear画像
	static constexpr int CLEAR_IMG_Y = 200;
	static constexpr float CLEAR_IMG_SCALE = 1.3f;

	// メッセージ
	static constexpr int MESSAGE_FONT_SIZE = 128;
	static constexpr int MESSAGE_X_OFFSET = 5; // 文字列調整の係数
	static constexpr float MESSAGE_X_SCALE = 3.5f;

	// フォント戻すサイズ
	static constexpr int FONT_SIZE_DEFAULT = 16;

	// 画像
	int imgClear_;
	int imgBackGameClaer_;
	int imgClearWolrd_;
	int imgReplay_;
	int imgReturn_;
	int imgPressKey_;

	// カウンター
	int cheackCounter_;

	// アニメーション用
	// -------------------------------------
	int maskLeftX_;
	int messageX_, messageY_;
	int imgW_, imgH_;
	int maskSpeed_;

	int clearAlpha_;     // 透明度（0～255）
	int fadeSpeed_;      // フェードインの速度

	// imgPressKey_に使用するアニメーション
	int pressKeyY_;				// 表示Y座標（初期は画面外）
	int targetPressKeyY_;		// 最終的な表示位置
	int pressKeyAlpha_;			// フェードインの透明度
	bool isPressKeyAnimStart_;
	bool isPressKeyAnimEnd_;	// アニメーション完了フラグ

	// 敵キャラクター
	Transform enemy_;

	// 敵のアニメーション
	std::unique_ptr<AnimationController> animationControllerEnemy_;

	// アニメーション終わり
	bool isAnimEnd_;

};