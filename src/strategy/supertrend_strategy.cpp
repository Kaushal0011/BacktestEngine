#include "strategy/supertrend_strategy.h"

namespace backtest
{
    namespace strategy
    {

        SupertrendStrategy::SupertrendStrategy()
            : period_(0), multiplier_(0.0), in_position_(false), last_trend_(0) {}

        void SupertrendStrategy::initialize(const StrategyParams &params)
        {
            if (params.params.size() < 2)
            {
                throw std::runtime_error("Supertrend requires 2 parameters: period, multiplier");
            }

            period_ = static_cast<int>(params.params[0]);
            multiplier_ = params.params[1];

            supertrend_ = std::make_unique<indicators::Supertrend>(period_, multiplier_);

            in_position_ = false;
            last_trend_ = 0;
        }

        void SupertrendStrategy::calculateIndicators(const std::vector<Bar> &bars)
        {
            supertrend_->calculate(bars);
        }

        Signal SupertrendStrategy::generateSignal(size_t index, const std::vector<Bar> &bars)
        {
            if (!isReady(index) || index == 0)
            {
                return Signal::NONE;
            }

            int current_trend = supertrend_->getTrend(index);
            int prev_trend = supertrend_->getTrend(index - 1);

            // Trend changed from bearish to bullish
            if (prev_trend == -1 && current_trend == 1)
            {
                if (in_position_ && last_trend_ == -1)
                {
                    last_trend_ = current_trend;
                    return Signal::EXIT_SHORT;
                }
                if (!in_position_)
                {
                    in_position_ = true;
                    last_trend_ = current_trend;
                    return Signal::LONG;
                }
            }

            // Trend changed from bullish to bearish
            if (prev_trend == 1 && current_trend == -1)
            {
                if (in_position_ && last_trend_ == 1)
                {
                    last_trend_ = current_trend;
                    return Signal::EXIT_LONG;
                }
                if (!in_position_)
                {
                    in_position_ = true;
                    last_trend_ = current_trend;
                    return Signal::SHORT;
                }
            }

            last_trend_ = current_trend;
            return Signal::NONE;
        }

        bool SupertrendStrategy::isReady(size_t index) const
        {
            return supertrend_ && supertrend_->isReady(index);
        }

        std::string SupertrendStrategy::getParamsString() const
        {
            return "Period" + std::to_string(period_) + "_Mult" +
                   std::to_string(static_cast<int>(multiplier_ * 10));
        }

    } // namespace strategy
} // namespace backtest
