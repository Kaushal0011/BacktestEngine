#include "indicators/ema.h"
#include <cmath>

namespace backtest
{
    namespace indicators
    {

        EMA::EMA(int period) : period_(period)
        {
            multiplier_ = 2.0 / (period_ + 1.0);
        }

        void EMA::calculate(const std::vector<Bar> &bars)
        {
            values_.clear();
            values_.resize(bars.size(), 0.0);

            if (bars.empty() || bars.size() < static_cast<size_t>(period_))
            {
                return;
            }

            // Calculate initial SMA for first EMA value
            double sum = 0.0;
            for (int i = 0; i < period_; ++i)
            {
                sum += bars[i].close;
            }
            values_[period_ - 1] = sum / period_;

            // Calculate EMA for remaining values
            for (size_t i = period_; i < bars.size(); ++i)
            {
                values_[i] = (bars[i].close - values_[i - 1]) * multiplier_ + values_[i - 1];
            }
        }

        double EMA::getValue(size_t index) const
        {
            if (index >= values_.size())
            {
                return 0.0;
            }
            return values_[index];
        }

        bool EMA::isReady(size_t index) const
        {
            return index >= static_cast<size_t>(period_ - 1) && !values_.empty();
        }

    } // namespace indicators
} // namespace backtest
