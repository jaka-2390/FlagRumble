#include "Sound.h"
#include <algorithm>

Sound::Sound(void)
{
    soundType_ = TYPE::NONE;
    path_ = "";
    for (int i = 0; i < MAX_HANDLE_NUM; ++i)
    {
        handleIds_[i] = -1;
    }
    pos_ = { 0.0f, 0.0f, 0.0f };
    radius_ = 0.0f;
    maxVolume_ = 255;
}

Sound::Sound(TYPE type, const std::string& path)
{
    soundType_ = type;
    path_ = path;
    for (int i = 0; i < MAX_HANDLE_NUM; ++i)
    {
        handleIds_[i] = -1;
    }
    pos_ = { 0.0f, 0.0f, 0.0f };
    radius_ = 0.0f;
    maxVolume_ = 255;
}

Sound::~Sound(void)
{
    Release();
}

void Sound::Update(VECTOR pos)
{
    if (soundType_ != TYPE::SOUND_3D)
        return;

    for (int i = 0; i < MAX_HANDLE_NUM; ++i)
    {
        if (CheckSoundMem(handleIds_[i]) == 1)
        {
            Set3DPositionSoundMem(pos, handleIds_[i]);
        }
    }

    pos_ = pos;
}

void Sound::Load(void)
{
    for (int i = 0; i < MAX_HANDLE_NUM; ++i)
    {
        SetCreate3DSoundFlag(soundType_ == TYPE::SOUND_3D);
        handleIds_[i] = LoadSoundMem(path_.c_str());
    }
}

void Sound::Release(void)
{
    for (int i = 0; i < MAX_HANDLE_NUM; ++i)
    {
        if (handleIds_[i] != -1)
        {
            StopSoundMem(handleIds_[i]);
            DeleteSoundMem(handleIds_[i]);
            handleIds_[i] = -1;
        }
    }
}

bool Sound::Play(TIMES times)
{
    if (soundType_ != TYPE::SOUND_2D)
        return false;

    if (times == TIMES::ONCE)
    {
        // どれか一つでも再生中なら無視
        for (int i = 0; i < MAX_HANDLE_NUM; ++i)
        {
            if (CheckSoundMem(handleIds_[i]) == 1)
                return false;
        }
        int result = PlaySoundMem(handleIds_[0], DX_PLAYTYPE_BACK, TRUE);
        ChangeVolume(1.0f);
        return result == 0;
    }

    if (times == TIMES::FORCE_ONCE || times == TIMES::LOOP)
    {
        for (int i = 0; i < MAX_HANDLE_NUM; ++i)
        {
            if (CheckSoundMem(handleIds_[i]) != 1)
            {
                int result = PlaySoundMem(handleIds_[i], times == TIMES::LOOP ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK, TRUE);
                ChangeVolume(1.0f);
                return result == 0;
            }
        }
    }

    return false;
}

bool Sound::Play(VECTOR pos, float radius, TIMES times)
{
    if (soundType_ != TYPE::SOUND_3D)
        return false;

    // ※今回は ONCE / FORCE_ONCE の違いは未実装（必要あれば対応可）
    for (int i = 0; i < MAX_HANDLE_NUM; ++i)
    {
        if (CheckSoundMem(handleIds_[i]) != 1)
        {
            Set3DPositionSoundMem(pos, handleIds_[i]);
            Set3DRadiusSoundMem(radius, handleIds_[i]);
            int result = PlaySoundMem(handleIds_[i], times == TIMES::LOOP ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK, TRUE);
            ChangeVolume(1.0f);
            this->pos_ = pos;
            this->radius_ = radius;
            return result == 0;
        }
    }

    return false;
}

void Sound::Stop(void)
{
    for (int i = 0; i < MAX_HANDLE_NUM; ++i)
    {
        if (CheckSoundMem(handleIds_[i]) == 1)
        {
            StopSoundMem(handleIds_[i]);
        }
    }
    pos_ = { 0.0f, 0.0f, 0.0f };
    radius_ = 0.0f;
}

bool Sound::CheckMove(void)
{
    for (int i = 0; i < MAX_HANDLE_NUM; ++i)
    {
        if (CheckSoundMem(handleIds_[i]) == 1)
            return true;
    }
    return false;
}

bool Sound::CheckLoad(void)
{
    return handleIds_[0] != -1;
}

void Sound::ChangeVolume(float per)
{
    per = std::clamp(per, 0.0f, 1.0f);
    int vol = static_cast<int>(per * maxVolume_);
    for (int i = 0; i < MAX_HANDLE_NUM; ++i)
    {
        ChangeVolumeSoundMem(vol, handleIds_[i]);
    }
}

void Sound::ChangeMaxVolume(float per)
{
    per = std::clamp(per, 0.0f, 1.0f);
    maxVolume_ = static_cast<int>(255 * per);
}