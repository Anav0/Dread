#pragma once

#include <fstream>
#include <string>
#include <vector>

template <typename T>
concept CsvRow = requires(const T& t) {
    {
        t.ToCsvRow()
    } -> std::convertible_to<std::string>;
};

class CsvSaver {
private:
    const char* path;
    std::ofstream file;

    std::string header;
    std::vector<std::string> rows;

public:
    CsvSaver(const char* path)
        : path(path)
    {
        Open(path);
    }

    void AddHeader(const std::string& header)
    {
        this->header = header;
    }

    template <CsvRow R>
    void AddRow(R& obj)
    {
        rows.push_back(obj.ToCsvRow());
    }

    void Open(const char* path)
    {
        this->path = path;
        file = std::ofstream(path);
    }

    void Flush()
    {
        file << header << "\n";
        for (auto& row : rows) {
            file << row << "\n";
        }
    }

    void Close()
    {
        file.close();
    }
};
