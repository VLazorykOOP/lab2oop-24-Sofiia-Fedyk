#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Функція для підрахунку кількості одиниць у числі
int countOnes(unsigned short value) {
    int count = 0;
    while (value) {
        count += value & 1;
        value >>= 1;
    }
    return count;
}

// Функція для шифрування символу
unsigned short encryptChar(char ch, int row, int position) {
    unsigned short result = 0;
    
    // Додаємо номер рядка (біти 0-1)
    result |= (row & 0x3);
    
    // Додаємо позицію символу в рядку (біти 2-6)
    result |= ((position & 0x1F) << 2);
    
    // Додаємо ASCII-код символу (біти 7-14)
    result |= ((unsigned char)ch << 7);
    
    // Обчислюємо біт парності (біт 15)
    int ones = countOnes(result);
    if (ones % 2 == 0) {
        result |= (1 << 15); // Додаємо 1 в 15-й біт, щоб загальна кількість одиниць була непарною
    }
    
    return result;
}

int main(int argc, char* argv[]) {
    const int ROW_COUNT = 4;
    const int MAX_CHARS = 32;
    std::string inputFileName = "input.txt";
    std::string outputFileName = "encrypted.bin";
    
    // Перевірка аргументів командного рядка
    if (argc > 1) {
        inputFileName = argv[1];
    }
    if (argc > 2) {
        outputFileName = argv[2];
    }
    
    std::vector<std::string> rows(ROW_COUNT);
    
    // Спробуємо прочитати з файлу
    std::ifstream inputFile(inputFileName);
    if (inputFile.is_open()) {
        std::cout << "Читання даних з файлу " << inputFileName << std::endl;
        for (int i = 0; i < ROW_COUNT && !inputFile.eof(); i++) {
            std::getline(inputFile, rows[i]);
        }
        inputFile.close();
    } else {
        std::cout << "Не вдалося відкрити файл " << inputFileName << std::endl;
        std::cout << "Введіть " << ROW_COUNT << " рядки тексту (до " << MAX_CHARS << " символів кожен):" << std::endl;
        
        // Зчитуємо рядки з консолі
        for (int i = 0; i < ROW_COUNT; i++) {
            std::cout << "Рядок " << (i + 1) << ": ";
            std::getline(std::cin, rows[i]);
        }
    }
    
    // Доповнюємо рядки пробілами до MAX_CHARS символів
    for (int i = 0; i < ROW_COUNT; i++) {
        // Обрізаємо рядок, якщо він довший за MAX_CHARS символів
        if (rows[i].length() > MAX_CHARS) {
            rows[i] = rows[i].substr(0, MAX_CHARS);
        }
        
        // Доповнюємо пробілами до MAX_CHARS символів
        rows[i].resize(MAX_CHARS, ' ');
        
        std::cout << "Рядок " << (i + 1) << " (після обробки): \"" << rows[i] << "\"" << std::endl;
    }
    
    // Відкриваємо бінарний файл для запису
    std::ofstream outputFile(outputFileName, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Помилка: Не вдалося відкрити файл " << outputFileName << " для запису." << std::endl;
        return 1;
    }
    
    // Шифруємо кожен символ і записуємо в файл
    for (int rowIndex = 0; rowIndex < ROW_COUNT; rowIndex++) {
        for (int pos = 0; pos < MAX_CHARS; pos++) {
            unsigned short encryptedChar = encryptChar(rows[rowIndex][pos], rowIndex, pos);
            outputFile.write(reinterpret_cast<char*>(&encryptedChar), sizeof(encryptedChar));
        }
    }
    
    outputFile.close();
    
    std::cout << "Шифрування завершено. Результат записано у файл " << outputFileName << std::endl;
    
    return 0;
}