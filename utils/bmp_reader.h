#include "image.h"
#include "exceptions.h"

#include <fstream>
#include <map>
#include <string>

namespace bmp_reader {
uint32_t ByteRead(uint8_t* array, size_t start, size_t length);

template <typename T>
void ByteWrite(uint8_t* array, const T& data, size_t start, size_t length);

Image* ReadFile(const std::string& file_path, Image* img);

void SaveFile(const std::string& output_path, Image* img);
};  // namespace bmp_reader
