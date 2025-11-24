#ifndef TRADE_H
#define TRADE_H

#include <string>
#include <cmath>

enum class TradeDirection {
    LONG,
    SHORT
};

class Trade {
public:
    std::string entry_timestamp;
    std::string exit_timestamp;
    int dte;  // Days to expiry (for intraday, this would be 0)
    double entry_price;
    double exit_price;
    double entry_keltner_upper;
    double entry_keltner_lower;
    double exit_keltner_upper;
    double exit_keltner_lower;
    
    int ema_period;
    int atr_period;
    double multiplier;
    
    double investment;
    int quantity;
    TradeDirection direction;
    
    double max_profit_during_trade;
    double max_drawdown_during_trade;
    
    Trade() : dte(0), entry_price(0), exit_price(0), 
              entry_keltner_upper(0), entry_keltner_lower(0),
              exit_keltner_upper(0), exit_keltner_lower(0),
              ema_period(0), atr_period(0), multiplier(0),
              investment(0), quantity(0), direction(TradeDirection::LONG),
              max_profit_during_trade(0), max_drawdown_during_trade(0) {}
    
    double getProfitLoss() const {
        if (direction == TradeDirection::LONG) {
            return (exit_price - entry_price) * quantity;
        } else {
            return (entry_price - exit_price) * quantity;
        }
    }
    
    double getProfitLossPercentage() const {
        if (investment == 0) return 0;
        return (getProfitLoss() / investment) * 100.0;
    }
    
    double getMaxDrawdownPercentage() const {
        if (investment == 0) return 0;
        return (max_drawdown_during_trade / investment) * 100.0;
    }
    
    double getCalmarRatio() const {
        double max_dd_pct = getMaxDrawdownPercentage();
        if (std::abs(max_dd_pct) < 0.0001) {
            return 0;
        }
        double total_return = getProfitLossPercentage();
        return total_return / std::abs(max_dd_pct);
    }
};

#endif // TRADE_H
