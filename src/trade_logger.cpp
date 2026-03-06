#include "trade_logger.h"
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/writer.h>
#include <iostream>

namespace backtest
{

    TradeLogger::TradeLogger() {}

    void TradeLogger::logTrade(const Trade &trade)
    {
        trades_.push_back(trade);
    }

    void TradeLogger::logTradeThreadSafe(const Trade &trade)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        trades_.push_back(trade);
    }

    void TradeLogger::clear()
    {
        trades_.clear();
    }

    bool TradeLogger::saveToParquet(const std::string &filepath)
    {
        if (trades_.empty())
        {
            std::cout << "No trades to save." << std::endl;
            return true;
        }

        // Create Arrow schema
        auto schema = arrow::schema({arrow::field("entry_time", arrow::utf8()),
                                     arrow::field("exit_time", arrow::utf8()),
                                     arrow::field("entry_date", arrow::utf8()),
                                     arrow::field("exit_date", arrow::utf8()),
                                     arrow::field("entry_price", arrow::float64()),
                                     arrow::field("exit_price", arrow::float64()),
                                     arrow::field("quantity", arrow::float64()),
                                     arrow::field("pnl", arrow::float64()),
                                     arrow::field("pnl_percentage", arrow::float64()),
                                     arrow::field("direction", arrow::utf8()),
                                     arrow::field("dte", arrow::int32()),
                                     arrow::field("strategy_name", arrow::utf8()),
                                     arrow::field("parameters", arrow::utf8())});

        // Create builders
        arrow::StringBuilder entry_time_builder, exit_time_builder;
        arrow::StringBuilder entry_date_builder, exit_date_builder;
        arrow::DoubleBuilder entry_price_builder, exit_price_builder;
        arrow::DoubleBuilder quantity_builder, pnl_builder, pnl_pct_builder;
        arrow::StringBuilder direction_builder;
        arrow::Int32Builder dte_builder;
        arrow::StringBuilder strategy_builder, params_builder;

        // Append data
        for (const auto &trade : trades_)
        {
            entry_time_builder.Append(trade.entry_time);
            exit_time_builder.Append(trade.exit_time);
            entry_date_builder.Append(trade.entry_date);
            exit_date_builder.Append(trade.exit_date);
            entry_price_builder.Append(trade.entry_price);
            exit_price_builder.Append(trade.exit_price);
            quantity_builder.Append(trade.quantity);
            pnl_builder.Append(trade.pnl);
            pnl_pct_builder.Append(trade.pnl_percentage);
            direction_builder.Append(trade.direction);
            dte_builder.Append(trade.dte);
            strategy_builder.Append(trade.strategy_name);
            params_builder.Append(trade.parameters);
        }

        // Finish arrays
        std::shared_ptr<arrow::Array> entry_time_array, exit_time_array;
        std::shared_ptr<arrow::Array> entry_date_array, exit_date_array;
        std::shared_ptr<arrow::Array> entry_price_array, exit_price_array;
        std::shared_ptr<arrow::Array> quantity_array, pnl_array, pnl_pct_array;
        std::shared_ptr<arrow::Array> direction_array, dte_array;
        std::shared_ptr<arrow::Array> strategy_array, params_array;

        entry_time_builder.Finish(&entry_time_array);
        exit_time_builder.Finish(&exit_time_array);
        entry_date_builder.Finish(&entry_date_array);
        exit_date_builder.Finish(&exit_date_array);
        entry_price_builder.Finish(&entry_price_array);
        exit_price_builder.Finish(&exit_price_array);
        quantity_builder.Finish(&quantity_array);
        pnl_builder.Finish(&pnl_array);
        pnl_pct_builder.Finish(&pnl_pct_array);
        direction_builder.Finish(&direction_array);
        dte_builder.Finish(&dte_array);
        strategy_builder.Finish(&strategy_array);
        params_builder.Finish(&params_array);

        // Create table
        auto table = arrow::Table::Make(schema, {entry_time_array, exit_time_array, entry_date_array, exit_date_array,
                                                 entry_price_array, exit_price_array, quantity_array, pnl_array,
                                                 pnl_pct_array, direction_array, dte_array, strategy_array, params_array});

        // Write to Parquet
        std::shared_ptr<arrow::io::FileOutputStream> outfile;
        PARQUET_ASSIGN_OR_THROW(
            outfile,
            arrow::io::FileOutputStream::Open(filepath));

        PARQUET_THROW_NOT_OK(
            parquet::arrow::WriteTable(*table, arrow::default_memory_pool(),
                                       outfile, 50000));

        std::cout << "Saved " << trades_.size() << " trades to: " << filepath << std::endl;
        return true;
    }

} // namespace backtest
