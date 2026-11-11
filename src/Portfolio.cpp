#include "Portfolio.h"

#include <cmath>
#include <stdexcept>

namespace market {
namespace {
void validatePositive(double value, const char* fieldName) {
    if (!std::isfinite(value) || value <= 0.0) {
        throw std::invalid_argument(std::string(fieldName) +
                                    " must be a positive finite value.");
    }
}
} // namespace

double Holding::currentValue(double currentPrice) const {
    validatePositive(currentPrice, "Current price");
    return quantity * currentPrice;
}

double Holding::unrealizedProfitLoss(double currentPrice) const {
    validatePositive(currentPrice, "Current price");
    return quantity * (currentPrice - averageCost);
}

void Portfolio::addShares(const std::string& symbol, double quantity,
                          double purchasePrice) {
    addToHoldings(stocks_, symbol, quantity, purchasePrice);
}

void Portfolio::removeShares(const std::string& symbol, double quantity) {
    removeFromHoldings(stocks_, symbol, quantity);
}

void Portfolio::addFundUnits(const std::string& symbol, double units, double nav) {
    addToHoldings(funds_, symbol, units, nav);
}

void Portfolio::removeFundUnits(const std::string& symbol, double units) {
    removeFromHoldings(funds_, symbol, units);
}

void Portfolio::addToHoldings(std::map<std::string, Holding>& holdings,
                              const std::string& symbol, double quantity,
                              double purchasePrice) {
    if (symbol.empty()) {
        throw std::invalid_argument("Holding symbol cannot be empty.");
    }
    validatePositive(quantity, "Quantity");
    validatePositive(purchasePrice, "Purchase price");

    Holding& holding = holdings[symbol];
    const double previousCost = holding.quantity * holding.averageCost;
    const double addedCost = quantity * purchasePrice;
    holding.quantity += quantity;
    holding.averageCost = (previousCost + addedCost) / holding.quantity;
}

void Portfolio::removeFromHoldings(std::map<std::string, Holding>& holdings,
                                   const std::string& symbol, double quantity) {
    validatePositive(quantity, "Quantity");
    const auto found = holdings.find(symbol);
    if (found == holdings.end() || found->second.quantity < quantity) {
        throw std::runtime_error("Cannot sell or redeem more than the quantity held.");
    }

    found->second.quantity -= quantity;
    if (found->second.quantity < 1e-9) {
        holdings.erase(found);
    }
}

double Portfolio::stockValue(
    const std::map<std::string, double>& currentPrices) const {
    return totalValue(stocks_, currentPrices);
}

double Portfolio::fundValue(
    const std::map<std::string, double>& currentNavs) const {
    return totalValue(funds_, currentNavs);
}

double Portfolio::totalValue(const std::map<std::string, Holding>& holdings,
                             const std::map<std::string, double>& prices) {
    double total = 0.0;
    for (const auto& [symbol, holding] : holdings) {
        const auto price = prices.find(symbol);
        if (price == prices.end()) {
            throw std::runtime_error("Missing current price for " + symbol + ".");
        }
        total += holding.currentValue(price->second);
    }
    return total;
}

} // namespace market
