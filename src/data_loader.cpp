#include "data_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <algorithm>
#include <iomanip>

namespace backtest
{

    int DataLoader::calculateDTE(const std::string &current_date,
                                 const std::string &expiry_date)
    {
        // Simple DTE calculation based on date difference
        // Format: DD-MM-YYYY
        auto parseDate = [](const std::string &date) -> std::tm
        {
            std::tm tm = {};
            std::istringstream ss(date);
            ss >> std::get_time(&tm, "%d-%m-%Y");
            return tm;
        };

        std::tm current = parseDate(current_date);
        std::tm expiry = parseDate(expiry_date);

        std::time_t current_time = std::mktime(&current);
        std::time_t expiry_time = std::mktime(&expiry);

        double diff_seconds = std::difftime(expiry_time, current_time);
        int days = static_cast<int>(diff_seconds / (60 * 60 * 24));

        return std::max(1, std::min(5, days + 1)); // Clamp to 1-5
    }

    std::vector<Bar> DataLoader::loadFromCSV(const std::string &csv_path)
    {
        std::vector<Bar> bars;
        std::ifstream file(csv_path);

        if (!file.is_open())
        {
            std::cerr << "Error: Cannot open CSV file: " << csv_path << std::endl;
            return bars;
        }

        std::string line;
        std::getline(file, line); // Skip header

        while (std::getline(file, line))
        {
            std::istringstream ss(line);
            Bar bar;
            std::string field;

            std::getline(ss, bar.timestamp, '\t');
            std::getline(ss, field, '\t');
            bar.open = std::stod(field);
            std::getline(ss, field, '\t');
            bar.high = std::stod(field);
            std::getline(ss, field, '\t');
            bar.low = std::stod(field);
            std::getline(ss, field, '\t');
            bar.close = std::stod(field);
            std::getline(ss, bar.date, '\t');
            std::getline(ss, bar.weekly_expiry_date, '\t');
            std::getline(ss, bar.dt, '\t');

            // Calculate DTE
            bar.dte = calculateDTE(bar.date, bar.weekly_expiry_date);

            bars.push_back(bar);
        }

        file.close();
        std::cout << "Loaded " << bars.size() << " bars from CSV" << std::endl;
        return bars;
    }

    bool DataLoader::convertCSVToParquet(const std::string &csv_path,
                                         const std::string &parquet_path)
    {
        std::cout << "Converting CSV to Parquet..." << std::endl;

        // Load CSV data
        std::vector<Bar> bars = loadFromCSV(csv_path);
        if (bars.empty())
        {
            return false;
        }

        // Create Arrow schema
        auto schema = arrow::schema({arrow::field("timestamp", arrow::utf8()),
                                     arrow::field("open", arrow::float64()),
                                     arrow::field("high", arrow::float64()),
                                     arrow::field("low", arrow::float64()),
                                     arrow::field("close", arrow::float64()),
                                     arrow::field("date", arrow::utf8()),
                                     arrow::field("weekly_expiry_date", arrow::utf8()),
                                     arrow::field("dt", arrow::utf8()),
                                     arrow::field("dte", arrow::int32())});

        // Create builders
        arrow::StringBuilder timestamp_builder;
        arrow::DoubleBuilder open_builder, high_builder, low_builder, close_builder;
        arrow::StringBuilder date_builder, expiry_builder, dt_builder;
        arrow::Int32Builder dte_builder;

        // Append data
        for (const auto &bar : bars)
        {
            timestamp_builder.Append(bar.timestamp);
            open_builder.Append(bar.open);
            high_builder.Append(bar.high);
            low_builder.Append(bar.low);
            close_builder.Append(bar.close);
            date_builder.Append(bar.date);
            expiry_builder.Append(bar.weekly_expiry_date);
            dt_builder.Append(bar.dt);
            dte_builder.Append(bar.dte);
        }

        // Finish arrays
        std::shared_ptr<arrow::Array> timestamp_array, open_array, high_array,
            low_array, close_array, date_array,
            expiry_array, dt_array, dte_array;
        timestamp_builder.Finish(&timestamp_array);
        open_builder.Finish(&open_array);
        high_builder.Finish(&high_array);
        low_builder.Finish(&low_array);
        close_builder.Finish(&close_array);
        date_builder.Finish(&date_array);
        expiry_builder.Finish(&expiry_array);
        dt_builder.Finish(&dt_array);
        dte_builder.Finish(&dte_array);

        // Create table
        auto table = arrow::Table::Make(schema, {timestamp_array, open_array, high_array, low_array, close_array,
                                                 date_array, expiry_array, dt_array, dte_array});

        // Write to Parquet
        std::shared_ptr<arrow::io::FileOutputStream> outfile;
        PARQUET_ASSIGN_OR_THROW(
            outfile,
            arrow::io::FileOutputStream::Open(parquet_path));

        PARQUET_THROW_NOT_OK(
            parquet::arrow::WriteTable(*table, arrow::default_memory_pool(),
                                       outfile, 100000));

        std::cout << "Successfully converted to Parquet: " << parquet_path << std::endl;
        return true;
    }

    std::vector<Bar> DataLoader::loadFromParquet(const std::string &parquet_path)
    {
        std::vector<Bar> bars;

        std::shared_ptr<arrow::io::ReadableFile> infile;
        PARQUET_ASSIGN_OR_THROW(
            infile,
            arrow::io::ReadableFile::Open(parquet_path));

        std::unique_ptr<parquet::arrow::FileReader> reader;
        PARQUET_THROW_NOT_OK(
            parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader));

        std::shared_ptr<arrow::Table> table;
        PARQUET_THROW_NOT_OK(reader->ReadTable(&table));

        // Extract columns
        auto timestamp_col = std::static_pointer_cast<arrow::StringArray>(table->column(0)->chunk(0));
        auto open_col = std::static_pointer_cast<arrow::DoubleArray>(table->column(1)->chunk(0));
        auto high_col = std::static_pointer_cast<arrow::DoubleArray>(table->column(2)->chunk(0));
        auto low_col = std::static_pointer_cast<arrow::DoubleArray>(table->column(3)->chunk(0));
        auto close_col = std::static_pointer_cast<arrow::DoubleArray>(table->column(4)->chunk(0));
        auto date_col = std::static_pointer_cast<arrow::StringArray>(table->column(5)->chunk(0));
        auto expiry_col = std::static_pointer_cast<arrow::StringArray>(table->column(6)->chunk(0));
        auto dt_col = std::static_pointer_cast<arrow::StringArray>(table->column(7)->chunk(0));
        auto dte_col = std::static_pointer_cast<arrow::Int32Array>(table->column(8)->chunk(0));

        // Fill bars
        bars.reserve(table->num_rows());
        for (int64_t i = 0; i < table->num_rows(); ++i)
        {
            Bar bar;
            bar.timestamp = timestamp_col->GetString(i);
            bar.open = open_col->Value(i);
            bar.high = high_col->Value(i);
            bar.low = low_col->Value(i);
            bar.close = close_col->Value(i);
            bar.date = date_col->GetString(i);
            bar.weekly_expiry_date = expiry_col->GetString(i);
            bar.dt = dt_col->GetString(i);
            bar.dte = dte_col->Value(i);
            bars.push_back(bar);
        }

        std::cout << "Loaded " << bars.size() << " bars from Parquet" << std::endl;
        return bars;
    }

} // namespace backtest
