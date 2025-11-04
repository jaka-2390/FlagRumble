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

    flagMax_ = static_cast<int>(flags_.size());
}

void FlagManager::Update(const VECTOR& playerPos, bool allEnemyDefeated)
{
    for (size_t i = 0; i < flags_.size(); ++i)
    {
        flags_[i]->Update(playerPos, allEnemyDefeated);

        // ä¯Ç™óßÇ¡ÇΩÇÁéüÇÃä¯Ç…êiÇﬂÇÈ
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

int FlagManager::GetClearedFlagCount() const
{
    int count = 0;

    for (const auto& flag : flags_)
    {
        if (flag->IsFlagClear()) count++;
    }

    return count;
}

std::vector<int> FlagManager::GetSpawnFlag(const VECTOR& playerPos)
{
    std::vector<int> result;
    for (int i = 0; i < (int)flags_.size(); ++i)
    {
        if (flags_[i]->SpawnEnemies(playerPos))
        {
            result.push_back(i);
            flags_[i]->SetEnemySpawned(true); // èoåªçœÇ›Ç…ÇµÇƒÇ®Ç≠
        }
    }

    return result;
}

int FlagManager::GetFlagMax() const
{
    return flagMax_;
}
