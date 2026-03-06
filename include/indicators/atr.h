#ifndef ATR_H
#define ATR_H

#include "indicator_base.h"

namespace backtest
{
    namespace indicators
    {

        class ATR : public IndicatorBase
        {
        public:
            explicit ATR(int period);

            void calculate(const std::vector<Bar> &bars) override;
            double getValue(size_t index) const override;
            bool isReady(size_t index) const override;

        private:
            int period_;
            std::vector<double> true_range_;
        };

    } // namespace indicators
} // namespace backtest

#endif // ATR_H
