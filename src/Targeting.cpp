#include "Targeting.h"

#include <cctype>

std::optional<std::uint64_t> SelectLowestEnemyStartingWith(
    const std::vector<Enemy>& enemies,
    char firstCharacter
) {
    const char wanted = static_cast<char>(std::tolower(static_cast<unsigned char>(firstCharacter)));
    const Enemy* best = nullptr;
    for (const auto& enemy : enemies) {
        if (!enemy.IsActive() || enemy.GetWord().empty() || enemy.GetTypedProgress() != 0) {
            continue;
        }
        if (enemy.GetWord().front() == wanted && (best == nullptr || enemy.GetPosition().y > best->GetPosition().y)) {
            best = &enemy;
        }
    }
    if (best == nullptr) {
        return std::nullopt;
    }
    return best->GetId();
}
