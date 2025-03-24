#include <cstdio>
#include <cstdlib>
#include <cstring>

// Функція для перевірки біта парності
bool checkParity(unsigned short value) {
    // Підраховуємо кількість одиниць у числі
    int count = 0;
    unsigned short temp = value;
    while (temp) {
        count += temp & 1;
        temp >>= 1;
    }
    
    // Перевіряємо, чи кількість одиниць непарна
    return (count % 2 == 1);
}

// Функція для розшифрування символу
char decryptChar(unsigned short encryptedChar, int& row, int& position) {
    // Перевіряємо біт парності
    if (!checkParity(encryptedChar)) {
        fprintf(stderr, "Помилка: Порушення біта парності!\n");
    }
    
    // Отримуємо номер рядка (біти 0-1)
    row = encryptedChar & 0x3;
    
    // Отримуємо позицію символу в рядку (біти 2-6)
    position = (encryptedChar >> 2) & 0x1F;
    
    // Отримуємо ASCII-код символу (біти 7-14)
    char asciiChar = (encryptedChar >> 7) & 0xFF;
    
    return asciiChar;
}

// Функція для отримання розміру файлу
long getFileSize(FILE* file) {
    long size;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

int main(int argc, char* argv[]) {
    const char* inputFileName = "outb.bin";
    const char* outputFileName = "decrypted.txt";
    
    // Перевірка аргументів командного рядка
    if (argc > 1) {
        inputFileName = argv[1];
    }
    if (argc > 2) {
        outputFileName = argv[2];
    }
    
    // Відкриваємо бінарний файл для читання
    FILE* inputFile = fopen(inputFileName, "rb");
    if (inputFile == NULL) {
        fprintf(stderr, "Помилка: Не вдалося відкрити файл %s для читання.\n", inputFileName);
        return 1;
    }
    
    // Визначаємо розмір файлу
    long fileSize = getFileSize(inputFile);
    
    // Перевіряємо, чи файл містить повні двобайтові значення
    if (fileSize % 2 != 0) {
        fprintf(stderr, "Помилка: Неправильний формат файлу. Розмір повинен бути кратним 2 байтам.\n");
        fclose(inputFile);
        return 1;
    }
    
    // Обчислюємо кількість зашифрованих символів
    size_t encryptedSize = fileSize / 2;
    
    // Створюємо масив для зашифрованих символів
    unsigned short* encryptedData = (unsigned short*)malloc(fileSize);
    if (encryptedData == NULL) {
        fprintf(stderr, "Помилка: Не вдалося виділити пам'ять.\n");
        fclose(inputFile);
        return 1;
    }
    
    // Читаємо всі зашифровані символи
    size_t readElements = fread(encryptedData, sizeof(unsigned short), encryptedSize, inputFile);
    fclose(inputFile);
    
    if (readElements != encryptedSize) {
        fprintf(stderr, "Попередження: Прочитано %zu елементів із %zu очікуваних.\n", readElements, encryptedSize);
    }
    
    // Відкриваємо текстовий файл для запису розшифрованих даних
    FILE* outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
        fprintf(stderr, "Помилка: Не вдалося відкрити файл %s для запису.\n", outputFileName);
        free(encryptedData);
        return 1;
    }
    
    // Матриця для розшифрованих символів (4 рядки по 32 символи)
    const int ROW_COUNT = 4;
    const int MAX_CHARS = 32;
    char decryptedRows[ROW_COUNT][MAX_CHARS + 1]; // +1 для нуль-термінатора
    
    // Ініціалізуємо масив пробілами
    for (int i = 0; i < ROW_COUNT; i++) {
        memset(decryptedRows[i], ' ', MAX_CHARS);
        decryptedRows[i][MAX_CHARS] = '\0'; // Додаємо нуль-термінатор
    }
    
    // Розшифровуємо кожен символ і розміщуємо у відповідному рядку та позиції
    for (size_t i = 0; i < readElements; i++) {
        int row, position;
        char decryptedChar = decryptChar(encryptedData[i], row, position);
        
        // Перевіряємо чи індекси входять в допустимий діапазон
        if (row >= 0 && row < ROW_COUNT && position >= 0 && position < MAX_CHARS) {
            decryptedRows[row][position] = decryptedChar;
        } else {
            fprintf(stderr, "Помилка: Некоректні індекси рядка або позиції (%d, %d).\n", row, position);
        }
    }
    
    // Звільняємо пам'ять
    free(encryptedData);
    
    // Виводимо розшифровані дані в консоль і записуємо у файл
    printf("Розшифровані дані:\n");
    for (int i = 0; i < ROW_COUNT; i++) {
        printf("Рядок %d: \"%s\"\n", (i + 1), decryptedRows[i]);
        fprintf(outputFile, "%s\n", decryptedRows[i]);
    }
    
    fclose(outputFile);
    
    printf("Розшифрування завершено. Результат записано у файл %s\n", outputFileName);
    
    return 0;
}