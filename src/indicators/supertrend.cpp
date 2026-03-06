#include "indicators/supertrend.h"

namespace backtest
{
    namespace indicators
    {

        Supertrend::Supertrend(int period, double multiplier)
            : period_(period), multiplier_(multiplier), atr_(period) {}

        void Supertrend::calculate(const std::vector<Bar> &bars)
        {
            values_.clear();
            trend_.clear();

            if (bars.empty())
            {
                return;
            }

            // Calculate ATR first
            atr_.calculate(bars);

            values_.resize(bars.size(), 0.0);
            trend_.resize(bars.size(), 0);

            std::vector<double> basic_upper(bars.size(), 0.0);
            std::vector<double> basic_lower(bars.size(), 0.0);
            std::vector<double> final_upper(bars.size(), 0.0);
            std::vector<double> final_lower(bars.size(), 0.0);

            for (size_t i = 0; i < bars.size(); ++i)
            {
                if (!atr_.isReady(i))
                {
                    continue;
                }

                double hl_avg = (bars[i].high + bars[i].low) / 2.0;
                double atr_value = atr_.getValue(i);

                basic_upper[i] = hl_avg + multiplier_ * atr_value;
                basic_lower[i] = hl_avg - multiplier_ * atr_value;

                // Final bands with filtering
                if (i == 0 || i < static_cast<size_t>(period_))
                {
                    final_upper[i] = basic_upper[i];
                    final_lower[i] = basic_lower[i];
                }
                else
                {
                    final_upper[i] = (basic_upper[i] < final_upper[i - 1] ||
                                      bars[i - 1].close > final_upper[i - 1])
                                         ? basic_upper[i]
                                         : final_upper[i - 1];

                    final_lower[i] = (basic_lower[i] > final_lower[i - 1] ||
                                      bars[i - 1].close < final_lower[i - 1])
                                         ? basic_lower[i]
                                         : final_lower[i - 1];
                }

                // Determine trend
                if (i == 0 || i < static_cast<size_t>(period_))
                {
                    trend_[i] = 1;
                    values_[i] = final_lower[i];
                }
                else
                {
                    if (trend_[i - 1] == 1)
                    {
                        trend_[i] = (bars[i].close <= final_lower[i]) ? -1 : 1;
                    }
                    else
                    {
                        trend_[i] = (bars[i].close >= final_upper[i]) ? 1 : -1;
                    }

                    values_[i] = (trend_[i] == 1) ? final_lower[i] : final_upper[i];
                }
            }
        }

        double Supertrend::getValue(size_t index) const
        {
            if (index >= values_.size())
            {
                return 0.0;
            }
            return values_[index];
        }

        bool Supertrend::isReady(size_t index) const
        {
            return atr_.isReady(index) && index < trend_.size();
        }

        int Supertrend::getTrend(size_t index) const
        {
            if (index >= trend_.size())
            {
                return 0;
            }
            return trend_[index];
        }

    } // namespace indicators
} // namespace backtest
