#include "MarketCatalog.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace market {
namespace {
std::string normalized(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });                                                                                                                                                                                                         
    return value;
}
} // namespace

MarketCatalog::MarketCatalog()
    : stocks_{Stock("ALPH", "Alpha Technologies", 184.50),
              Stock("NOVA", "Nova Motors", 92.75),
              Stock("ORBT", "Orbit Energy", 246.20)},
      funds_{MutualFund("EQF01", "Sample Equity Fund", "Equity", "High", 50.00),
             MutualFund("HYF02", "Sample Hybrid Fund", "Hybrid", "Moderate", 32.40),
             MutualFund("DBF03", "Sample Debt Fund", "Debt", "Low", 25.00)} {}

const Stock& MarketCatalog::findStock(const std::string& query) const {
    const auto key = normalized(query);
    const auto found = std::find_if(stocks_.begin(), stocks_.end(), [&](const Stock& stock) {
        return normalized(stock.symbol()) == key || normalized(stock.name()).find(key) != std::string::npos;
    });
    if (found == stocks_.end()) throw std::out_of_range("Stock not found: " + query);
    return *found;
}

const MutualFund& MarketCatalog::findFund(const std::string& query) const {
    const auto key = normalized(query);
    const auto found = std::find_if(funds_.begin(), funds_.end(), [&](const MutualFund& fund) {
        return normalized(fund.symbol()) == key || normalized(fund.name()).find(key) != std::string::npos;
    });
    if (found == funds_.end()) throw std::out_of_range("Mutual fund not found: " + query);
    return *found;
}

} // namespace market
