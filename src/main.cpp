#include "data_loader.h"
#include "backtest_engine.h"
#include "strategy/ema_crossover.h"
#include "strategy/supertrend_strategy.h"
#include <iostream>
#include <filesystem>
#include <vector>

using namespace backtest;

void printUsage()
{
    std::cout << "\nBacktest Engine - High Performance Trading Strategy Tester\n"
              << std::endl;
    std::cout << "Usage: ./backtest_engine [options]\n"
              << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --convert-csv      Convert market_data.csv to Parquet format" << std::endl;
    std::cout << "  --strategy NAME    Strategy name (EMA_Crossover, Supertrend)" << std::endl;
    std::cout << "  --params P1,P2,... Strategy parameters (comma-separated)" << std::endl;
    std::cout << "  --dte N            DTE filter (1-5, or -1 for all)" << std::endl;
    std::cout << "  --optimize         Run parameter optimization" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  ./backtest_engine --convert-csv" << std::endl;
    std::cout << "  ./backtest_engine --strategy EMA_Crossover --params 5,20 --dte 1" << std::endl;
    std::cout << "  ./backtest_engine --strategy Supertrend --optimize" << std::endl;
}

std::vector<StrategyParams> generateEMACombinations()
{
    std::vector<StrategyParams> combinations;

    // EMA Fast: 5, 10, 15
    // EMA Slow: 20, 30, 40, 50
    // DTE: 1, 2, 3, 4, 5

    std::vector<int> fast_periods = {5, 10, 15};
    std::vector<int> slow_periods = {20, 30, 40, 50};
    std::vector<int> dte_values = {1, 2, 3, 4, 5};

    for (int fast : fast_periods)
    {
        for (int slow : slow_periods)
        {
            if (fast >= slow)
                continue;

            for (int dte : dte_values)
            {
                StrategyParams params;
                params.strategy_name = "EMA_Crossover";
                params.params = {static_cast<double>(fast), static_cast<double>(slow)};
                params.dte_filter = dte;
                combinations.push_back(params);
            }
        }
    }

    return combinations;
}

std::vector<StrategyParams> generateSupertrendCombinations()
{
    std::vector<StrategyParams> combinations;

    // Period: 7, 10, 14
    // Multiplier: 1.5, 2.0, 2.5, 3.0
    // DTE: 1, 2, 3, 4, 5

    std::vector<int> periods = {7, 10, 14};
    std::vector<double> multipliers = {1.5, 2.0, 2.5, 3.0};
    std::vector<int> dte_values = {1, 2, 3, 4, 5};

    for (int period : periods)
    {
        for (double mult : multipliers)
        {
            for (int dte : dte_values)
            {
                StrategyParams params;
                params.strategy_name = "Supertrend";
                params.params = {static_cast<double>(period), mult};
                params.dte_filter = dte;
                combinations.push_back(params);
            }
        }
    }

    return combinations;
}

