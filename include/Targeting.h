#pragma once

#include "Enemy.h"

#include <cstdint>
#include <optional>
#include <vector>

std::optional<std::uint64_t> SelectLowestEnemyStartingWith(
    const std::vector<Enemy>& enemies,
    char firstCharacter
);

