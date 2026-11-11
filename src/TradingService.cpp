#include "TradingService.h"

#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace market {
namespace {
void validatePositive(double value, const char* label) {
    if (!std::isfinite(value) || value <= 0.0) {
        throw std::invalid_argument(std::string(label) + " must be positive and finite.");
    }
}

std::string currentTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &time);
#else
    localtime_r(&time, &localTime);
#endif
    std::ostringstream output;
    output << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return output.str();
}
} // namespace

void TradingService::buyShares(User& user, const Stock& stock, double quantity) {
    validatePositive(quantity, "Share quantity");
    const double cost = quantity * stock.price();
    if (!std::isfinite(cost) || cost > user.balance_) {
        throw std::runtime_error("Insufficient balance to buy these shares.");
    }
    user.portfolio_.addShares(stock.symbol(), quantity, stock.price());
    user.balance_ -= cost;
    record(stock.symbol(), TransactionType::BuyShares, quantity, stock.price(), cost);
}

void TradingService::sellShares(User& user, const Stock& stock, double quantity) {
    validatePositive(quantity, "Share quantity");
    const double proceeds = quantity * stock.price();
    if (!std::isfinite(proceeds)) {
        throw std::overflow_error("Sale amount is too large.");
    }
    user.portfolio_.removeShares(stock.symbol(), quantity);
    user.balance_ += proceeds;
    record(stock.symbol(), TransactionType::SellShares, quantity, stock.price(), proceeds);
}

void TradingService::investInFund(User& user, const MutualFund& fund, double amount) {
    validatePositive(amount, "Investment amount");
    if (amount > user.balance_) {
        throw std::runtime_error("Insufficient balance to invest this amount.");
    }
    const double units = amount / fund.price();
    if (!std::isfinite(units) || units <= 0.0) {
        throw std::overflow_error("Investment produces an invalid number of fund units.");
    }
    user.portfolio_.addFundUnits(fund.symbol(), units, fund.price());
    user.balance_ -= amount;
    record(fund.symbol(), TransactionType::BuyFundUnits, units, fund.price(), amount);
}

void TradingService::redeemFundUnits(User& user, const MutualFund& fund, double units) {
    validatePositive(units, "Fund unit quantity");
    const double proceeds = units * fund.price();
    if (!std::isfinite(proceeds)) {
        throw std::overflow_error("Redemption amount is too large.");
    }
    user.portfolio_.removeFundUnits(fund.symbol(), units);
    user.balance_ += proceeds;
    record(fund.symbol(), TransactionType::RedeemFundUnits, units, fund.price(), proceeds);
}

void TradingService::record(const std::string& symbol, TransactionType type,
                            double quantity, double unitPrice, double amount) {
    std::ostringstream id;
    id << "TXN" << std::setw(6) << std::setfill('0') << nextTransactionId_++;
    history_.push_back(Transaction{id.str(), symbol, type, quantity, unitPrice,
                                   amount, currentTimestamp()});
}

} // namespace market
