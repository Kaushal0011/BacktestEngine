# BacktestEngine - High-Performance Trading Strategy Backtester

A professional-grade backtesting system for intraday trading strategies built with C++ for high-performance execution and Python for comprehensive analytics.

## 🎯 Overview

This project provides a complete backtesting framework designed for testing trading strategies on large intraday datasets. It combines the speed of C++ with the analytical power of Python to deliver fast, reliable, and insightful strategy testing.

### Key Features

- ⚡ **High-Performance C++ Core**: Optimized for speed with multithreaded execution
- 📊 **Modular Indicator System**: Easy-to-extend framework for technical indicators
- 🔄 **Parallel Parameter Optimization**: Test multiple parameter combinations concurrently
- 💾 **Efficient Data Storage**: Uses Apache Parquet for fast I/O operations
- 📈 **Comprehensive Analytics**: Python-based analysis with Sharpe, Sortino, Profit Factor, and more
- 🎨 **Rich Visualizations**: Heatmaps, equity curves, and performance comparisons
- 📑 **Excel Reports**: Detailed performance reports with multiple analysis sheets

---

## 🏗️ Architecture

### System Design

The system follows a modular, layered architecture optimized for performance and extensibility:

```
┌─────────────────────────────────────────────────────────────┐
│                     USER INTERFACE                          │
│              Command Line / Configuration                   │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│                   DATA PREPROCESSING                        │
│         CSV → Parquet Conversion (One-time)                 │
│         DTE Calculation & Data Validation                   │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│                   BACKTEST ENGINE (C++)                     │
│  ┌───────────────┐  ┌────────────────┐  ┌────────────────┐ │
│  │   Indicators  │  │   Strategies   │  │  Trade Logger  │ │
│  │   - EMA       │  │  - Crossover   │  │  - Parquet     │ │
│  │   - SMA       │  │  - Supertrend  │  │  - Buffering   │ │
│  │   - Supertrend│  │  - Custom      │  │  - Thread-safe │ │
│  │   - Keltner   │  │                │  │                │ │
│  └───────────────┘  └────────────────┘  └────────────────┘ │
│                            ↓                                │
│              ┌─────────────────────────┐                    │
│              │ Multithreaded Execution │                    │
│              │  - Parameter Grid       │                    │
│              │  - DTE Filtering        │                    │
│              │  - Position Management  │                    │
│              └─────────────────────────┘                    │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│                ANALYTICS LAYER (Python)                     │
│  ┌──────────────────┐  ┌──────────────────────────────┐    │
│  │ Performance      │  │  Visualization               │    │
│  │ - Sharpe Ratio   │  │  - Equity Curves             │    │
│  │ - Sortino Ratio  │  │  - Performance Heatmaps      │    │
│  │ - Calmar Ratio   │  │  - DTE Analysis              │    │
│  │ - Drawdown       │  │  - Metric Comparisons        │    │
│  │ - Expectancy     │  │                              │    │
│  └──────────────────┘  └──────────────────────────────┘    │
│                            ↓                                │
│                  ┌─────────────────────┐                    │
│                  │   Excel Reports     │                    │
│                  │   PNG Charts        │                    │
│                  └─────────────────────┘                    │
└─────────────────────────────────────────────────────────────┘
```

### Design Patterns

1. **Strategy Pattern**: Modular strategy implementation allowing easy addition of new strategies
2. **Factory Pattern**: Dynamic strategy creation based on configuration
3. **Template Method**: Base indicator class with common calculation patterns
4. **Observer Pattern**: Trade logging with thread-safe observers
5. **Builder Pattern**: Complex parameter combination generation

---

## 📦 Components

### 1. Indicators (`include/indicators/`, `src/indicators/`)

All indicators inherit from `IndicatorBase` and implement:

- `calculate()`: Compute indicator values for all bars
- `getValue()`: Retrieve indicator value at specific index
- `isReady()`: Check if indicator has enough data

**Available Indicators:**

