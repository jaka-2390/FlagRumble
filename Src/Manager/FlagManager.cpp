#include "FlagManager.h"

void FlagManager::Init()
{
    flags_.clear();

    auto flag1 = std::make_unique<Flag>(VGet(-2000.0f, 254.0f, 2000.0f), Flag::ENEMY_TYPE::DOG, Flag::STATE::ENEMY);
    auto flag2 = std::make_unique<Flag>(VGet(-250.0f, 254.0f, 4000.0f), Flag::ENEMY_TYPE::SABO, Flag::STATE::ENEMY);
    auto flag3 = std::make_unique<Flag>(VGet(2300.0f, 254.0f, 2000.0f), Flag::ENEMY_TYPE::DOG, Flag::STATE::ENEMY);
    auto flag4 = std::make_unique<Flag>(VGet(-250.0f, 254.0f, 1000.0f), Flag::ENEMY_TYPE::DOG, Flag::STATE::ENEMY);

    auto flag5 = std::make_unique<PlayerFlag>(VGet(-2500.0f, 254.0f, 0.0f), Flag::ENEMY_TYPE::DOG, Flag::STATE::PLAYER);
    auto flag6 = std::make_unique<Flag>(VGet(2500.0f, 254.0f, 4700.0f), Flag::ENEMY_TYPE::DOG, Flag::STATE::ENEMY);

    flags_.push_back(std::move(flag1));
    flags_.push_back(std::move(flag2));
    flags_.push_back(std::move(flag3));
    flags_.push_back(std::move(flag4));

    flags_.push_back(std::move(flag5)); //ÉvÉåÉCÉÑÅ[êwín(é©ï™ÇÃãíì_)
    flags_.push_back(std::move(flag6)); //ìGêwín(ìGÇÃãíì_)

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

FlagBase* FlagManager::GetFlag(int index) const
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

std::vector<FlagBase*> FlagManager::GetPlayerFlags() const
{
    std::vector<FlagBase*> playerFlags;
    for (auto& flag : flags_)
    {
        if (flag->IsOwnedByPlayer() && flag->GetEnemyType() != Flag::ENEMY_TYPE::SABO)
        {
            playerFlags.push_back(flag.get());
        }
    }
    return playerFlags;
}

void FlagManager::Clear()
{
    flags_.clear();
    flagMax_ = 0;
}

void FlagManager::AddFlag(const VECTOR pos, Flag::ENEMY_TYPE type, Flag::STATE state)
{
    auto flag = std::make_unique<Flag>(pos, type, state);

    flag->Init();

    flags_.push_back(std::move(flag));

    flagMax_ = static_cast<int>(flags_.size());
}