int main(int argc, char *argv[])
{
    std::cout << "==================================" << std::endl;
    std::cout << "   Backtest Engine v1.0" << std::endl;
    std::cout << "   High-Performance Strategy Tester" << std::endl;
    std::cout << "==================================" << std::endl;

    std::string csv_path = "market_data.csv";
    std::string parquet_path = "market_data.parquet";
    std::string output_dir = "output";

    // Create output directory
    std::filesystem::create_directories(output_dir);
    std::filesystem::create_directories(output_dir + "/trades");

    // Parse command line arguments
    bool convert_csv = false;
    bool optimize = false;
    std::string strategy_name;
    std::vector<double> params;
    int dte_filter = -1;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--convert-csv")
        {
            convert_csv = true;
        }
        else if (arg == "--strategy" && i + 1 < argc)
        {
            strategy_name = argv[++i];
        }
        else if (arg == "--params" && i + 1 < argc)
        {
            std::string params_str = argv[++i];
            std::istringstream ss(params_str);
            std::string token;
            while (std::getline(ss, token, ','))
            {
                params.push_back(std::stod(token));
            }
        }
        else if (arg == "--dte" && i + 1 < argc)
        {
            dte_filter = std::stoi(argv[++i]);
        }
        else if (arg == "--optimize")
        {
            optimize = true;
        }
        else if (arg == "--help" || arg == "-h")
        {
            printUsage();
            return 0;
        }
    }

    // Convert CSV to Parquet if needed
    if (convert_csv || !std::filesystem::exists(parquet_path))
    {
        std::cout << "\nStep 1: Converting CSV to Parquet format..." << std::endl;
        if (!DataLoader::convertCSVToParquet(csv_path, parquet_path))
        {
            std::cerr << "Error: Failed to convert CSV to Parquet" << std::endl;
            return 1;
        }
    }

    // Load data
    std::cout << "\nStep 2: Loading market data from Parquet..." << std::endl;
    std::vector<Bar> bars = DataLoader::loadFromParquet(parquet_path);
    if (bars.empty())
    {
        std::cerr << "Error: No data loaded" << std::endl;
        return 1;
    }

    std::cout << "Loaded " << bars.size() << " bars" << std::endl;
    std::cout << "Date range: " << bars.front().date << " to " << bars.back().date << std::endl;

    // Create engine
    BacktestEngine engine(2000000.0); // 20 Lakh INR

    if (optimize)
    {
        // Run optimization
        std::cout << "\nStep 3: Running parameter optimization..." << std::endl;

        std::vector<StrategyParams> combinations;

        if (strategy_name.empty() || strategy_name == "EMA_Crossover")
        {
            std::cout << "Generating EMA Crossover combinations..." << std::endl;
            auto ema_combos = generateEMACombinations();
            combinations.insert(combinations.end(), ema_combos.begin(), ema_combos.end());
        }

        if (strategy_name.empty() || strategy_name == "Supertrend")
        {
            std::cout << "Generating Supertrend combinations..." << std::endl;
            auto st_combos = generateSupertrendCombinations();
            combinations.insert(combinations.end(), st_combos.begin(), st_combos.end());
        }

        std::cout << "Total combinations to test: " << combinations.size() << std::endl;

        std::vector<PerformanceMetrics> results = engine.runOptimization(
            bars,
            strategy_name.empty() ? "ALL" : strategy_name,
            combinations,
            output_dir + "/trades");

        std::cout << "\nOptimization complete! Results saved to " << output_dir << std::endl;
        std::cout << "Run Python analysis: python analytics/analyze_results.py" << std::endl;
    }
    else if (!strategy_name.empty() && !params.empty())
    {
        // Run single backtest
        std::cout << "\nStep 3: Running single backtest..." << std::endl;

        StrategyParams strat_params;
        strat_params.strategy_name = strategy_name;
        strat_params.params = params;
        strat_params.dte_filter = dte_filter;

        TradeLogger logger;

        auto strategy = engine.createStrategy(strategy_name);
        if (!strategy)
        {
            std::cerr << "Error: Unknown strategy: " << strategy_name << std::endl;
            return 1;
        }

        PerformanceMetrics metrics = engine.runBacktest(bars, strategy.get(), strat_params, logger);

        // Print results
        std::cout << "\n=== Backtest Results ===" << std::endl;
        std::cout << "Strategy: " << strategy_name << std::endl;
        std::cout << "Parameters: " << strat_params.to_string() << std::endl;
        std::cout << "Total Trades: " << metrics.total_trades << std::endl;
        std::cout << "Winning Trades: " << metrics.winning_trades << std::endl;
        std::cout << "Losing Trades: " << metrics.losing_trades << std::endl;
        std::cout << "Win Rate: " << metrics.win_rate << "%" << std::endl;
        std::cout << "Total PnL: ₹" << metrics.total_pnl << std::endl;
        std::cout << "Total Return: " << metrics.total_return_pct << "%" << std::endl;
        std::cout << "Profit Factor: " << metrics.profit_factor << std::endl;
        std::cout << "Expectancy: ₹" << metrics.expectancy << std::endl;
        std::cout << "Max Drawdown: " << metrics.max_drawdown << "%" << std::endl;

        // Save trades
        std::string trades_file = output_dir + "/trades/single_backtest.parquet";
        logger.saveToParquet(trades_file);
        std::cout << "\nTrades saved to: " << trades_file << std::endl;
    }
    else
    {
        printUsage();
        return 1;
    }

    return 0;
}
