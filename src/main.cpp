#include <iostream>
#include <vector>
#include <string>
#include "../include/MarketData.hpp"
#include "../include/CSVParser.hpp"
#include "../include/KeltnerIndicator.hpp"
#include "../include/TradingStrategy.hpp"
#include "../include/BacktestEngine.hpp"
#include "../include/TradeLogger.hpp"

int main(int argc, char* argv[]) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "    KELTNER CHANNEL BACKTESTING ENGINE" << std::endl;
    std::cout << "    High-Performance C++ Trading Strategy Analyzer" << std::endl;
    std::cout << std::string(60, '=') << "\n" << std::endl;
    
    std::string data_file = "data/market_data.csv";
    if (argc > 1) {
        data_file = argv[1];
    }
    
    MarketData market_data;
    CSVParser parser;
    
    std::cout << "Loading market data from: " << data_file << std::endl;
    if (!parser.loadFromFile(data_file, market_data)) {
        std::cerr << "Failed to load market data. Exiting." << std::endl;
        return 1;
    }
    
    if (market_data.size() == 0) {
        std::cerr << "No data loaded. Please check your CSV file." << std::endl;
        return 1;
    }
    
    const double CAPITAL = 2000000.0;
    
    std::vector<ParameterSet> parameter_sets;
    
    std::vector<int> ema_periods = {10, 20, 30, 50};
    std::vector<int> atr_periods = {10, 14, 20};
    std::vector<double> multipliers = {1.5, 2.0, 2.5, 3.0};
    
    std::cout << "\nGenerating parameter combinations..." << std::endl;
    std::cout << "EMA Periods: ";
    for (int ema : ema_periods) std::cout << ema << " ";
    std::cout << "\nATR Periods: ";
    for (int atr : atr_periods) std::cout << atr << " ";
    std::cout << "\nMultipliers: ";
    for (double mult : multipliers) std::cout << mult << " ";
    std::cout << "\n" << std::endl;
    
    for (int ema : ema_periods) {
        for (int atr : atr_periods) {
            for (double mult : multipliers) {
                parameter_sets.emplace_back(ema, atr, mult);
            }
        }
    }
    
    std::cout << "Total combinations to test: " << parameter_sets.size() << "\n" << std::endl;
    
    BacktestEngine engine(CAPITAL);
    engine.loadMarketData(market_data);
    
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;
    
    std::cout << "System CPU threads available: " << num_threads << "\n" << std::endl;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    engine.runParallelBacktests(parameter_sets, num_threads);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "Backtesting completed in " << duration.count() / 1000.0 << " seconds" << std::endl;
    std::cout << std::string(60, '=') << "\n" << std::endl;
    
    const auto& results = engine.getResults();
    
    TradeLogger logger("output");
    
    std::cout << "Exporting trade logs..." << std::endl;
    if (logger.exportTradeLogs(results)) {
        std::cout << "Trade logs exported successfully!" << std::endl;
    } else {
        std::cerr << "Error exporting trade logs." << std::endl;
    }
    
    std::cout << "\nExporting summary report..." << std::endl;
    if (logger.exportSummaryReport(results)) {
        std::cout << "Summary report exported successfully!" << std::endl;
    } else {
        std::cerr << "Error exporting summary report." << std::endl;
    }
    
    engine.printSummary();
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "All results saved in 'output/' directory" << std::endl;
    std::cout << std::string(60, '=') << "\n" << std::endl;
    
    double best_pnl = -1e9;
    const BacktestResult* best_result = nullptr;
    
    for (const auto& result : results) {
        if (result.total_profit_loss > best_pnl) {
            best_pnl = result.total_profit_loss;
            best_result = &result;
        }
    }
    
    if (best_result != nullptr) {
        std::cout << "\nBEST PERFORMING PARAMETERS:" << std::endl;
        std::cout << std::string(60, '-') << std::endl;
        std::cout << "EMA Period: " << best_result->ema_period << std::endl;
        std::cout << "ATR Period: " << best_result->atr_period << std::endl;
        std::cout << "Multiplier: " << best_result->multiplier << std::endl;
        std::cout << "Total P&L: Rs " << best_result->total_profit_loss << std::endl;
        std::cout << "Total Trades: " << best_result->trades.size() << std::endl;
        std::cout << "Win Rate: " << best_result->win_rate << "%" << std::endl;
        std::cout << std::string(60, '-') << "\n" << std::endl;
    }
    
    return 0;
}
