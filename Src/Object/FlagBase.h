#pragma once
#include <DxLib.h>

class FlagBase 
{

public:

    virtual ~FlagBase() {}

    virtual void Init() = 0;
    virtual void Update(const VECTOR& playerPos, bool allEnemyDefeated) = 0;
    virtual void Draw() = 0;

    bool IsFlagClear() const;
    VECTOR GetPosition() const;

protected:

    VECTOR pos_;
    VECTOR scl_;
    VECTOR rot_;

    bool circleVisible_ = false;
    bool flagVisible_ = false;
    bool flagClear_ = false;
    float clearGauge_ = 0.0f;
    float clearGaugeMax_ = 100.0f;
    float flagRadius_ = 100.0f;

    void DrawCircleOnMap(VECTOR center, float radius, int color);
    void DrawGauge3D(VECTOR center, float gaugeRate);

};