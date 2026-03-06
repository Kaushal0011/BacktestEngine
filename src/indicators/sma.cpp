#include "indicators/sma.h"

namespace backtest
{
    namespace indicators
    {

        SMA::SMA(int period) : period_(period) {}

        void SMA::calculate(const std::vector<Bar> &bars)
        {
            values_.clear();
            values_.resize(bars.size(), 0.0);

            if (bars.empty() || bars.size() < static_cast<size_t>(period_))
            {
                return;
            }

            // Calculate first SMA
            double sum = 0.0;
            for (int i = 0; i < period_; ++i)
            {
                sum += bars[i].close;
            }
            values_[period_ - 1] = sum / period_;

            // Calculate remaining SMAs using rolling window
            for (size_t i = period_; i < bars.size(); ++i)
            {
                sum = sum - bars[i - period_].close + bars[i].close;
                values_[i] = sum / period_;
            }
        }

        double SMA::getValue(size_t index) const
        {
            if (index >= values_.size())
            {
                return 0.0;
            }
            return values_[index];
        }

        bool SMA::isReady(size_t index) const
        {
            return index >= static_cast<size_t>(period_ - 1) && !values_.empty();
        }

    } // namespace indicators
} // namespace backtest
