#ifndef SUPERTREND_STRATEGY_H
#define SUPERTREND_STRATEGY_H

#include "strategy_base.h"
#include "../indicators/supertrend.h"
#include <memory>

namespace backtest
{
    namespace strategy
    {

        class SupertrendStrategy : public StrategyBase
        {
        public:
            SupertrendStrategy();

            void initialize(const StrategyParams &params) override;
            void calculateIndicators(const std::vector<Bar> &bars) override;
            Signal generateSignal(size_t index, const std::vector<Bar> &bars) override;
            bool isReady(size_t index) const override;
            std::string getName() const override { return "Supertrend"; }
            std::string getParamsString() const override;

        private:
            int period_;
            double multiplier_;
            std::unique_ptr<indicators::Supertrend> supertrend_;
            bool in_position_;
            int last_trend_;
        };

    } // namespace strategy
} // namespace backtest

#endif // SUPERTREND_STRATEGY_H
