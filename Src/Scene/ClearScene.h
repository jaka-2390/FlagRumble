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
	int msgX_, msgY_;
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

	// -------------------------------------


};