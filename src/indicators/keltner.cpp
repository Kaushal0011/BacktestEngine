#include "indicators/keltner.h"

namespace backtest
{
    namespace indicators
    {

        KeltnerChannel::KeltnerChannel(int ema_period, int atr_period, double multiplier)
            : ema_period_(ema_period), atr_period_(atr_period),
              multiplier_(multiplier), ema_(ema_period), atr_(atr_period) {}

        void KeltnerChannel::calculate(const std::vector<Bar> &bars)
        {
            values_.clear();
            upper_band_.clear();
            lower_band_.clear();

            if (bars.empty())
            {
                return;
            }

            // Calculate EMA and ATR
            ema_.calculate(bars);
            atr_.calculate(bars);

            values_.resize(bars.size(), 0.0);
            upper_band_.resize(bars.size(), 0.0);
            lower_band_.resize(bars.size(), 0.0);

            for (size_t i = 0; i < bars.size(); ++i)
            {
                if (!ema_.isReady(i) || !atr_.isReady(i))
                {
                    continue;
                }

                double middle = ema_.getValue(i);
                double atr_value = atr_.getValue(i);

                values_[i] = middle;
                upper_band_[i] = middle + multiplier_ * atr_value;
                lower_band_[i] = middle - multiplier_ * atr_value;
            }
        }

        double KeltnerChannel::getValue(size_t index) const
        {
            return getMiddleLine(index);
        }

        bool KeltnerChannel::isReady(size_t index) const
        {
            return ema_.isReady(index) && atr_.isReady(index);
        }

        double KeltnerChannel::getUpperBand(size_t index) const
        {
            if (index >= upper_band_.size())
            {
                return 0.0;
            }
            return upper_band_[index];
        }

        double KeltnerChannel::getLowerBand(size_t index) const
        {
            if (index >= lower_band_.size())
            {
                return 0.0;
            }
            return lower_band_[index];
        }

        double KeltnerChannel::getMiddleLine(size_t index) const
        {
            if (index >= values_.size())
            {
                return 0.0;
            }
            return values_[index];
        }

    } // namespace indicators
} // namespace backtest
