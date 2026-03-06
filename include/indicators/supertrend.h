#ifndef SUPERTREND_H
#define SUPERTREND_H

#include "indicator_base.h"
#include "atr.h"

namespace backtest
{
    namespace indicators
    {

        class Supertrend : public IndicatorBase
        {
        public:
            Supertrend(int period, double multiplier);

            void calculate(const std::vector<Bar> &bars) override;
            double getValue(size_t index) const override;
            bool isReady(size_t index) const override;

            // Get trend direction: 1 for uptrend, -1 for downtrend
            int getTrend(size_t index) const;

        private:
            int period_;
            double multiplier_;
            ATR atr_;
            std::vector<int> trend_;
        };

    } // namespace indicators
} // namespace backtest

#endif // SUPERTREND_H
