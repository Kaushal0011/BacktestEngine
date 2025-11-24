#ifndef BACKTEST_RESULT_H
#define BACKTEST_RESULT_H

#include <vector>
#include <string>
#include "Trade.hpp"

class BacktestResult
{
public:
    int ema_period;
    int atr_period;
    double multiplier;

    std::vector<Trade> trades;

    double total_profit_loss;
    double total_profit_loss_percentage;
    int winning_trades;
    int losing_trades;
    double win_rate;
    double avg_profit_per_trade;
    double max_drawdown;
    double sharpe_ratio;

    BacktestResult() : ema_period(0), atr_period(0), multiplier(0),
                       total_profit_loss(0), total_profit_loss_percentage(0),
                       winning_trades(0), losing_trades(0), win_rate(0),
                       avg_profit_per_trade(0), max_drawdown(0), sharpe_ratio(0) {}

    BacktestResult(int ema, int atr, double mult)
        : ema_period(ema), atr_period(atr), multiplier(mult),
          total_profit_loss(0), total_profit_loss_percentage(0),
          winning_trades(0), losing_trades(0), win_rate(0),
          avg_profit_per_trade(0), max_drawdown(0), sharpe_ratio(0) {}

    void calculateMetrics()
    {
        total_profit_loss = 0;
        winning_trades = 0;
        losing_trades = 0;

        for (const auto &trade : trades)
        {
            double pnl = trade.getProfitLoss();
            total_profit_loss += pnl;

            if (pnl > 0)
            {
                winning_trades++;
            }
            else if (pnl < 0)
            {
                losing_trades++;
            }
        }

        int total_trades = winning_trades + losing_trades;
        win_rate = total_trades > 0 ? (double)winning_trades / total_trades * 100.0 : 0;
        avg_profit_per_trade = total_trades > 0 ? total_profit_loss / total_trades : 0;

        double cumulative_pnl = 0;
        double peak = 0;
        max_drawdown = 0;

        for (const auto &trade : trades)
        {
            cumulative_pnl += trade.getProfitLoss();
            if (cumulative_pnl > peak)
            {
                peak = cumulative_pnl;
            }
            double drawdown = peak - cumulative_pnl;
            if (drawdown > max_drawdown)
            {
                max_drawdown = drawdown;
            }
        }
    }

    std::string getSummary() const
    {
        std::string summary = "EMA: " + std::to_string(ema_period) +
                              ", ATR: " + std::to_string(atr_period) +
                              ", Mult: " + std::to_string(multiplier) + "\n";
        summary += "Total Trades: " + std::to_string(trades.size()) + "\n";
        summary += "Winning Trades: " + std::to_string(winning_trades) + "\n";
        summary += "Losing Trades: " + std::to_string(losing_trades) + "\n";
        summary += "Win Rate: " + std::to_string(win_rate) + "%\n";
        summary += "Total P&L: Rs " + std::to_string(total_profit_loss) + "\n";
        summary += "Avg Profit/Trade: Rs " + std::to_string(avg_profit_per_trade) + "\n";
        summary += "Max Drawdown: Rs " + std::to_string(max_drawdown) + "\n";
        return summary;
    }
};

#endif // BACKTEST_RESULT_H
