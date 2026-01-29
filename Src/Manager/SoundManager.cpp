#include "../Application.h"
#include "SoundManager.h"
#include "Sound.h"

SoundManager* SoundManager::instance_ = nullptr;

void SoundManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new SoundManager();
	}
	instance_->Init();
}

SoundManager& SoundManager::GetInstance(void)
{
	return *instance_;
}

void SoundManager::Init(void)
{
	using RES = Sound;
	using RES_T = RES::TYPE;

	static std::string PATH_EFF = Application::PATH_SOUND;
	Sound res;

	// タイトル
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "TitleBGM.mp3");
	res.ChangeMaxVolume(0.7);
	soundMap_.emplace(SRC::TITLE_BGM, res);

	//デモ
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "DemoBGM.mp3");
	res.ChangeMaxVolume(0.7);
	soundMap_.emplace(SRC::DEMO_BGM, res);

	// ゲーム
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "GameBGM.mp3");
	res.ChangeMaxVolume(0.55);
	soundMap_.emplace(SRC::GAME_BGM, res);

	// ゲームオーバー
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "GameOverBGM.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::GAMEOVER_BGM, res);

	// ゲームクリア
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "GameClearBGM.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::GAMECLEAR_BGM, res);

	// レベルアップ
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "LevelUpSE.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::LEVEL_UP_SE, res);

	// 水ゲット
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "GetWeterSE.mp3");
	res.ChangeMaxVolume(3.0);
	soundMap_.emplace(SRC::GETWATER_SE, res);

	// アタック1
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "AtkSE1.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::ATK_SE1, res);

	// アタック2
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "AtkSE2.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::ATK_SE2, res);

	// アタック3
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "AtkSE3.mp3");
	res.ChangeMaxVolume(0.6);
	soundMap_.emplace(SRC::ATK_SE3, res);

	// スピードアップ
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "SpeedUpSE.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::SPEEDUP_SE, res);

	// パワーアップ
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "PowerUpSE.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::POWERUP_SE, res);

	// 木が無敵
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "MutekiSE.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::MUTEKI_SE, res);

	// 回復
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "HealSE.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::HEAL_SE, res);

	// 敵死亡
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "EDownSE.mp3");
	res.ChangeMaxVolume(0.55);
	soundMap_.emplace(SRC::E_DOWN_SE, res);

	// プレイヤー死亡
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "PDownSE.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::P_DOWN_SE, res);

	// プレイヤーダメージ
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "PDamageSE.mp3");
	res.ChangeMaxVolume(1.0);
	soundMap_.emplace(SRC::P_DAMAGE_SE, res);

	// 木ダメージ
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "TDamageSE.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::T_DAMAGE_SE, res);

	// 敵ダメージ
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "EDamageSE.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::E_DAMAGE_SE, res);

	// 決定
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "SetSE.mp3");
	res.ChangeMaxVolume(0.5);
	soundMap_.emplace(SRC::SET_SE, res);

	// 警告
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "WarningSE.mp3");
	res.ChangeMaxVolume(1.5);
	soundMap_.emplace(SRC::WARNING_SE, res);

	//フラッグ取得
	res = Sound(Sound::TYPE::SOUND_2D, Application::PATH_SOUND + "GetFlagSE.mp3");
	res.ChangeMaxVolume(0.8);
	soundMap_.emplace(SRC::GETFLAG_SE, res);
}

void SoundManager::Release(void)
{
	for (auto& p : soundMap_)
	{
		p.second.Release();
	}

	soundMap_.clear();
}

void SoundManager::Destroy(void)
{
	Release();
	soundMap_.clear();
	delete instance_;
}

bool SoundManager::Play(SRC src, Sound::TIMES times)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		if (!lPair->second.CheckLoad())
		{
			lPair->second.Load();
		}
		return lPair->second.Play(times);
	}
	return false;
}

bool SoundManager::Play(SRC src, Sound::TIMES times, VECTOR pos, float radius)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		if (!lPair->second.CheckLoad())
		{
			lPair->second.Load();
		}
		return lPair->second.Play(pos, radius, times);
	}
	return false;
}

void SoundManager::Stop(SRC src)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		return lPair->second.Stop();
	}
}

bool SoundManager::CheckMove(SRC src)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		return lPair->second.CheckMove();
	}
	return false;
}

void SoundManager::ChengeVolume(SRC src, float per)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		return lPair->second.ChangeVolume(per);
	}
}
