#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "MarketData.hpp"

class CSVParser
{
private:
    std::string trim(const std::string &str) const
    {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
            return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

public:
    CSVParser() {}

    bool loadFromFile(const std::string &filename, MarketData &market_data)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return false;
        }

        std::string line;
        bool first_line = true;
        int line_number = 0;

        while (std::getline(file, line))
        {
            line_number++;

            if (first_line)
            {
                first_line = false;
                continue;
            }

            if (line.empty() || trim(line).empty())
            {
                continue;
            }

            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;

            while (std::getline(ss, token, ','))
            {
                tokens.push_back(trim(token));
            }

            if (tokens.size() < 6)
            {
                std::cerr << "Warning: Line " << line_number << " has insufficient columns. Skipping." << std::endl;
                continue;
            }

            try
            {
                OHLCV bar;
                bar.timestamp = tokens[0];
                bar.open = std::stod(tokens[1]);
                bar.high = std::stod(tokens[2]);
                bar.low = std::stod(tokens[3]);
                bar.close = std::stod(tokens[4]);
                bar.volume = std::stol(tokens[5]);

                market_data.addBar(bar);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Warning: Error parsing line " << line_number << ": " << e.what() << std::endl;
                continue;
            }
        }

        file.close();
        std::cout << "Loaded " << market_data.size() << " bars from " << filename << std::endl;
        return true;
    }
};

#endif // CSV_PARSER_H
