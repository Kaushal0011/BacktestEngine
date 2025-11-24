#ifndef KELTNER_INDICATOR_H
#define KELTNER_INDICATOR_H

#include <vector>
#include <cmath>
#include "MarketData.hpp"

struct KeltnerBands
{
    double middle; // EMA
    double upper;
    double lower;
    double atr;

    KeltnerBands() : middle(0), upper(0), lower(0), atr(0) {}
};

class KeltnerIndicator
{
private:
    int ema_period;
    int atr_period;
    double multiplier;

    std::vector<double> calculateEMAIterative(const std::vector<double> &prices, int period) const
    {
        std::vector<double> ema_values;
        if (prices.empty())
        {
            return ema_values;
        }

        ema_values.resize(prices.size(), 0.0);

        if (static_cast<int>(prices.size()) < period)
        {
            return ema_values;
        }

        double sum = 0;
        for (int i = 0; i < period; i++)
        {
            sum += prices[i];
        }
        ema_values[period - 1] = sum / period;

        double alpha = 2.0 / (period + 1);
        for (size_t i = period; i < prices.size(); i++)
        {
            ema_values[i] = (prices[i] * alpha) + (ema_values[i - 1] * (1 - alpha));
        }

        return ema_values;
    }

    double calculateTrueRange(const OHLCV &current, const OHLCV &previous) const
    {
        double tr1 = current.high - current.low;
        double tr2 = std::abs(current.high - previous.close);
        double tr3 = std::abs(current.low - previous.close);
        return std::max({tr1, tr2, tr3});
    }

    std::vector<double> calculateATRIterative(const std::vector<OHLCV> &data, int period) const
    {
        std::vector<double> atr_values;
        if (data.empty())
        {
            return atr_values;
        }

        atr_values.resize(data.size(), 0.0);

        if (static_cast<int>(data.size()) <= period)
        {
            return atr_values;
        }

        double sum = 0;
        for (int i = 1; i <= period; i++)
        {
            sum += calculateTrueRange(data[i], data[i - 1]);
        }
        atr_values[period] = sum / period;

        double alpha = 1.0 / period;
        for (size_t i = period + 1; i < data.size(); i++)
        {
            double tr = calculateTrueRange(data[i], data[i - 1]);
            atr_values[i] = (tr * alpha) + (atr_values[i - 1] * (1 - alpha));
        }

        return atr_values;
    }

public:
    KeltnerIndicator(int ema_p, int atr_p, double mult)
        : ema_period(ema_p), atr_period(atr_p), multiplier(mult) {}

    std::vector<KeltnerBands> calculate(const MarketData &market_data) const
    {
        std::vector<KeltnerBands> bands;
        const auto &data = market_data.getData();

        if (data.empty())
        {
            return bands;
        }

        std::vector<double> close_prices;
        for (const auto &bar : data)
        {
            close_prices.push_back(bar.close);
        }

        std::vector<double> ema_values = calculateEMAIterative(close_prices, ema_period);
        std::vector<double> atr_values = calculateATRIterative(data, atr_period);

        for (size_t i = 0; i < data.size(); i++)
        {
            KeltnerBands band;

            if (i >= static_cast<size_t>(std::max(ema_period, atr_period)))
            {
                band.middle = ema_values[i];
                band.atr = atr_values[i];
                band.upper = band.middle + (multiplier * band.atr);
                band.lower = band.middle - (multiplier * band.atr);
            }

            bands.push_back(band);
        }

        return bands;
    }

    int getEmaPeriod() const { return ema_period; }
    int getAtrPeriod() const { return atr_period; }
    double getMultiplier() const { return multiplier; }
};

#endif // KELTNER_INDICATOR_H
