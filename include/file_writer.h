#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <iostream>
#include <fstream>

class FileWriter {
    private:
        std::ofstream out;

    public:
        FileWriter(const std::string &file);
        FileWriter(FileWriter &other) = delete;
        ~FileWriter();

        std::ofstream& operator*() {
            return out;
        }
};

#endif
