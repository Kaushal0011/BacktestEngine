#ifndef BACKTEST_ENGINE_H
#define BACKTEST_ENGINE_H

#include <vector>
#include <thread>
#include <mutex>
#include <future>
#include <iostream>
#include "MarketData.hpp"
#include "KeltnerIndicator.hpp"
#include "TradingStrategy.hpp"
#include "BacktestResult.hpp"

struct ParameterSet
{
    int ema_period;
    int atr_period;
    double multiplier;

    ParameterSet(int ema, int atr, double mult)
        : ema_period(ema), atr_period(atr), multiplier(mult) {}
};

class BacktestEngine
{
private:
    MarketData market_data;
    double capital;
    std::vector<BacktestResult> results;
    std::mutex results_mutex;

    BacktestResult runSingleBacktest(const ParameterSet &params)
    {
        KeltnerIndicator indicator(params.ema_period, params.atr_period, params.multiplier);
        TradingStrategy strategy(capital);

        std::vector<Trade> trades = strategy.executeBacktest(market_data, indicator);

        BacktestResult result(params.ema_period, params.atr_period, params.multiplier);
        result.trades = trades;
        result.calculateMetrics();

        return result;
    }

public:
    BacktestEngine(double initial_capital) : capital(initial_capital) {}

    void loadMarketData(const MarketData &data)
    {
        market_data = data;
    }

    void runParallelBacktests(const std::vector<ParameterSet> &parameter_sets,
                              int num_threads = 4)
    {
        results.clear();

        std::cout << "\n========================================" << std::endl;
        std::cout << "Starting Parallel Backtests" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Total parameter combinations: " << parameter_sets.size() << std::endl;
        std::cout << "Number of threads: " << num_threads << std::endl;
        std::cout << "Capital per backtest: Rs " << capital << std::endl;
        std::cout << "Market data bars: " << market_data.size() << std::endl;
        std::cout << "========================================\n"
                  << std::endl;

        std::vector<std::future<BacktestResult> > futures;

        for (const auto &params : parameter_sets)
        {
            futures.push_back(std::async(std::launch::async,
                                         [this, params]()
                                         {
                                             BacktestResult result = this->runSingleBacktest(params);

                                             std::lock_guard<std::mutex> lock(this->results_mutex);
                                             std::cout << "Completed: EMA=" << params.ema_period
                                                       << ", ATR=" << params.atr_period
                                                       << ", Mult=" << params.multiplier
                                                       << " | Trades: " << result.trades.size()
                                                       << " | P&L: Rs " << result.total_profit_loss
                                                       << std::endl;

                                             return result;
                                         }));

            if (futures.size() >= static_cast<size_t>(num_threads))
            {
                for (auto &future : futures)
                {
                    results.push_back(future.get());
                }
                futures.clear();
            }
        }

        for (auto &future : futures)
        {
            results.push_back(future.get());
        }

        std::cout << "\n========================================" << std::endl;
        std::cout << "All backtests completed!" << std::endl;
        std::cout << "Total results: " << results.size() << std::endl;
        std::cout << "========================================\n"
                  << std::endl;
    }

    const std::vector<BacktestResult> &getResults() const
    {
        return results;
    }

    void printSummary() const
    {
        std::cout << "\n========================================" << std::endl;
        std::cout << "BACKTEST SUMMARY" << std::endl;
        std::cout << "========================================\n"
                  << std::endl;

        for (const auto &result : results)
        {
            std::cout << result.getSummary() << std::endl;
        }
    }
};

#endif // BACKTEST_ENGINE_H
