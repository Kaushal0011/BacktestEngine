#ifndef STRATEGY_BASE_H
#define STRATEGY_BASE_H

#include "../data_structures.h"
#include <vector>
#include <memory>

namespace backtest
{
    namespace strategy
    {

        enum class Signal
        {
            NONE,
            LONG,
            SHORT,
            EXIT_LONG,
            EXIT_SHORT
        };

        class StrategyBase
        {
        public:
            virtual ~StrategyBase() = default;

            // Initialize strategy with parameters
            virtual void initialize(const StrategyParams &params) = 0;

            // Calculate indicators for all bars
            virtual void calculateIndicators(const std::vector<Bar> &bars) = 0;

            // Generate signal at specific bar index
            virtual Signal generateSignal(size_t index, const std::vector<Bar> &bars) = 0;

            // Check if strategy is ready at specific index
            virtual bool isReady(size_t index) const = 0;

            // Get strategy name
            virtual std::string getName() const = 0;

            // Get parameters as string
            virtual std::string getParamsString() const = 0;
        };

    } // namespace strategy
} // namespace backtest

#endif // STRATEGY_BASE_H
