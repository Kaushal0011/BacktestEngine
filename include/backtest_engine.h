#ifndef BACKTEST_ENGINE_H
#define BACKTEST_ENGINE_H

#include "data_structures.h"
#include "strategy/strategy_base.h"
#include "trade_logger.h"
#include <vector>
#include <memory>
#include <string>

namespace backtest
{

    class BacktestEngine
    {
    public:
        BacktestEngine(double initial_capital = 2000000.0);

        // Run backtest for single strategy configuration
        PerformanceMetrics runBacktest(
            const std::vector<Bar> &bars,
            strategy::StrategyBase *strategy,
            const StrategyParams &params,
            TradeLogger &logger);

        // Run backtest for multiple parameter combinations (multithreaded)
        std::vector<PerformanceMetrics> runOptimization(
            const std::vector<Bar> &bars,
            const std::string &strategy_name,
            const std::vector<StrategyParams> &param_combinations,
            const std::string &output_dir);

    private:
        double initial_capital_;

        // Calculate performance metrics from trades
        PerformanceMetrics calculateMetrics(
            const std::vector<Trade> &trades,
            const StrategyParams &params,
            int dte_filter);

        // Check if bar is within trading hours
        bool isWithinTradingHours(const std::string &timestamp) const;

        // Check if should square off
        bool shouldSquareOff(const std::string &timestamp) const;

        // Create strategy instance from name
        std::unique_ptr<strategy::StrategyBase> createStrategy(const std::string &name);
    };

} // namespace backtest

#endif // BACKTEST_ENGINE_H