- **EMA (Exponential Moving Average)**: Trend-following indicator with faster response
- **SMA (Simple Moving Average)**: Basic trend-following indicator
- **ATR (Average True Range)**: Volatility measurement
- **Supertrend**: Trend-following indicator combining price and ATR
- **Keltner Channel**: Volatility-based channel indicator

### 2. Strategies (`include/strategy/`, `src/strategy/`)

Strategies implement signal generation logic:

**EMA Crossover Strategy:**

- Parameters: Fast EMA period, Slow EMA period
- Logic: Long when fast crosses above slow, Short when fast crosses below slow

**Supertrend Strategy:**

- Parameters: ATR period, Multiplier
- Logic: Follow trend direction indicated by Supertrend

### 3. Backtesting Engine (`src/backtest_engine.cpp`)

Core features:

- **Position Management**: Automatic entry/exit with capital allocation
- **DTE Filtering**: Test specific days-to-expiry
- **Time Filtering**: Intraday trading hours (09:15 - 15:25)
- **Square-off Logic**: End-of-day position closure
- **Performance Calculation**: Real-time metric computation

### 4. Data Management

**DataLoader** (`src/data_loader.cpp`):

- CSV to Parquet conversion (one-time, 10-50x faster subsequent loads)
- DTE calculation from expiry dates
- Memory-efficient data structures

**TradeLogger** (`src/trade_logger.cpp`):

- Thread-safe trade recording
- Batch Parquet writes for efficiency
- Minimal memory footprint

### 5. Analytics Layer (`analytics/`)

**PerformanceAnalyzer** (`performance_analyzer.py`):

- Comprehensive metric calculation
- Risk-adjusted returns (Sharpe, Sortino, Calmar)
- Drawdown analysis with duration
- Trade statistics

**ResultsAnalyzer** (`analyze_results.py`):

- Multi-strategy comparison
- DTE-based analysis
- Visualization generation
- Excel report creation

---

## 🚀 Installation

### Prerequisites

**System Requirements:**

- Linux/Unix-based system
- GCC 7+ or Clang 5+ (C++17 support)
- CMake 3.15+
- Python 3.8+

**Required Libraries:**

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libarrow-dev \
    libparquet-dev \
    python3-pip

# CentOS/RHEL
sudo yum install -y \
    gcc-c++ \
    cmake \
    arrow-devel \
    parquet-devel \
    python3-pip
```

### Build Instructions

1. **Clone/Navigate to project directory:**

```bash
cd "Backtest Engine"
```

2. **Install Python dependencies:**

```bash
pip3 install -r requirements.txt
```

3. **Build C++ engine:**

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
cd ..
```

4. **Verify installation:**

```bash
./build/backtest_engine --help
```

---

## 📖 Usage

### Quick Start

1. **Convert CSV to Parquet (one-time):**

```bash
./build/backtest_engine --convert-csv
```

2. **Run parameter optimization:**

```bash
./build/backtest_engine --strategy EMA_Crossover --optimize
```

3. **Analyze results:**

```bash
python3 analytics/analyze_results.py
```

### Command Line Options

```bash
./build/backtest_engine [options]

Options:
  --convert-csv       Convert market_data.csv to Parquet format
  --strategy NAME     Strategy name (EMA_Crossover, Supertrend)
  --params P1,P2,...  Strategy parameters (comma-separated)
  --dte N            DTE filter (1-5, or -1 for all)
  --optimize         Run parameter optimization
  --help             Show help message
```

### Examples

**Single Strategy Test:**

```bash
# Test EMA Crossover with fast=10, slow=30 on DTE=2
./build/backtest_engine --strategy EMA_Crossover --params 10,30 --dte 2
```

**Full Optimization:**

```bash
# Test all Supertrend combinations
./build/backtest_engine --strategy Supertrend --optimize
```

**Both Strategies:**

```bash
# Optimize both strategies across all parameters
./build/backtest_engine --optimize
```

