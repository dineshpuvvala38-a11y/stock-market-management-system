#pragma once

#include <map>
#include <string>

namespace market {

// Records the quantity owned and the weighted average purchase price.
struct Holding {
    double quantity = 0.0;
    double averageCost = 0.0;

    double currentValue(double currentPrice) const;
    double unrealizedProfitLoss(double currentPrice) const;
};

// Owns a collection of stock share holdings and mutual-fund unit holdings.
class Portfolio {
public:
    void addShares(const std::string& symbol, double quantity, double purchasePrice);
    void removeShares(const std::string& symbol, double quantity);
    void addFundUnits(const std::string& symbol, double units, double nav);
    void removeFundUnits(const std::string& symbol, double units);

    const std::map<std::string, Holding>& stockHoldings() const noexcept {
        return stocks_;
    }
    const std::map<std::string, Holding>& fundHoldings() const noexcept {
        return funds_;
    }

    double stockValue(const std::map<std::string, double>& currentPrices) const;
    double fundValue(const std::map<std::string, double>& currentNavs) const;

private:
    static void addToHoldings(std::map<std::string, Holding>& holdings,
                              const std::string& symbol, double quantity,
                              double purchasePrice);
    static void removeFromHoldings(std::map<std::string, Holding>& holdings,
                                   const std::string& symbol, double quantity);
    static double totalValue(const std::map<std::string, Holding>& holdings,
                             const std::map<std::string, double>& prices);

    std::map<std::string, Holding> stocks_;
    std::map<std::string, Holding> funds_;
};

} // namespace market
