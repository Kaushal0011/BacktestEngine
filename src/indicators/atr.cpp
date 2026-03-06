#include "indicators/atr.h"
#include <algorithm>
#include <cmath>

namespace backtest
{
    namespace indicators
    {

        ATR::ATR(int period) : period_(period) {}

        void ATR::calculate(const std::vector<Bar> &bars)
        {
            values_.clear();
            true_range_.clear();

            if (bars.empty())
            {
                return;
            }

            values_.resize(bars.size(), 0.0);
            true_range_.resize(bars.size(), 0.0);

            // Calculate True Range
            true_range_[0] = bars[0].high - bars[0].low;

            for (size_t i = 1; i < bars.size(); ++i)
            {
                double tr1 = bars[i].high - bars[i].low;
                double tr2 = std::abs(bars[i].high - bars[i - 1].close);
                double tr3 = std::abs(bars[i].low - bars[i - 1].close);
                true_range_[i] = std::max({tr1, tr2, tr3});
            }

            if (bars.size() < static_cast<size_t>(period_))
            {
                return;
            }

            // Calculate initial ATR (SMA of TR)
            double sum = 0.0;
            for (int i = 0; i < period_; ++i)
            {
                sum += true_range_[i];
            }
            values_[period_ - 1] = sum / period_;

            // Calculate ATR using smoothing
            for (size_t i = period_; i < bars.size(); ++i)
            {
                values_[i] = (values_[i - 1] * (period_ - 1) + true_range_[i]) / period_;
            }
        }

        double ATR::getValue(size_t index) const
        {
            if (index >= values_.size())
            {
                return 0.0;
            }
            return values_[index];
        }

        bool ATR::isReady(size_t index) const
        {
            return index >= static_cast<size_t>(period_ - 1) && !values_.empty();
        }

    } // namespace indicators
} // namespace backtest
