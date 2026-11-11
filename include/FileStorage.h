#pragma once

#include "TradingService.h"

#include <string>

namespace market {

class FileStorage {
public:
    static void save(const std::string& path, const User& user,
                     const TradingService& tradingService);
    static bool load(const std::string& path, User& user,
                     TradingService& tradingService);
};

} // namespace market
