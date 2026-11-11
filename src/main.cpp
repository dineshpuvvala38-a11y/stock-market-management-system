#include "FileStorage.h"
#include "MarketCatalog.h"

#include <iomanip>
#include <iostream>
#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>

namespace {
using namespace market;
constexpr const char* DATA_FILE = "stock_market_data.txt";

double readNumber(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        double value = 0.0;
        if (std::cin >> value && std::isfinite(value)) return value;
        if (std::cin.eof()) throw std::runtime_error("Input ended.");
        std::cout << "Enter a valid number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin >> std::ws, value);
    return value;
}

void showStocks(const MarketCatalog& catalog) {
    std::cout << "\nSimulated stock prices (INR)\n";
    for (const auto& stock : catalog.stocks())
        std::cout << "  " << stock.symbol() << " | " << stock.name()
                  << " | Rs " << stock.price() << '\n';
}

void showFunds(const MarketCatalog& catalog) {
    std::cout << "\nSimulated mutual fund NAVs (INR per unit)\n";
    for (const auto& fund : catalog.funds())
        std::cout << "  " << fund.symbol() << " | " << fund.name() << " | "
                  << fund.category() << " | Risk: " << fund.riskLabel()
                  << " | NAV Rs " << fund.price() << '\n';
}

void showPortfolio(const User& user, const MarketCatalog& catalog) {
    std::map<std::string, double> stockPrices, fundNavs;
    for (const auto& stock : catalog.stocks()) stockPrices[stock.symbol()] = stock.price();
    for (const auto& fund : catalog.funds()) fundNavs[fund.symbol()] = fund.price();

    std::cout << "\nCash balance: Rs " << user.balance() << '\n';
    std::cout << "Stocks:\n";
    if (user.portfolio().stockHoldings().empty()) std::cout << "  (none)\n";
    for (const auto& [symbol, holding] : user.portfolio().stockHoldings()) {
        const auto price = stockPrices.find(symbol);
        if (price == stockPrices.end()) {
            std::cout << "  " << symbol << " | current quote unavailable\n";
            continue;
        }
        std::cout << "  " << symbol << " | shares " << holding.quantity
                  << " | avg cost Rs " << holding.averageCost
                  << " | value Rs " << holding.currentValue(price->second)
                  << " | unrealized P/L Rs " << holding.unrealizedProfitLoss(price->second) << '\n';
    }
    std::cout << "Mutual funds:\n";
    if (user.portfolio().fundHoldings().empty()) std::cout << "  (none)\n";
    for (const auto& [symbol, holding] : user.portfolio().fundHoldings()) {
        const auto nav = fundNavs.find(symbol);
        if (nav == fundNavs.end()) {
            std::cout << "  " << symbol << " | current NAV unavailable\n";
            continue;
        }
        std::cout << "  " << symbol << " | units " << holding.quantity
                  << " | avg NAV Rs " << holding.averageCost
                  << " | value Rs " << holding.currentValue(nav->second)
                  << " | unrealized P/L Rs " << holding.unrealizedProfitLoss(nav->second) << '\n';
    }
}

const char* transactionName(TransactionType type) {
    switch (type) {
        case TransactionType::BuyShares: return "Buy shares";
        case TransactionType::SellShares: return "Sell shares";
        case TransactionType::BuyFundUnits: return "Invest in fund";
        case TransactionType::RedeemFundUnits: return "Redeem fund units";
    }
    return "Unknown";
}

void showHistory(const TradingService& service) {
    std::cout << "\nTransaction history\n";
    if (service.history().empty()) {
        std::cout << "  (no transactions yet)\n";
        return;
    }
    for (const auto& tx : service.history())
        std::cout << "  " << tx.id << " | " << tx.timestamp << " | "
                  << transactionName(tx.type) << " | " << tx.instrumentSymbol
                  << " | qty " << tx.quantity << " | Rs " << tx.totalAmount << '\n';
}

void printMenu() {
    std::cout << "\n=== Simulated Stock and Mutual Fund Manager ===\n"
              << "1. List/search stocks\n2. List/search mutual funds\n"
              << "3. Buy shares\n4. Sell shares\n5. Invest in a mutual fund\n"
              << "6. Redeem mutual fund units\n7. View portfolio\n"
              << "8. View transaction history\n0. Save and exit\n";
}
} // namespace

int main() {
    using namespace market;
    std::cout << std::fixed << std::setprecision(2);
    try {
        User user("DEMO01", "Demo Investor", 10000.00);
        TradingService trading;
        MarketCatalog catalog;
        if (FileStorage::load(DATA_FILE, user, trading))
            std::cout << "Loaded saved account for " << user.name() << ".\n";
        else
            std::cout << "Starting demo account with Rs 10000.00.\n";
        std::cout << "All listed quotes and NAVs are simulated sample values.\n";

        bool running = true;
        while (running) {
            printMenu();
            const double choice = readNumber("Choose an option: ");
            if (choice != static_cast<int>(choice)) {
                std::cout << "Choose a whole-number menu option.\n";
                continue;
            }
            try {
                switch (static_cast<int>(choice)) {
                    case 1: {
                        showStocks(catalog);
                        const auto query = readLine("Search by symbol/name (or press Enter to skip): ");
                        if (!query.empty()) {
                            const auto& stock = catalog.findStock(query);
                            std::cout << stock.symbol() << " | " << stock.name()
                                      << " | Simulated Rs " << stock.price() << '\n';
                        }
                        break;
                    }
                    case 2: {
                        showFunds(catalog);
                        const auto query = readLine("Search by symbol/name (or press Enter to skip): ");
                        if (!query.empty()) {
                            const auto& fund = catalog.findFund(query);
                            std::cout << fund.symbol() << " | " << fund.name() << " | "
                                      << fund.category() << " | Simulated NAV Rs " << fund.price() << '\n';
                        }
                        break;
                    }
                    case 3: {
                        const auto& stock = catalog.findStock(readLine("Stock symbol/name: "));
                        trading.buyShares(user, stock, readNumber("Shares to buy: "));
                        std::cout << "Purchase completed.\n";
                        break;
                    }
                    case 4: {
                        const auto& stock = catalog.findStock(readLine("Stock symbol/name: "));
                        trading.sellShares(user, stock, readNumber("Shares to sell: "));
                        std::cout << "Sale completed.\n";
                        break;
                    }
                    case 5: {
                        const auto& fund = catalog.findFund(readLine("Fund symbol/name: "));
                        trading.investInFund(user, fund, readNumber("Amount to invest (Rs): "));
                        std::cout << "Investment completed.\n";
                        break;
                    }
                    case 6: {
                        const auto& fund = catalog.findFund(readLine("Fund symbol/name: "));
                        trading.redeemFundUnits(user, fund, readNumber("Units to redeem: "));
                        std::cout << "Redemption completed.\n";
                        break;
                    }
                    case 7: showPortfolio(user, catalog); break;
                    case 8: showHistory(trading); break;
                    case 0:
                        FileStorage::save(DATA_FILE, user, trading);
                        std::cout << "Saved to " << DATA_FILE << ". Goodbye.\n";
                        running = false;
                        break;
                    default: std::cout << "Choose one of the listed options.\n";
                }
            } catch (const std::exception& error) {
                std::cout << "Could not complete that action: " << error.what() << '\n';
            }
        }
    } catch (const std::exception& error) {
        std::cerr << "Application error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
