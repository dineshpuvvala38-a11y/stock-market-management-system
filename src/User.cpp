#include "User.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace market {

User::User(std::string id, std::string name, double openingBalance)
    : id_(std::move(id)), name_(std::move(name)), balance_(openingBalance) {
    if (id_.empty() || name_.empty()) {
        throw std::invalid_argument("User ID and name cannot be empty.");
    }
    if (!std::isfinite(balance_) || balance_ < 0.0) {
        throw std::invalid_argument("Opening balance cannot be negative or non-finite.");
    }
}

} // namespace market
