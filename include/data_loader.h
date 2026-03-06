#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include "data_structures.h"
#include <vector>
#include <string>

namespace backtest
{

    class DataLoader
    {
    public:
        // Convert CSV to Parquet (one-time operation)
        static bool convertCSVToParquet(const std::string &csv_path,
                                        const std::string &parquet_path);

        // Load data from Parquet file
        static std::vector<Bar> loadFromParquet(const std::string &parquet_path);

        // Calculate DTE from dates
        static int calculateDTE(const std::string &current_date,
                                const std::string &expiry_date);

    private:
        static std::vector<Bar> loadFromCSV(const std::string &csv_path);
    };

} // namespace backtest

#endif // DATA_LOADER_H
