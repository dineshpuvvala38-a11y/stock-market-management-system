#pragma once

#include "Instrument.h"
#include "User.h"

#include <string>
#include <vector>

namespace market {

class FileStorage;

enum class TransactionType { BuyShares, SellShares, BuyFundUnits, RedeemFundUnits };

struct Transaction {
    std::string id;
    std::string instrumentSymbol;
    TransactionType type;
    double quantity;
    double unitPrice;
    double totalAmount;
    std::string timestamp;
};

class TradingService {
public:
    void buyShares(User& user, const Stock& stock, double quantity);
    void sellShares(User& user, const Stock& stock, double quantity);
    void investInFund(User& user, const MutualFund& fund, double amount);
    void redeemFundUnits(User& user, const MutualFund& fund, double units);

    const std::vector<Transaction>& history() const noexcept { return history_; }

private:
    friend class FileStorage;
    void record(const std::string& symbol, TransactionType type, double quantity,
                double unitPrice, double totalAmount);
    std::vector<Transaction> history_;
    unsigned long long nextTransactionId_ = 1;
};

} // namespace market
