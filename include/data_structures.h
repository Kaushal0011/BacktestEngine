#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <string>
#include <vector>
#include <ctime>

namespace backtest
{

    // Market data bar structure
    struct Bar
    {
        std::string timestamp;
        double open;
        double high;
        double low;
        double close;
        std::string date;
        std::string weekly_expiry_date;
        std::string dt;
        int dte; // Days to expiry

        Bar() : open(0), high(0), low(0), close(0), dte(0) {}
    };

    // Trade structure
    struct Trade
    {
        std::string entry_time;
        std::string exit_time;
        std::string entry_date;
        std::string exit_date;
        double entry_price;
        double exit_price;
        double quantity;
        double pnl;
        double pnl_percentage;
        std::string direction; // LONG or SHORT
        int dte;
        std::string strategy_name;
        std::string parameters;

        Trade() : entry_price(0), exit_price(0), quantity(0),
                  pnl(0), pnl_percentage(0), dte(0) {}
    };

    // Strategy parameters structure
    struct StrategyParams
    {
        std::string strategy_name;
        std::vector<double> params;
        int dte_filter; // -1 for all, or specific DTE (1-5)

        std::string to_string() const
        {
            std::string result = strategy_name + "_";
            for (size_t i = 0; i < params.size(); ++i)
            {
                result += std::to_string(static_cast<int>(params[i]));
                if (i < params.size() - 1)
                    result += "_";
            }
            result += "_DTE" + std::to_string(dte_filter);
            return result;
        }
    };

    // Performance metrics structure
    struct PerformanceMetrics
    {
        int total_trades;
        int winning_trades;
        int losing_trades;
        double total_pnl;
        double total_return_pct;
        double win_rate;
        double avg_win;
        double avg_loss;
        double max_win;
        double max_loss;
        double profit_factor;
        double expectancy;
        double max_drawdown;
        int consecutive_wins;
        int consecutive_losses;
        int dte;
        std::string strategy_params;

        PerformanceMetrics() : total_trades(0), winning_trades(0), losing_trades(0),
                               total_pnl(0), total_return_pct(0), win_rate(0),
                               avg_win(0), avg_loss(0), max_win(0), max_loss(0),
                               profit_factor(0), expectancy(0), max_drawdown(0),
                               consecutive_wins(0), consecutive_losses(0), dte(0) {}
    };

} // namespace backtest

#endif // DATA_STRUCTURES_H
