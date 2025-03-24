#include <cstdio>
#include <fstream>
#include <cstring>

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

// Функція для зчитування рядка з консолі
void readLine(char* buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        // Видаляємо символ нового рядка, якщо він є
        int len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
    }
}

int main(int argc, char* argv[]) {
    const int ROW_COUNT = 4;
    const int MAX_CHARS = 32;
    const char* inputFileName = "in.txt";
    const char* outputFileName = "outb.bin";
    
    // Перевірка аргументів командного рядка
    if (argc > 1) {
        inputFileName = argv[1];
    }
    if (argc > 2) {
        outputFileName = argv[2];
    }
    
    // Масив символів для кожного рядка
    char rows[ROW_COUNT][MAX_CHARS + 1]; // +1 для нуль-термінатора
    
    // Спробуємо прочитати з файлу
    FILE* inputFile = fopen(inputFileName, "r");
    if (inputFile != NULL) {
        printf("Читання даних з файлу %s\n", inputFileName);
        for (int i = 0; i < ROW_COUNT; i++) {
            // Очистимо буфер рядка
            memset(rows[i], 0, MAX_CHARS + 1);
            
            // Зчитуємо рядок з файлу
            if (fgets(rows[i], MAX_CHARS + 1, inputFile) == NULL) {
                // Досягнуто кінця файлу або сталася помилка
                break;
            }
            
            // Видаляємо символ нового рядка, якщо він є
            int len = strlen(rows[i]);
            if (len > 0 && rows[i][len-1] == '\n') {
                rows[i][len-1] = '\0';
            }
        }
        fclose(inputFile);
    } else {
        printf("Не вдалося відкрити файл %s\n", inputFileName);
        printf("Введіть %d рядки тексту (до %d символів кожен):\n", ROW_COUNT, MAX_CHARS);
        
        // Зчитуємо рядки з консолі
        for (int i = 0; i < ROW_COUNT; i++) {
            printf("Рядок %d: ", (i + 1));
            // Очистимо буфер рядка
            memset(rows[i], 0, MAX_CHARS + 1);
            readLine(rows[i], MAX_CHARS + 1);
        }
    }
    
    // Доповнюємо рядки пробілами до MAX_CHARS символів
    for (int i = 0; i < ROW_COUNT; i++) {
        size_t len = strlen(rows[i]);
        
        // Заповнюємо решту символів пробілами
        for (size_t j = len; j < MAX_CHARS; j++) {
            rows[i][j] = ' ';
        }
        
        // Встановлюємо нуль-термінатор після MAX_CHARS символів
        rows[i][MAX_CHARS] = '\0';
        
        printf("Рядок %d (після обробки): \"%s\"\n", (i + 1), rows[i]);
    }
    
    // Відкриваємо бінарний файл для запису
    FILE* outputFile = fopen(outputFileName, "wb");
    if (outputFile == NULL) {
        fprintf(stderr, "Помилка: Не вдалося відкрити файл %s для запису.\n", outputFileName);
        return 1;
    }
    
    // Шифруємо кожен символ і записуємо в файл
    for (int rowIndex = 0; rowIndex < ROW_COUNT; rowIndex++) {
        for (int pos = 0; pos < MAX_CHARS; pos++) {
            unsigned short encryptedChar = encryptChar(rows[rowIndex][pos], rowIndex, pos);
            fwrite(&encryptedChar, sizeof(encryptedChar), 1, outputFile);
        }
    }
    
    fclose(outputFile);
    
    printf("Шифрування завершено. Результат записано у файл %s\n", outputFileName);
    
    return 0;
}