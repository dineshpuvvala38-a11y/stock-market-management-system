#pragma once

#include "Portfolio.h"

#include <string>

namespace market {

class FileStorage;

class User {
public:
    User(std::string id, std::string name, double openingBalance);

    const std::string& id() const noexcept { return id_; }
    const std::string& name() const noexcept { return name_; }
    double balance() const noexcept { return balance_; }
    const Portfolio& portfolio() const noexcept { return portfolio_; }

private:
    friend class TradingService;
    friend class FileStorage;

    std::string id_;
    std::string name_;
    double balance_;
    Portfolio portfolio_;
};

} // namespace market
