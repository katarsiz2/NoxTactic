#pragma once
#include <iostream>
#include <fstream>
#include <string>

class File
{
    std::ifstream* in;
    std::ofstream* out;
    bool input_open, output_open;
public:
    bool IsOpen() const { return input_open || output_open; }
    void Open_Input(const std::string& filepath) {
        if (IsOpen()) { Close(); }
        input_open = true;
        in->open(filepath);
    }
    void Open_Ouput(const std::string& filepath) {
        if (IsOpen()) { Close(); }
        output_open = true;
        out->open(filepath);
    }
    void Close() {
        if (input_open) {
            in->close();
        }
        if (output_open) {
            out->close();
        }
        input_open = output_open = false;
    }
    template <class T>
    File& operator<<(const T& x) {
        if (!output_open) {
            throw("File not open");
        }
        out << x;
        return *this;
    }
    template <class T>
    File& operator>>(T& x) {
        if (!input_open) {
            throw("File not open");
        }
        in >> x;
        return *this;
    }
};