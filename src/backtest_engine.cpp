#include "backtest_engine.h"
#include "strategy/ema_crossover.h"
#include "strategy/supertrend_strategy.h"
#include <thread>
#include <future>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace backtest
{

    BacktestEngine::BacktestEngine(double initial_capital)
        : initial_capital_(initial_capital) {}

    std::unique_ptr<strategy::StrategyBase> BacktestEngine::createStrategy(const std::string &name)
    {
        if (name == "EMA_Crossover")
        {
            return std::make_unique<strategy::EMACrossover>();
        }
        else if (name == "Supertrend")
        {
            return std::make_unique<strategy::SupertrendStrategy>();
        }
        return nullptr;
    }

    bool BacktestEngine::isWithinTradingHours(const std::string &timestamp) const
    {
        // Extract time from timestamp (format: YYYY-MM-DD HH:MM:SS+05:30)
        size_t space_pos = timestamp.find(' ');
        if (space_pos == std::string::npos)
            return false;

        std::string time_str = timestamp.substr(space_pos + 1, 5); // HH:MM
        int hour = std::stoi(time_str.substr(0, 2));
        int minute = std::stoi(time_str.substr(3, 2));

        int time_minutes = hour * 60 + minute;
        int start_time = 9 * 60 + 15; // 09:15
        int end_time = 15 * 60 + 25;  // 15:25

        return time_minutes >= start_time && time_minutes <= end_time;
    }

    bool BacktestEngine::shouldSquareOff(const std::string &timestamp) const
    {
        size_t space_pos = timestamp.find(' ');
        if (space_pos == std::string::npos)
            return false;

        std::string time_str = timestamp.substr(space_pos + 1, 5);
        int hour = std::stoi(time_str.substr(0, 2));
        int minute = std::stoi(time_str.substr(3, 2));

        int time_minutes = hour * 60 + minute;
        int square_off_time = 15 * 60 + 25; // 15:25

        return time_minutes >= square_off_time;
    }

    PerformanceMetrics BacktestEngine::runBacktest(
        const std::vector<Bar> &bars,
        strategy::StrategyBase *strategy,
        const StrategyParams &params,
        TradeLogger &logger)
    {
        // Initialize strategy
        strategy->initialize(params);
        strategy->calculateIndicators(bars);

        // Trading state
        bool in_position = false;
        Trade current_trade;
        double quantity = 0.0;

        // Iterate through bars
        for (size_t i = 0; i < bars.size(); ++i)
        {
            const Bar &bar = bars[i];

            // Check DTE filter
            if (params.dte_filter != -1 && bar.dte != params.dte_filter)
            {
                // Square off if we're in position and DTE changed
                if (in_position && shouldSquareOff(bar.timestamp))
                {
                    current_trade.exit_time = bar.timestamp;
                    current_trade.exit_date = bar.date;
                    current_trade.exit_price = bar.close;

                    if (current_trade.direction == "LONG")
                    {
                        current_trade.pnl = (current_trade.exit_price - current_trade.entry_price) * quantity;
                    }
                    else
                    {
                        current_trade.pnl = (current_trade.entry_price - current_trade.exit_price) * quantity;
                    }
                    current_trade.pnl_percentage = (current_trade.pnl / (current_trade.entry_price * quantity)) * 100.0;

                    logger.logTrade(current_trade);
                    in_position = false;
                }
                continue;
            }

            // Skip if not in trading hours
            if (!isWithinTradingHours(bar.timestamp))
            {
                continue;
            }

            // Square off at end of day
            if (in_position && shouldSquareOff(bar.timestamp))
            {
                current_trade.exit_time = bar.timestamp;
                current_trade.exit_date = bar.date;
                current_trade.exit_price = bar.close;

                if (current_trade.direction == "LONG")
                {
                    current_trade.pnl = (current_trade.exit_price - current_trade.entry_price) * quantity;
                }
                else
                {
                    current_trade.pnl = (current_trade.entry_price - current_trade.exit_price) * quantity;
                }
                current_trade.pnl_percentage = (current_trade.pnl / (current_trade.entry_price * quantity)) * 100.0;

                logger.logTrade(current_trade);
                in_position = false;
                continue;
            }

            // Generate signal
            if (!strategy->isReady(i))
            {
                continue;
            }

            strategy::Signal signal = strategy->generateSignal(i, bars);

            // Handle signals
            if (signal == strategy::Signal::LONG && !in_position)
            {
                in_position = true;
                quantity = std::floor(initial_capital_ / bar.close);

                current_trade = Trade();
                current_trade.entry_time = bar.timestamp;
                current_trade.entry_date = bar.date;
                current_trade.entry_price = bar.close;
                current_trade.quantity = quantity;
                current_trade.direction = "LONG";
                current_trade.dte = bar.dte;
                current_trade.strategy_name = strategy->getName();
                current_trade.parameters = params.to_string();
            }
            else if (signal == strategy::Signal::SHORT && !in_position)
            {
                in_position = true;
                quantity = std::floor(initial_capital_ / bar.close);

                current_trade = Trade();
                current_trade.entry_time = bar.timestamp;
                current_trade.entry_date = bar.date;
                current_trade.entry_price = bar.close;
                current_trade.quantity = quantity;
                current_trade.direction = "SHORT";
                current_trade.dte = bar.dte;
                current_trade.strategy_name = strategy->getName();
                current_trade.parameters = params.to_string();
            }
            else if ((signal == strategy::Signal::EXIT_LONG || signal == strategy::Signal::EXIT_SHORT) && in_position)
            {
                current_trade.exit_time = bar.timestamp;
                current_trade.exit_date = bar.date;
                current_trade.exit_price = bar.close;

                if (current_trade.direction == "LONG")
                {
                    current_trade.pnl = (current_trade.exit_price - current_trade.entry_price) * quantity;
                }
                else
                {
                    current_trade.pnl = (current_trade.entry_price - current_trade.exit_price) * quantity;
                }
                current_trade.pnl_percentage = (current_trade.pnl / (current_trade.entry_price * quantity)) * 100.0;

                logger.logTrade(current_trade);
                in_position = false;
            }
        }

        // Calculate and return metrics
        return calculateMetrics(logger.getTrades(), params, params.dte_filter);
    }

    PerformanceMetrics BacktestEngine::calculateMetrics(
        const std::vector<Trade> &trades,
        const StrategyParams &params,
        int dte_filter)
    {
        PerformanceMetrics metrics;
        metrics.strategy_params = params.to_string();
        metrics.dte = dte_filter;

        if (trades.empty())
        {
            return metrics;
        }

        double total_wins = 0.0;
        double total_losses = 0.0;
        int current_wins = 0, current_losses = 0;

        for (const auto &trade : trades)
        {
            metrics.total_trades++;
            metrics.total_pnl += trade.pnl;

            if (trade.pnl > 0)
            {
                metrics.winning_trades++;
                total_wins += trade.pnl;
                metrics.max_win = std::max(metrics.max_win, trade.pnl);
                current_wins++;
                current_losses = 0;
                metrics.consecutive_wins = std::max(metrics.consecutive_wins, current_wins);
            }
            else
            {
                metrics.losing_trades++;
                total_losses += std::abs(trade.pnl);
                metrics.max_loss = std::min(metrics.max_loss, trade.pnl);
                current_losses++;
                current_wins = 0;
                metrics.consecutive_losses = std::max(metrics.consecutive_losses, current_losses);
            }
        }

        metrics.total_return_pct = (metrics.total_pnl / initial_capital_) * 100.0;
        metrics.win_rate = (static_cast<double>(metrics.winning_trades) / metrics.total_trades) * 100.0;

        if (metrics.winning_trades > 0)
        {
            metrics.avg_win = total_wins / metrics.winning_trades;
        }
        if (metrics.losing_trades > 0)
        {
            metrics.avg_loss = total_losses / metrics.losing_trades;
        }

        if (total_losses > 0)
        {
            metrics.profit_factor = total_wins / total_losses;
        }

        metrics.expectancy = (metrics.win_rate / 100.0) * metrics.avg_win -
                             ((100.0 - metrics.win_rate) / 100.0) * metrics.avg_loss;

        // Calculate max drawdown
        double peak = initial_capital_;
        double current_capital = initial_capital_;
        double max_dd = 0.0;

        for (const auto &trade : trades)
        {
            current_capital += trade.pnl;
            if (current_capital > peak)
            {
                peak = current_capital;
            }
            double dd = ((peak - current_capital) / peak) * 100.0;
            max_dd = std::max(max_dd, dd);
        }
        metrics.max_drawdown = max_dd;

        return metrics;
    }

    std::vector<PerformanceMetrics> BacktestEngine::runOptimization(
        const std::vector<Bar> &bars,
        const std::string &strategy_name,
        const std::vector<StrategyParams> &param_combinations,
        const std::string &output_dir)
    {
        std::cout << "\n=== Running Optimization ===" << std::endl;
        std::cout << "Strategy: " << strategy_name << std::endl;
        std::cout << "Parameter combinations: " << param_combinations.size() << std::endl;
        std::cout << "Using " << std::thread::hardware_concurrency() << " threads\n"
                  << std::endl;

        std::vector<PerformanceMetrics> all_metrics;
        std::mutex metrics_mutex;

        // Worker function for each thread
        auto worker = [&](const StrategyParams &params)
        {
            auto strategy = createStrategy(strategy_name);
            if (!strategy)
            {
                std::cerr << "Unknown strategy: " << strategy_name << std::endl;
                return;
            }

            TradeLogger logger;
            PerformanceMetrics metrics = runBacktest(bars, strategy.get(), params, logger);

            // Save trades to parquet
            std::string filename = output_dir + "/trades_" + params.to_string() + ".parquet";
            logger.saveToParquet(filename);

            // Add metrics
            {
                std::lock_guard<std::mutex> lock(metrics_mutex);
                all_metrics.push_back(metrics);

                // Progress indicator
                std::cout << "Completed: " << params.to_string()
                          << " | Trades: " << metrics.total_trades
                          << " | PnL: " << std::fixed << std::setprecision(2) << metrics.total_pnl
                          << " | Return: " << metrics.total_return_pct << "%"
                          << std::endl;
            }
        };

        // Launch threads
        std::vector<std::thread> threads;
        size_t num_threads = std::thread::hardware_concurrency();
        size_t combinations_per_thread = (param_combinations.size() + num_threads - 1) / num_threads;

        for (size_t t = 0; t < num_threads; ++t)
        {
            size_t start = t * combinations_per_thread;
            size_t end = std::min(start + combinations_per_thread, param_combinations.size());

            if (start >= param_combinations.size())
                break;

            threads.emplace_back([&, start, end]()
                                 {
            for (size_t i = start; i < end; ++i) {
                worker(param_combinations[i]);
            } });
        }

        // Wait for all threads
        for (auto &thread : threads)
        {
            thread.join();
        }

        std::cout << "\n=== Optimization Complete ===" << std::endl;
        std::cout << "Total results: " << all_metrics.size() << std::endl;

        return all_metrics;
    }

} // namespace backtest
