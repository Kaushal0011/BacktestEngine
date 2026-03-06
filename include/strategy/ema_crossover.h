#ifndef EMA_CROSSOVER_H
#define EMA_CROSSOVER_H

#include "strategy_base.h"
#include "../indicators/ema.h"
#include <memory>

namespace backtest
{
    namespace strategy
    {

        class EMACrossover : public StrategyBase
        {
        public:
            EMACrossover();

            void initialize(const StrategyParams &params) override;
            void calculateIndicators(const std::vector<Bar> &bars) override;
            Signal generateSignal(size_t index, const std::vector<Bar> &bars) override;
            bool isReady(size_t index) const override;
            std::string getName() const override { return "EMA_Crossover"; }
            std::string getParamsString() const override;

        private:
            int fast_period_;
            int slow_period_;
            std::unique_ptr<indicators::EMA> fast_ema_;
            std::unique_ptr<indicators::EMA> slow_ema_;
            bool in_position_;
            bool was_long_;
        };

    } // namespace strategy
} // namespace backtest

#endif // EMA_CROSSOVER_H
