#ifndef KELTNER_H
#define KELTNER_H

#include "indicator_base.h"
#include "ema.h"
#include "atr.h"

namespace backtest
{
    namespace indicators
    {

        class KeltnerChannel : public IndicatorBase
        {
        public:
            KeltnerChannel(int ema_period, int atr_period, double multiplier);

            void calculate(const std::vector<Bar> &bars) override;
            double getValue(size_t index) const override; // Returns middle line
            bool isReady(size_t index) const override;

            // Get channel bands
            double getUpperBand(size_t index) const;
            double getLowerBand(size_t index) const;
            double getMiddleLine(size_t index) const;

        private:
            int ema_period_;
            int atr_period_;
            double multiplier_;
            EMA ema_;
            ATR atr_;
            std::vector<double> upper_band_;
            std::vector<double> lower_band_;
        };

    } // namespace indicators
} // namespace backtest

#endif // KELTNER_H
