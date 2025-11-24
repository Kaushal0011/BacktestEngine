# Backtesting Engine

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

- C++17 compatible compiler
- Make utility

### Compilation

```bash
make
```

## Running the Backtest

### Quick Start

```bash
make run
```

## Input Data Format

The CSV file should have the following columns:

```csv
timestamp,open,high,low,close,volume
2024-01-02 09:15:00,21450.50,21475.25,21440.00,21460.75,1250000
```
The sample file is stored in the data folder as market_data_sample.csv

## Output Files

All results are saved in the `output/` directory:

### Individual Trade Logs

- **Filename**: `tradelog_EMA{X}_ATR{Y}_MULT{Z}.csv`
- **Contains**: Entry/exit timestamps, prices, Keltner bands, P&L, max profit, max drawdown, Calmar ratio

### Summary Report

- **Filename**: `summary_report.csv`
- **Contains**: Aggregated statistics for all parameter combinations
