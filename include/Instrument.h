#pragma once

#include <string>

namespace market {

// Shared interface for assets whose displayed prices are simulated by this app.
class Instrument {
public:
    Instrument(std::string symbol, std::string name, double price);
    virtual ~Instrument() = default;

    const std::string& symbol() const noexcept { return symbol_; }
    const std::string& name() const noexcept { return name_; }
    double price() const noexcept { return price_; }

    virtual std::string type() const = 0;
    virtual void updatePrice(double newPrice);

private:
    std::string symbol_;
    std::string name_;
    double price_;
};

class Stock final : public Instrument {
public:
    Stock(std::string symbol, std::string companyName, double simulatedPrice);
    std::string type() const override { return "Stock"; }
};

class MutualFund final : public Instrument {
public:
    MutualFund(std::string symbol, std::string fundName, std::string category,
               std::string riskLabel, double simulatedNav);

    const std::string& category() const noexcept { return category_; }
    const std::string& riskLabel() const noexcept { return riskLabel_; }
    std::string type() const override { return "Mutual Fund"; }

private:
    std::string category_;
    std::string riskLabel_;
};

} // namespace market
