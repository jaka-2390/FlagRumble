#include "FlagManager.h"

void FlagManager::Init()
{
    flags_.clear();

    auto flag1 = std::make_unique<Flag>(VGet(-2000.0f, 254.0f, 2000.0f), Flag::ENEMY_TYPE::DOG);
    auto flag2 = std::make_unique<Flag>(VGet(-250.0f, 254.0f, 4000.0f), Flag::ENEMY_TYPE::SABO);
    auto flag3 = std::make_unique<Flag>(VGet(2300.0f, 254.0f, 2000.0f), Flag::ENEMY_TYPE::DOG);
    auto flag4 = std::make_unique<Flag>(VGet(-250.0f, 254.0f, 1000.0f), Flag::ENEMY_TYPE::DOG);

    flags_.push_back(std::move(flag1));
    flags_.push_back(std::move(flag2));
    flags_.push_back(std::move(flag3));
    flags_.push_back(std::move(flag4));

    for (auto& flag : flags_)
    {
        flag->Init();
    }

    flagMax_ = static_cast<int>(flags_.size());
}

void FlagManager::Update(const VECTOR& playerPos, const std::vector<std::shared_ptr<EnemyBase>>& enemies)
{
    for (size_t i = 0; i < flags_.size(); ++i)
    {
        flags_[i]->Update(playerPos, enemies);
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

Flag* FlagManager::GetFlag(int index) const
{
    if (index < 0 || index >= static_cast<int>(flags_.size()))
        return nullptr;

    return flags_[index].get();
}

int FlagManager::GetClearedFlagCount() const
{
    int count = 0;

    for (const auto& flag : flags_)
    {
        if (flag->IsOwnedByPlayer()) count++;
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

std::vector<Flag*> FlagManager::GetPlayerFlags() const
{
    std::vector<Flag*> playerFlags;
    for (auto& flag : flags_)
    {
        if (flag->IsOwnedByPlayer() && flag->GetEnemyType() != Flag::ENEMY_TYPE::SABO)
        {
            playerFlags.push_back(flag.get());
        }
    }
    return playerFlags;
}
