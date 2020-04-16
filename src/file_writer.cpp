#include "file_writer.h"

FileWriter::FileWriter(const std::string &file): out(std::ofstream(file)) {}

FileWriter::~FileWriter() {
    out.close();
}

