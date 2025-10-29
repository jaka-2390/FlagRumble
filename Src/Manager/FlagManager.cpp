#include "FlagManager.h"

void FlagManager::Init()
{
    flags_.clear();

    auto flag1 = std::make_unique<Flag>(VGet(-660.0f, 254.0f, -100.0f));
    auto flag2 = std::make_unique<Flag>(VGet(0.0f, 254.0f, -200.0f));
    auto flag3 = std::make_unique<Flag>(VGet(400.0f, 254.0f, -300.0f));

    flags_.push_back(std::move(flag1));
    flags_.push_back(std::move(flag2));
    flags_.push_back(std::move(flag3));

    for (auto& flag : flags_)
    {
        flag->Init();
    }
}

void FlagManager::Update(const VECTOR& playerPos, bool allEnemyDefeated)
{
    for (size_t i = 0; i < flags_.size(); ++i)
    {
        flags_[i]->Update(playerPos, allEnemyDefeated);

        // Šø‚ª—§‚Á‚½‚çŽŸ‚ÌŠø‚Éi‚ß‚é
        if (i == nextFlagIndex_ && flags_[i]->IsFlagClear())
        {
            nextFlagIndex_++;
        }
    }
}

void FlagManager::Draw()
{
    for (auto& flag : flags_)
    {
        flag->Draw();
    }
}

VECTOR FlagManager::GetFlagPosition(int index) const
{
    if (index < 0 || index >= static_cast<int>(flags_.size()))
        return VGet(0, 0, 0);

    return flags_[index]->GetPosition();
}

bool FlagManager::AllFlagsCleared() const
{
    for (const auto& flag : flags_)
    {
        if (!flag->IsFlagClear())
            return false;
    }

    return true;
}

int FlagManager::GetNextFlagIndex() const
{
    return nextFlagIndex_;
}

int FlagManager::GetClearedFlagCount() const
{
    int count = 0;

    for (const auto& flag : flags_)
    {
        if (flag->IsFlagClear()) count++;
    }

    return count;
}