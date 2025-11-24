# Keltner Channel Backtesting Engine

A high-performance C++ backtesting engine for trading strategies using the Keltner Channel indicator. This system is designed with professional-grade architecture, utilizing object-oriented programming principles and multithreading for optimal performance.

## Features

- **High-Performance C++ Implementation**: Built for speed and efficiency
- **Keltner Channel Indicator**: Configurable EMA period, ATR period, and multiplier
- **Multithreaded Backtesting**: Simultaneous testing of multiple parameter combinations
- **Intraday Trading Logic**: Automatic end-of-day position exit
- **Comprehensive Trade Logs**: Detailed CSV exports with all trade metrics
- **Performance Analytics**: Profit/loss, max profit, max drawdown, Calmar ratio, and more

## Architecture

The system follows clean object-oriented design with the following components:

- **MarketData**: Manages OHLCV data structures
- **Trade**: Encapsulates individual trade information and metrics
- **BacktestResult**: Stores results for each parameter combination
- **KeltnerIndicator**: Calculates Keltner Channel bands (EMA, ATR, upper/lower bands)
- **CSVParser**: Efficient CSV file parsing
- **TradingStrategy**: Implements intraday trading logic with EOD exits
- **BacktestEngine**: Multithreaded backtesting orchestrator
- **TradeLogger**: CSV export for trade logs and summary reports

## Building the Project

### Prerequisites

- C++17 compatible compiler (clang++ or g++)
- Make utility

### Compilation

```bash
make
```

This will compile the project with optimization flags (-O3) and create the `backtest_engine` executable.

## Running the Backtest

### Quick Start

```bash
make run
```

Or run directly:

```bash
./backtest_engine
```

### Custom Data File

```bash
./backtest_engine path/to/your/data.csv
```

## Input Data Format

The CSV file should have the following columns:

```csv
timestamp,open,high,low,close,volume
2024-01-02 09:15:00,21450.50,21475.25,21440.00,21460.75,1250000
```

## Configuration

Edit `src/main.cpp` to customize:

- **Capital**: Default is Rs 2,000,000
- **EMA Periods**: Currently testing [10, 20, 30, 50]
- **ATR Periods**: Currently testing [10, 14, 20]
- **Multipliers**: Currently testing [1.5, 2.0, 2.5, 3.0]

## Output Files

All results are saved in the `output/` directory:

### Individual Trade Logs

- **Filename**: `tradelog_EMA{X}_ATR{Y}_MULT{Z}.csv`
- **Contains**: Entry/exit timestamps, prices, Keltner bands, P&L, max profit, max drawdown, Calmar ratio

### Summary Report

- **Filename**: `summary_report.csv`
- **Contains**: Aggregated statistics for all parameter combinations

## Trade Log Columns

Each trade log includes:

- Entry Timestamp
- Exit Timestamp
- DTE (Days to Expiry)
- Entry Price
- Exit Price
- EMA Period, ATR Period, Multiplier
- Entry Keltner Upper/Lower Bands
- Exit Keltner Upper/Lower Bands
- Investment Amount
- Quantity
- Direction (LONG/SHORT)
- Profit/Loss (Rs)
- Profit/Loss (%)
- Max Profit During Trade
- Max Drawdown During Trade
- Calmar Ratio

## Performance Metrics

The engine calculates:

- **Total P&L**: Absolute and percentage returns
- **Win Rate**: Percentage of profitable trades
- **Max Drawdown**: Maximum equity decline
- **Calmar Ratio**: Return/drawdown ratio
- **Average Profit per Trade**

## Trading Logic

### Entry Signals

- **Long**: Price closes above upper Keltner band
- **Short**: Price closes below lower Keltner band

### Exit Signals

- **Long**: Price closes below middle band (EMA)
- **Short**: Price closes above middle band (EMA)
- **End of Day**: All positions closed automatically

## Multithreading

The engine automatically detects available CPU cores and runs backtests in parallel, significantly reducing execution time for large parameter sets.

## Project Structure

```
.
├── include/           # Header files
│   ├── MarketData.hpp
│   ├── Trade.hpp
│   ├── BacktestResult.hpp
│   ├── KeltnerIndicator.hpp
│   ├── CSVParser.hpp
│   ├── TradingStrategy.hpp
│   ├── BacktestEngine.hpp
│   └── TradeLogger.hpp
├── src/              # Source files
│   └── main.cpp
├── data/             # Market data CSV files
├── output/           # Generated trade logs and reports
├── Makefile          # Build configuration
└── README.md         # This file
```

## Clean Build

```bash
make clean
```

## Example Output

```
========================================
Starting Parallel Backtests
========================================
Total parameter combinations: 48
Number of threads: 8
Capital per backtest: Rs 2000000
Market data bars: 85
========================================

Completed: EMA=10, ATR=10, Mult=1.5 | Trades: 5 | P&L: Rs 12450.50
...

========================================
All backtests completed!
Total results: 48
========================================
```

## License

This project is provided as-is for educational and research purposes.
