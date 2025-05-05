#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <csignal>
#include <zlib.h>

volatile std::sig_atomic_t interrupted = 0;

void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "Операция прервана пользователем (Ctrl+C)\n";
        interrupted = 1;
    }
}

bool compressFile(const std::string& inFilename, const std::string& outFilename) {
    std::ifstream inFile(inFilename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Ошибка открытия входного файла: " << inFilename << "\n";
        return false;
    }
    std::ofstream outFile(outFilename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Ошибка создания архива: " << outFilename << "\n";
        return false;
    }
    std::vector<char> inputBuffer(1024 * 1024);
    std::vector<char> outputBuffer(compressBound(inputBuffer.size()));
    while (!inFile.eof() && !interrupted) {
        inFile.read(inputBuffer.data(), inputBuffer.size());
        std::streamsize bytesRead = inFile.gcount();
        uLongf compressedSize = outputBuffer.size();
        int res = compress(reinterpret_cast<Bytef*>(outputBuffer.data()), &compressedSize,
            reinterpret_cast<const Bytef*>(inputBuffer.data()), bytesRead);
        if (res != Z_OK) {
            std::cerr << "Ошибка сжатия данных\n";
            return false;
        }
        outFile.write(reinterpret_cast<const char*>(&compressedSize), sizeof(compressedSize));
        outFile.write(outputBuffer.data(), compressedSize);
    }
    std::cout << "Архивация завершена.\n";
    return !interrupted;
}

bool decompressFile(const std::string& inFilename, const std::string& outFilename) {
    std::ifstream inFile(inFilename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Ошибка открытия архива: " << inFilename << "\n";
        return false;
    }
    std::ofstream outFile(outFilename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Ошибка создания выходного файла: " << outFilename << "\n";
        return false;
    }
    std::vector<char> compressedBuffer(1024 * 1024);
    std::vector<char> decompressedBuffer(1024 * 1024);
    while (!inFile.eof() && !interrupted) {
        uLongf compressedSize;
        inFile.read(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize));
        if (inFile.eof()) break;
        if (compressedSize > compressedBuffer.size()) {
            compressedBuffer.resize(compressedSize);
        }
        inFile.read(compressedBuffer.data(), compressedSize);
        if (inFile.gcount() != compressedSize) {
            std::cerr << "Ошибка чтения сжатых данных.\n";
            return false;
        }
        uLongf decompressedSize = decompressedBuffer.size();
        int res = uncompress(reinterpret_cast<Bytef*>(decompressedBuffer.data()), &decompressedSize,
            reinterpret_cast<const Bytef*>(compressedBuffer.data()), compressedSize);
        if (res != Z_OK) {
            std::cerr << "Ошибка распаковки.\n";
            return false;
        }
        outFile.write(decompressedBuffer.data(), decompressedSize);
    }
    std::cout << "Распаковка завершена.\n";
    return !interrupted;
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, signal_handler);
    if (argc != 4) {
        std::cerr << "Использование:\n";
        std::cerr << "  " << argv[0] << " a <входной_файл> <архив>\n";
        std::cerr << "  " << argv[0] << " e <архив> <выходной_файл>\n";
        return 1;
    }
    std::string mode = argv[1];
    std::string input = argv[2];
    std::string output = argv[3];
    bool success = false;
    if (mode == "a") {
        std::cout << "Архивация файла " << input << " -> " << output << "\n";
        success = compressFile(input, output);
    }
    else if (mode == "e") {
        std::cout << "Распаковка файла " << input << " -> " << output << "\n";
        success = decompressFile(input, output);
    }
    else {
        std::cerr << "Неизвестный режим. Используйте 'a' для архивации или 'e' для распаковки.\n";
        return 1;
    }
    return success ? 0 : 1;
}
