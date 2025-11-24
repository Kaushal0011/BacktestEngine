#ifndef TRADING_STRATEGY_H
#define TRADING_STRATEGY_H

#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "MarketData.hpp"
#include "KeltnerIndicator.hpp"
#include "Trade.hpp"

class TradingStrategy
{
private:
    double capital;

    bool isSameDay(const std::string &timestamp1, const std::string &timestamp2) const
    {
        if (timestamp1.length() < 10 || timestamp2.length() < 10)
        {
            return false;
        }
        return timestamp1.substr(0, 10) == timestamp2.substr(0, 10);
    }

    bool isEndOfDay(const std::string &current_timestamp, const std::string &next_timestamp) const
    {
        return !isSameDay(current_timestamp, next_timestamp);
    }

    std::string extractTime(const std::string &timestamp) const
    {
        size_t space_pos = timestamp.find(' ');
        if (space_pos != std::string::npos && timestamp.length() > space_pos + 1)
        {
            return timestamp.substr(space_pos + 1);
        }
        return "00:00:00";
    }

public:
    TradingStrategy(double initial_capital) : capital(initial_capital) {}

    std::vector<Trade> executeBacktest(const MarketData &market_data,
                                       const KeltnerIndicator &indicator)
    {
        std::vector<Trade> trades;
        const auto &data = market_data.getData();

        if (data.size() < 2)
        {
            return trades;
        }

        std::vector<KeltnerBands> bands = indicator.calculate(market_data);

        Trade *active_trade = nullptr;
        double running_pnl = 0;
        double peak_pnl = 0;

        for (size_t i = std::max(indicator.getEmaPeriod(), indicator.getAtrPeriod());
             i < data.size(); i++)
        {

            const OHLCV &current_bar = data[i];
            const KeltnerBands &current_bands = bands[i];

            if (current_bands.upper == 0 || current_bands.lower == 0)
            {
                continue;
            }

            if (active_trade == nullptr)
            {
                if (current_bar.close > current_bands.upper)
                {
                    Trade new_trade;
                    new_trade.entry_timestamp = current_bar.timestamp;
                    new_trade.entry_price = current_bar.close;
                    new_trade.entry_keltner_upper = current_bands.upper;
                    new_trade.entry_keltner_lower = current_bands.lower;
                    new_trade.ema_period = indicator.getEmaPeriod();
                    new_trade.atr_period = indicator.getAtrPeriod();
                    new_trade.multiplier = indicator.getMultiplier();
                    new_trade.investment = capital;
                    new_trade.quantity = static_cast<int>(capital / current_bar.close);
                    new_trade.direction = TradeDirection::LONG;
                    new_trade.dte = 0;

                    trades.push_back(new_trade);
                    active_trade = &trades.back();
                    running_pnl = 0;
                    peak_pnl = 0;
                }
                else if (current_bar.close < current_bands.lower)
                {
                    Trade new_trade;
                    new_trade.entry_timestamp = current_bar.timestamp;
                    new_trade.entry_price = current_bar.close;
                    new_trade.entry_keltner_upper = current_bands.upper;
                    new_trade.entry_keltner_lower = current_bands.lower;
                    new_trade.ema_period = indicator.getEmaPeriod();
                    new_trade.atr_period = indicator.getAtrPeriod();
                    new_trade.multiplier = indicator.getMultiplier();
                    new_trade.investment = capital;
                    new_trade.quantity = static_cast<int>(capital / current_bar.close);
                    new_trade.direction = TradeDirection::SHORT;
                    new_trade.dte = 0;

                    trades.push_back(new_trade);
                    active_trade = &trades.back();
                    running_pnl = 0;
                    peak_pnl = 0;
                }
            }
            else
            {
                if (active_trade->direction == TradeDirection::LONG)
                {
                    running_pnl = (current_bar.close - active_trade->entry_price) * active_trade->quantity;
                }
                else
                {
                    running_pnl = (active_trade->entry_price - current_bar.close) * active_trade->quantity;
                }

                if (running_pnl > peak_pnl)
                {
                    peak_pnl = running_pnl;
                    active_trade->max_profit_during_trade = peak_pnl;
                }

                double drawdown_from_peak = peak_pnl - running_pnl;
                if (drawdown_from_peak > active_trade->max_drawdown_during_trade)
                {
                    active_trade->max_drawdown_during_trade = drawdown_from_peak;
                }

                bool should_exit = false;

                if (active_trade->direction == TradeDirection::LONG)
                {
                    if (current_bar.close < current_bands.middle)
                    {
                        should_exit = true;
                    }
                }
                else
                {
                    if (current_bar.close > current_bands.middle)
                    {
                        should_exit = true;
                    }
                }

                bool is_last_bar = (i == data.size() - 1);
                bool is_eod = false;
                if (!is_last_bar)
                {
                    is_eod = isEndOfDay(current_bar.timestamp, data[i + 1].timestamp);
                }
                else
                {
                    is_eod = true;
                }

                if (should_exit || is_eod)
                {
                    active_trade->exit_timestamp = current_bar.timestamp;
                    active_trade->exit_price = current_bar.close;
                    active_trade->exit_keltner_upper = current_bands.upper;
                    active_trade->exit_keltner_lower = current_bands.lower;

                    active_trade = nullptr;
                }
            }
        }

        if (active_trade != nullptr)
        {
            const OHLCV &last_bar = data.back();
            const KeltnerBands &last_bands = bands.back();

            active_trade->exit_timestamp = last_bar.timestamp;
            active_trade->exit_price = last_bar.close;
            active_trade->exit_keltner_upper = last_bands.upper;
            active_trade->exit_keltner_lower = last_bands.lower;
        }

        return trades;
    }
};

#endif // TRADING_STRATEGY_H
