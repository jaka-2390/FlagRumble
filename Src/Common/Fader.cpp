#include <DxLib.h>
#include "../Application.h"
#include "Fader.h"

Fader::STATE Fader::GetState(void) const
{
	return state_;
}

bool Fader::IsEnd(void) const
{
	return isEnd_;
}

void Fader::SetFade(STATE state)
{
	state_ = state;
	if (state_ != STATE::NONE)
	{
		isPreEnd_ = false;
		isEnd_ = false;
	}
}

void Fader::Init(void)
{
	state_ = STATE::NONE;
	alpha_ = 0;
	isPreEnd_ = true;
	isEnd_ = true;

	LoadDivGraph(
		"Data/image/Load.png",	 // スプライトシート
		4,						 // 分割数
		4, 1,						// 横4コマ、縦1コマ
		600, 129,					// 各コマのサイズ
		imgLoad_					// グラフィックIDを格納
	);

	loadingTimer_ = 0;
}

void Fader::Update(void)
{
	loadingTimer_++;

	if (isEnd_)
	{
		return;
	}

	switch (state_)
	{
	case STATE::NONE:
		return;

	case STATE::FADE_OUT:
		alpha_ += SPEED_ALPHA;
		if (alpha_ > 255)
		{
			// フェード終了
			alpha_ = 255;
			if (isPreEnd_)
			{
				// 1フレーム後(Draw後)に終了とする
				isEnd_ = true;
			}
			isPreEnd_ = true;
		}

		break;

	case STATE::FADE_IN:
		alpha_ -= SPEED_ALPHA;
		if (alpha_ < 0)
		{
			// フェード終了
			alpha_ = 0;
			if (isPreEnd_)
			{
				// 1フレーム後(Draw後)に終了とする
				isEnd_ = true;
			}
			isPreEnd_ = true;
		}
		break;

	default:
		return;
	}

}

void Fader::Draw(void)
{

	switch (state_)
	{
	case STATE::NONE:
		return;
	case STATE::FADE_OUT:
	case STATE::FADE_IN:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)alpha_);
		DrawBox(
			0, 0,
			Application::SCREEN_SIZE_X,
			Application::SCREEN_SIZE_Y,
			0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		break;
	}

	// 暗転中に「Now Loading...」など表示
	if (state_ == Fader::STATE::FADE_OUT || state_ == Fader::STATE::FADE_IN) {
		//DrawString(400, 400, "Now Loading...", GetColor(255, 255, 255)); // 中央表示など
		//DrawGraph(1200, 900, imgLoad_[4], true);


		int index = (loadingTimer_ / 20) % 4; // 約1秒で一周

		DrawGraph(1200, 900, imgLoad_[index], TRUE);
	}

}
