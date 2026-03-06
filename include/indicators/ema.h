#ifndef EMA_H
#define EMA_H

#include "indicator_base.h"

namespace backtest
{
    namespace indicators
    {

        class EMA : public IndicatorBase
        {
        public:
            explicit EMA(int period);

            void calculate(const std::vector<Bar> &bars) override;
            double getValue(size_t index) const override;
            bool isReady(size_t index) const override;

        private:
            int period_;
            double multiplier_;
        };

    } // namespace indicators
} // namespace backtest

#endif // EMA_H
