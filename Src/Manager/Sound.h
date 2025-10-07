#pragma once
#include <string>
#include <DxLib.h>

class Sound
{

public:

    enum class TYPE
    {
        NONE,
        SOUND_2D,
        SOUND_3D,
    };

    enum class TIMES
    {
        ONCE,
        LOOP,
        FORCE_ONCE,  // ← 新しく追加
    };

    static const int MAX_HANDLE_NUM = 2;    

    Sound(void);
    Sound(TYPE type, const std::string& path);
    ~Sound(void);

    void Update(VECTOR pos);
    void Load(void);
    void Release(void);

    bool Play(TIMES times); // 2D音再生
    bool Play(VECTOR pos, float radius, TIMES times); // 3D音再生（未対応だが必要なら拡張可能）

    void Stop(void);
    bool CheckMove(void);
    bool CheckLoad(void);

    void ChangeVolume(float per);
    void ChangeMaxVolume(float per);

private:

    TYPE soundType_;
    std::string path_;
    int handleIds_[MAX_HANDLE_NUM];
    int maxVolume_;
    VECTOR pos_;
    float radius_;

};