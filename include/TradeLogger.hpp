#ifndef TRADE_LOGGER_H
#define TRADE_LOGGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "BacktestResult.hpp"

class TradeLogger
{
private:
    std::string output_directory;

    std::string escapeCSV(const std::string &str) const
    {
        if (str.find(',') != std::string::npos || str.find('"') != std::string::npos)
        {
            std::string escaped = "\"";
            for (char c : str)
            {
                if (c == '"')
                    escaped += "\"\"";
                else
                    escaped += c;
            }
            escaped += "\"";
            return escaped;
        }
        return str;
    }

public:
    TradeLogger(const std::string &output_dir = "output")
        : output_directory(output_dir) {}

    bool exportTradeLogs(const std::vector<BacktestResult> &results)
    {
        for (const auto &result : results)
        {
            std::stringstream filename;
            filename << output_directory << "/tradelog_EMA" << result.ema_period
                     << "_ATR" << result.atr_period
                     << "_MULT" << std::fixed << std::setprecision(1) << result.multiplier
                     << ".csv";

            if (!exportSingleBacktestLog(result, filename.str()))
            {
                return false;
            }
        }
        return true;
    }

    bool exportSingleBacktestLog(const BacktestResult &result, const std::string &filename)
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not create file " << filename << std::endl;
            return false;
        }

        file << "Entry Timestamp,Exit Timestamp,DTE,Entry Price,Exit Price,"
             << "EMA Period,ATR Period,Multiplier,"
             << "Entry Keltner Upper,Entry Keltner Lower,"
             << "Exit Keltner Upper,Exit Keltner Lower,"
             << "Investment,Quantity,Direction,"
             << "Profit/Loss (Rs),Profit/Loss (%),Max Profit During Trade (Rs),"
             << "Max Drawdown During Trade (Rs),Calmar Ratio\n";

        for (const auto &trade : result.trades)
        {
            file << escapeCSV(trade.entry_timestamp) << ","
                 << escapeCSV(trade.exit_timestamp) << ","
                 << trade.dte << ","
                 << std::fixed << std::setprecision(2) << trade.entry_price << ","
                 << trade.exit_price << ","
                 << trade.ema_period << ","
                 << trade.atr_period << ","
                 << std::setprecision(2) << trade.multiplier << ","
                 << trade.entry_keltner_upper << ","
                 << trade.entry_keltner_lower << ","
                 << trade.exit_keltner_upper << ","
                 << trade.exit_keltner_lower << ","
                 << trade.investment << ","
                 << trade.quantity << ","
                 << (trade.direction == TradeDirection::LONG ? "LONG" : "SHORT") << ","
                 << trade.getProfitLoss() << ","
                 << std::setprecision(4) << trade.getProfitLossPercentage() << ","
                 << std::setprecision(2) << trade.max_profit_during_trade << ","
                 << trade.max_drawdown_during_trade << ","
                 << std::setprecision(4) << trade.getCalmarRatio() << "\n";
        }

        file.close();
        std::cout << "Exported " << result.trades.size() << " trades to " << filename << std::endl;
        return true;
    }

    bool exportSummaryReport(const std::vector<BacktestResult> &results,
                             const std::string &filename = "output/summary_report.csv")
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not create file " << filename << std::endl;
            return false;
        }

        file << "EMA Period,ATR Period,Multiplier,Total Trades,Winning Trades,"
             << "Losing Trades,Win Rate (%),Total P&L (Rs),Avg P&L per Trade (Rs),"
             << "Max Drawdown (Rs)\n";

        for (const auto &result : results)
        {
            file << result.ema_period << ","
                 << result.atr_period << ","
                 << std::fixed << std::setprecision(2) << result.multiplier << ","
                 << result.trades.size() << ","
                 << result.winning_trades << ","
                 << result.losing_trades << ","
                 << std::setprecision(2) << result.win_rate << ","
                 << result.total_profit_loss << ","
                 << result.avg_profit_per_trade << ","
                 << result.max_drawdown << "\n";
        }

        file.close();
        std::cout << "Exported summary report to " << filename << std::endl;
        return true;
    }
};

#endif // TRADE_LOGGER_H
