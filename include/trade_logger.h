#ifndef TRADE_LOGGER_H
#define TRADE_LOGGER_H

#include "data_structures.h"
#include <vector>
#include <string>
#include <mutex>

namespace backtest {

class TradeLogger {
public:
    TradeLogger();
    
    // Add trade to buffer
    void logTrade(const Trade& trade);
    
    // Get all trades
    const std::vector<Trade>& getTrades() const { return trades_; }
    
    // Save trades to Parquet file
    bool saveToParquet(const std::string& filepath);
    
    // Clear trade buffer
    void clear();
    
    // Thread-safe version
    void logTradeThreadSafe(const Trade& trade);
    
private:
    std::vector<Trade> trades_;
    std::mutex mutex_;
};

} // namespace backtest

#endif // TRADE_LOGGER_H
