#include "Instrument.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace market {

Instrument::Instrument(std::string symbol, std::string name, double price)
    : symbol_(std::move(symbol)), name_(std::move(name)), price_(price) {
    if (symbol_.empty() || name_.empty()) {
        throw std::invalid_argument("Instrument symbol and name cannot be empty.");
    }
    if (!std::isfinite(price_) || price_ <= 0.0) {
        throw std::invalid_argument("Instrument price must be a positive finite value.");
    }
}

void Instrument::updatePrice(double newPrice) {
    if (!std::isfinite(newPrice) || newPrice <= 0.0) {
        throw std::invalid_argument("Updated price must be a positive finite value.");
    }
    price_ = newPrice;
}

Stock::Stock(std::string symbol, std::string companyName, double simulatedPrice)
    : Instrument(std::move(symbol), std::move(companyName), simulatedPrice) {}

MutualFund::MutualFund(std::string symbol, std::string fundName,
                       std::string category, std::string riskLabel,
                       double simulatedNav)
    : Instrument(std::move(symbol), std::move(fundName), simulatedNav),
      category_(std::move(category)), riskLabel_(std::move(riskLabel)) {
    if (category_.empty() || riskLabel_.empty()) {
        throw std::invalid_argument("Fund category and risk label cannot be empty.");
    }
}

} // namespace market
