#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <sstream>

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
    std::ostringstream rows;

    std::string header;

public:
    CsvSaver() { }

    CsvSaver(const char* path)
        : path(path)
    {
        Open(path);
    }

    void AddHeader(const std::string& header)
    {
        this->header = header;
    }

    void AddRowRaw(const std::string& row)
    {
        rows << row << "\n";
    }

    template <CsvRow T>
    void AddRow(T& obj)
    {
        rows << obj.ToCsvRow() << "\n";
    }

    void Open(const char* path)
    {
        this->path = path;
        file = std::ofstream(path);
    }

    void Flush()
    {
        file << header << "\n" << rows.str();
    }

    void Close()
    {
        file.close();
    }
};