---

## 📊 Data Format

### Input CSV Format

Required columns (tab-separated):

```
timestamp                    open      high      low       close     date        weekly_expiry_date  DT
2023-08-25 09:15:00+05:30   19297.4   19301.15  19261.7   19269.25  25-08-2023  31-08-2023         DT-4
```

### Output Files

**Trade Logs** (`output/trades/*.parquet`):

- Entry/Exit timestamps and prices
- PnL and percentage returns
- Direction (LONG/SHORT)
- DTE and strategy parameters

**Analysis Results** (`output/analysis/`):

- `backtest_results.xlsx`: Comprehensive Excel report
- `*_heatmap.png`: Performance heatmaps
- `top_equity_curves.png`: Equity curve visualization
- `metric_comparisons.png`: Side-by-side metric comparison
- `dte_analysis.png`: DTE-based performance analysis

---

## 📈 Performance Metrics

### Trading Metrics

- **Total Trades**: Number of completed trades
- **Win Rate**: Percentage of winning trades
- **Profit Factor**: Gross profit / Gross loss
- **Expectancy**: Average expected profit per trade
- **Max Win/Loss**: Largest winning/losing trade

### Risk-Adjusted Metrics

- **Sharpe Ratio**: Risk-adjusted return (annualized)
- **Sortino Ratio**: Downside risk-adjusted return
- **Calmar Ratio**: Return / Maximum drawdown
- **Maximum Drawdown**: Largest peak-to-trough decline

### Additional Metrics

- **Consecutive Wins/Losses**: Longest winning/losing streaks
- **Average Trade Duration**: Time in position (minutes)
- **Recovery Duration**: Time to recover from max drawdown

---

## ⚙️ Configuration

### Strategy Parameters

**EMA Crossover:**

- Fast Period: 5, 10, 15
- Slow Period: 20, 30, 40, 50
- Total combinations: 3 × 4 × 5 (DTE) = 60

**Supertrend:**

- Period: 7, 10, 14
- Multiplier: 1.5, 2.0, 2.5, 3.0
- Total combinations: 3 × 4 × 5 (DTE) = 60

### DTE (Days to Expiry)

- DTE 1: Expiry day trading
- DTE 2: One day before expiry
- DTE 3: Two days before expiry
- DTE 4: Three days before expiry
- DTE 5: Four+ days before expiry

### Trading Hours

- Entry Window: 09:15 - 15:25 IST
- Auto Square-off: 15:25 IST

---

## 🎯 Best Practices

### Performance Optimization

1. **Always convert CSV to Parquet first** - 10-50x faster loading
2. **Use multithreading** for parameter optimization
3. **Limit parameter combinations** to most relevant ranges
4. **Monitor memory usage** on large datasets

### Strategy Development

1. **Start simple** - Test basic logic first
2. **Validate indicators** - Ensure calculations are correct
3. **Use proper DTE filtering** - Options behavior varies by expiry
4. **Consider transaction costs** - Add slippage/commissions for realism

### Analysis

1. **Don't overfit** - Favor consistent performance over peak returns
2. **Check multiple metrics** - Return alone isn't enough
3. **Analyze by DTE** - Different DTEs may require different parameters
4. **Validate on out-of-sample data** - Reserve data for final validation

---

## 🔍 Troubleshooting

### Build Issues

**Error: arrow/parquet not found**

```bash
# Install Arrow/Parquet development libraries
sudo apt-get install libarrow-dev libparquet-dev
```

**CMake version too old**

```bash
# Install newer CMake
pip3 install cmake --upgrade
```

### Runtime Issues

**Segmentation fault**

- Check data file exists and is valid
- Ensure Parquet file was created successfully
- Verify strategy parameters are valid

**No trades generated**

- Check DTE filter matches data
- Verify indicator periods are smaller than data size
- Ensure trading hours overlap with data timestamps

**Python import errors**

```bash
pip3 install -r requirements.txt --upgrade
```

---
