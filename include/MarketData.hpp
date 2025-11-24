#ifndef MARKET_DATA_H
#define MARKET_DATA_H

#include <string>
#include <vector>
#include <ctime>

struct OHLCV {
    std::string timestamp;
    time_t epoch_time;
    double open;
    double high;
    double low;
    double close;
    long volume;
    
    OHLCV() : epoch_time(0), open(0), high(0), low(0), close(0), volume(0) {}
    
    OHLCV(const std::string& ts, double o, double h, double l, double c, long v) 
        : timestamp(ts), epoch_time(0), open(o), high(h), low(l), close(c), volume(v) {}
};

class MarketData {
private:
    std::vector<OHLCV> data;
    
public:
    MarketData() {}
    
    void addBar(const OHLCV& bar) {
        data.push_back(bar);
    }
    
    const std::vector<OHLCV>& getData() const {
        return data;
    }
    
    size_t size() const {
        return data.size();
    }
    
    const OHLCV& operator[](size_t index) const {
        return data[index];
    }
    
    void clear() {
        data.clear();
    }
};

#endif // MARKET_DATA_H
