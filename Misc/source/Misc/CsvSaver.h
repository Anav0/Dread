#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <sstream>

template <typename T>
concept CsvRow = requires(const T& t, std::ostringstream& rows) {
    {
        t.ToCsvRow(rows)
    } -> std::same_as<void>;
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
        obj.ToCsvRow(rows);
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
