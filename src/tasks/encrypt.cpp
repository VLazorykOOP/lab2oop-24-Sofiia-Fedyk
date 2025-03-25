#include <stdio.h>
#include <iostream>

// Структура для зберігання закодованого символу
union SymbolEncryption {
struct {
    unsigned short line_number : 2;     
    unsigned short symbol_position : 5; 
    unsigned short ascii_code : 8;      
    unsigned short parity_bit : 1;      
} bits;
unsigned short full_value;  
};

class TextEncryptor {
private:
char lines[4][33];
unsigned short encryptedSymbols[4 * 32];
int symbolCount;

// Копіювання рядка з доповненням пробілами
void copyString(char* dest, const char* src, int maxLen) {
    int i = 0;
    while (src[i] && i < maxLen - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';

    // Доповнення пробілами до 32 символів
    while (i < maxLen) {
        dest[i++] = ' ';
    }
}

// Обчислення біту парності
unsigned short calculateParityBit(unsigned short value) {
    unsigned short count = 0;
    for (int i = 0; i < 15; i++) {
        if (value & (1 << i)) {
            count++;
        }
    }
    return count % 2;
}

public:
TextEncryptor() : symbolCount(0) {
    // Ініціалізація порожніми рядками
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 33; j++) {
            lines[i][j] = '\0';
        }
    }
}

// Читання рядків з файлу
int readInputFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Помилка відкриття файлу %s\n", filename);
        return 0;
    }

    // Читання 4 рядків
    for (int i = 0; i < 4; i++) {
        char buffer[100] = {0};
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            fclose(file);
            return 0;
        }
        
        // Видалення символу переведення рядка
        int len = 0;
        while (buffer[len] != '\0' && buffer[len] != '\n') {
            len++;
        }
        buffer[len] = '\0';

        // Копіювання з доповненням пробілами
        copyString(lines[i], buffer, 33);
    }

    fclose(file);
    return 1;
}

// Шифрування тексту
void encrypt() {
    symbolCount = 0;
    
    for (int line_index = 0; line_index < 4; ++line_index) {
        for (int pos = 0; pos < 32; ++pos) {
            SymbolEncryption symbol;
            
            // Заповнення бітових полів
            symbol.bits.line_number = line_index;
            symbol.bits.symbol_position = pos;
            symbol.bits.ascii_code = lines[line_index][pos];
            
            // Обчислення біту парності
            symbol.bits.parity_bit = calculateParityBit(symbol.full_value >> 1);

            encryptedSymbols[symbolCount++] = symbol.full_value;
        }
    }
}

// Декодування символів та запис у файл
int decode(const char* filename) {
    FILE* outfile = fopen(filename, "w");
    if (!outfile) {
        printf("Помилка відкриття файлу %s\n", filename);
        return 0;
    }

    // Підготовка масиву для декодованих рядків
    char decodedLines[4][33];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 33; j++) {
            decodedLines[i][j] = '\0';
        }
    }

    // Лічильники для кожного рядка
    int lineLengths[4] = {0};

    // Декодування символів
    for (int i = 0; i < symbolCount; ++i) {
        SymbolEncryption symbol;
        symbol.full_value = encryptedSymbols[i];

        // Перевірка біту парності
        unsigned short calculatedParity = calculateParityBit(encryptedSymbols[i] >> 1);
        if (calculatedParity != symbol.bits.parity_bit) {
            fprintf(outfile, "Помилка парності!\n");
            continue;
        }

        // Додавання символу до відповідного рядка
        int lineIndex = symbol.bits.line_number;
        int len = lineLengths[lineIndex];
        decodedLines[lineIndex][len] = symbol.bits.ascii_code;
        lineLengths[lineIndex]++;
    }

    // Запис декодованих рядків у файл
    for (int i = 0; i < 4; i++) {
        fprintf(outfile, "%s\n", decodedLines[i]);
        std::cout << decodedLines[i];
    }

    fclose(outfile);
    return 1;
}

// Виведення зашифрованих символів у файл
int printEncryptedSymbols(const char* filename) {
    FILE* outfile = fopen(filename, "a");
    if (!outfile) {
        printf("Помилка відкриття файлу %s\n", filename);
        return 0;
    }

    fprintf(outfile, "Зашифровані символи:\n");
    for (int i = 0; i < symbolCount; ++i) {
        fprintf(outfile, "Symbol %d: %x\n", i, encryptedSymbols[i]);
    }

    fclose(outfile);
    return 1;
}
};

int main() {
TextEncryptor encryptor;

// Читання вхідних рядків з файлу
if (!encryptor.readInputFile("in.txt")) {
    printf("Помилка читання вхідного файлу\n");
    return 1;
}

// Шифрування тексту
encryptor.encrypt();

// Виведення зашифрованих символів у файл
if (!encryptor.printEncryptedSymbols("decrypted.txt")) {
    printf("Помилка виведення зашифрованих символів\n");
    return 1;
}

// Декодування символів та запис у файл
if (!encryptor.decode("decrypted.txt")) {
    printf("Помилка декодування\n");
    return 1;
}

printf("Операція завершена успішно.\n");
return 0;
}