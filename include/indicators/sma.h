#ifndef SMA_H
#define SMA_H

#include "indicator_base.h"

namespace backtest {
namespace indicators {

class SMA : public IndicatorBase {
public:
    explicit SMA(int period);
    
    void calculate(const std::vector<Bar>& bars) override;
    double getValue(size_t index) const override;
    bool isReady(size_t index) const override;
    
private:
    int period_;
};

} // namespace indicators
} // namespace backtest

#endif // SMA_H
