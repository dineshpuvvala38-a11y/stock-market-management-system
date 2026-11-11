#include "FileStorage.h"

#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <utility>
#include <vector>

namespace market {

void FileStorage::save(const std::string& path, const User& user,
                       const TradingService& tradingService) {
    std::ofstream output(path, std::ios::trunc);
    if (!output) throw std::runtime_error("Could not open data file for writing: " + path);
    output << std::setprecision(17);
    output << "USER " << std::quoted(user.id_) << ' ' << std::quoted(user.name_)
           << ' ' << user.balance_ << '\n';
    for (const auto& [symbol, holding] : user.portfolio_.stockHoldings())
        output << "STOCK " << std::quoted(symbol) << ' ' << holding.quantity << ' '
               << holding.averageCost << '\n';
    for (const auto& [symbol, holding] : user.portfolio_.fundHoldings())
        output << "FUND " << std::quoted(symbol) << ' ' << holding.quantity << ' '
               << holding.averageCost << '\n';
    for (const auto& tx : tradingService.history_)
        output << "TX " << std::quoted(tx.id) << ' ' << std::quoted(tx.instrumentSymbol)
               << ' ' << static_cast<int>(tx.type) << ' ' << tx.quantity << ' '
               << tx.unitPrice << ' ' << tx.totalAmount << ' '
               << std::quoted(tx.timestamp) << '\n';
    if (!output) throw std::runtime_error("Failed while writing data file: " + path);
}

bool FileStorage::load(const std::string& path, User& user,
                       TradingService& tradingService) {
    std::ifstream input(path);
    if (!input) return false;
    Portfolio loadedPortfolio;
    std::vector<Transaction> loadedHistory;
    std::string recordType;
    bool foundUser = false;
    double loadedBalance = 0.0;
    while (input >> recordType) {
        if (recordType == "USER") {
            std::string id, name;
            if (!(input >> std::quoted(id) >> std::quoted(name) >> loadedBalance) ||
                id != user.id_ || name != user.name_ || loadedBalance < 0.0)
                throw std::runtime_error("Invalid or mismatched USER record in data file.");
            foundUser = true;
        } else if (recordType == "STOCK" || recordType == "FUND") {
            std::string symbol;
            double quantity = 0.0, averageCost = 0.0;
            if (!(input >> std::quoted(symbol) >> quantity >> averageCost))
                throw std::runtime_error("Malformed holding record in data file.");
            if (recordType == "STOCK") loadedPortfolio.addShares(symbol, quantity, averageCost);
            else loadedPortfolio.addFundUnits(symbol, quantity, averageCost);
        } else if (recordType == "TX") {
            Transaction tx{};
            int type = -1;
            if (!(input >> std::quoted(tx.id) >> std::quoted(tx.instrumentSymbol) >> type
                       >> tx.quantity >> tx.unitPrice >> tx.totalAmount
                       >> std::quoted(tx.timestamp)) || type < 0 || type > 3)
                throw std::runtime_error("Malformed transaction record in data file.");
            tx.type = static_cast<TransactionType>(type);
            loadedHistory.push_back(std::move(tx));
        } else {
            throw std::runtime_error("Unknown record in data file: " + recordType);
        }
    }
    if (!input.eof()) throw std::runtime_error("Could not read data file: " + path);
    if (!foundUser) throw std::runtime_error("Data file has no USER record.");

    user.balance_ = loadedBalance;
    user.portfolio_ = std::move(loadedPortfolio);
    tradingService.history_ = std::move(loadedHistory);
    tradingService.nextTransactionId_ = tradingService.history_.size() + 1;
    return true;
}

} // namespace market
