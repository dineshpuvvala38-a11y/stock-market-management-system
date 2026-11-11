#pragma once

#include "Instrument.h"

#include <string>
#include <vector>

namespace market {

class MarketCatalog {
public:
    MarketCatalog();
    const std::vector<Stock>& stocks() const noexcept { return stocks_; }
    const std::vector<MutualFund>& funds() const noexcept { return funds_; }
    const Stock& findStock(const std::string& query) const;
    const MutualFund& findFund(const std::string& query) const;

private:
    std::vector<Stock> stocks_;
    std::vector<MutualFund> funds_;
};

} // namespace market
