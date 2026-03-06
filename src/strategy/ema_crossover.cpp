#include "strategy/ema_crossover.h"

namespace backtest
{
    namespace strategy
    {

        EMACrossover::EMACrossover()
            : fast_period_(0), slow_period_(0), in_position_(false), was_long_(false) {}

        void EMACrossover::initialize(const StrategyParams &params)
        {
            if (params.params.size() < 2)
            {
                throw std::runtime_error("EMA Crossover requires 2 parameters: fast_period, slow_period");
            }

            fast_period_ = static_cast<int>(params.params[0]);
            slow_period_ = static_cast<int>(params.params[1]);

            fast_ema_ = std::make_unique<indicators::EMA>(fast_period_);
            slow_ema_ = std::make_unique<indicators::EMA>(slow_period_);

            in_position_ = false;
            was_long_ = false;
        }

        void EMACrossover::calculateIndicators(const std::vector<Bar> &bars)
        {
            fast_ema_->calculate(bars);
            slow_ema_->calculate(bars);
        }

        Signal EMACrossover::generateSignal(size_t index, const std::vector<Bar> &bars)
        {
            if (!isReady(index) || index == 0)
            {
                return Signal::NONE;
            }

            double fast_current = fast_ema_->getValue(index);
            double slow_current = slow_ema_->getValue(index);
            double fast_prev = fast_ema_->getValue(index - 1);
            double slow_prev = slow_ema_->getValue(index - 1);

            // Bullish crossover
            if (fast_prev <= slow_prev && fast_current > slow_current)
            {
                if (in_position_ && !was_long_)
                {
                    return Signal::EXIT_SHORT;
                }
                if (!in_position_)
                {
                    in_position_ = true;
                    was_long_ = true;
                    return Signal::LONG;
                }
            }

            // Bearish crossover
            if (fast_prev >= slow_prev && fast_current < slow_current)
            {
                if (in_position_ && was_long_)
                {
                    return Signal::EXIT_LONG;
                }
                if (!in_position_)
                {
                    in_position_ = true;
                    was_long_ = false;
                    return Signal::SHORT;
                }
            }

            return Signal::NONE;
        }

        bool EMACrossover::isReady(size_t index) const
        {
            return fast_ema_ && slow_ema_ &&
                   fast_ema_->isReady(index) && slow_ema_->isReady(index);
        }

        std::string EMACrossover::getParamsString() const
        {
            return "Fast" + std::to_string(fast_period_) + "_Slow" + std::to_string(slow_period_);
        }

    } // namespace strategy
} // namespace backtest
