#ifndef INDICATOR_BASE_H
#define INDICATOR_BASE_H

#include <vector>
#include "../data_structures.h"

namespace backtest
{
    namespace indicators
    {

        class IndicatorBase
        {
        public:
            virtual ~IndicatorBase() = default;

            // Calculate indicator values for all bars
            virtual void calculate(const std::vector<Bar> &bars) = 0;

            // Get indicator value at specific index
            virtual double getValue(size_t index) const = 0;

            // Check if indicator is ready at specific index
            virtual bool isReady(size_t index) const = 0;

        protected:
            std::vector<double> values_;
        };

    } // namespace indicators
} // namespace backtest

#endif // INDICATOR_BASE_H
