#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <iostream>
#include <cmath>

using namespace std;

namespace binary
{
  int addition(const int &num1, const int &num2);
  int subtraction(const int &num1, const int &num2);
  int multiply(const int &num1, const int &num2);
  int segmentation(const int &num1, const int &num2);
}


int binary::addition(const int &num1, const int &num2) { return num1 + num2; }
int binary::subtraction(const int &num1, const int &num2) { return num1 - num2; }

int binary::multiply(const int &num1, const int &num2)
{
  if (num1 == 0 || num2 == 0)
    return 0;

  int result = 0;
  bool isNegative = ((num1 < 0) ^ (num2 < 0));

  int abs_num1 = abs(num1);
  int abs_num2 = abs(num2);

  while (abs_num2 != 0)
  {
    if (abs_num2 & 1)
      result = addition(result, abs_num1);

    abs_num1 <<= 1;
    abs_num2 >>= 1;
  }

  return !isNegative ? result : -result;
}

int binary::segmentation(const int &num1, const int &num2)
{
  if (num1 == 0)
    return 0;

  if (num2 == 0)
  {
    cout << "Division by zero is not allowed." << endl;
    return 0;
  }

  int result = 0;
  bool isNegative = ((num1 < 0) ^ (num2 < 0));

  int abs_num1 = abs(num1);
  int abs_num2 = abs(num2);

  while (abs_num1 >= abs_num2)
  {
    int temp = abs_num2;
    int mult = 1;

    while ((temp << 1) <= abs_num1)
    {
      temp <<= 1;
      mult <<= 1;
    }

    abs_num1 -= temp;
    result += mult;
  }

  return !isNegative ? result : -result;
}

using namespace binary;

int calculate_expression(int a, int b, int c, int d) {
    // Множення на 15 як (16-1)*a = (a << 4) - a
    int term1 = multiply(a, 15);

    // Множення на 312 як (a << 5)
    int term2 = multiply(b, 312);

    // Додавання перших двох доданків
    int sum_ab = term1 + term2;

    // Ділення на 64 як зсув вправо на 6 позицій
    int div_ab = segmentation(sum_ab, 64);

    // Множення на 120 як (128 - 8)*c = (c << 7) - (c << 3)
    int term_c = multiply(c, 120);

    // Множення на 121 як (128 - 8 + 1)*d = (d << 7) - (d << 3) + d
    int term_d = multiply(d, 121);

    // Кінцеве обчислення виразу
    return div_ab - term_c + term_d;
}

void task_01() {
    int a, b, c, d;
        
    cout << "input";
    cin >> a >> b >> c >> d;
    
    // Обчислення результату
    int result = calculate_expression(a, b, c, d);
    
    // Виведення результату
    printf("%d\n", result);
}

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
        }

        fclose(outfile);
        return 1;
    }

    // Виведення зашифрованих символів у файл
    int printEncryptedSymbols(const char* filename) {
        FILE* outfile = fopen(filename, "a");

        fprintf(outfile, "Зашифровані символи:\n");
        for (int i = 0; i < symbolCount; ++i) {
            fprintf(outfile, "Symbol %d: %x\n", i, encryptedSymbols[i]);
        }

        fclose(outfile);
        return 1;
    }
};

void task_02() {
    TextEncryptor encryptor;

    // Шифрування тексту
    encryptor.encrypt();

    // Виведення зашифрованих символів у файл
    if (!encryptor.printEncryptedSymbols("decrypted.txt")) {
        printf("Помилка виведення зашифрованих символів\n");
    }

    // Декодування символів та запис у файл
    if (!encryptor.decode("decrypted.txt")) {
        printf("Помилка декодування\n");
    }

    printf("Операція завершена успішно.\n");
}

void MenuTask() {
    cout << "     Menu Task   \n";
    cout << "    1.  Calculation of expressions using bitwise operations  \n";
    cout << "    2.  Data encryption using bitwise operations \n";
    cout << "    3.  Data encryption using structures with bit fields \n";

    cout << "    q.  Quit \n";
}

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

void task_02_1() {
    const char* inputFileName = "outb.bin";
    const char* outputFileName = "decrypted.txt";
    
    
    // Відкриваємо бінарний файл для читання
    FILE* inputFile = fopen(inputFileName, "rb");
    if (inputFile == NULL) {
        fprintf(stderr, "Помилка: Не вдалося відкрити файл %s для читання.\n", inputFileName);
    }
    
    // Визначаємо розмір файлу
    long fileSize = getFileSize(inputFile);
    
    // Перевіряємо, чи файл містить повні двобайтові значення
    if (fileSize % 2 != 0) {
        fprintf(stderr, "Помилка: Неправильний формат файлу. Розмір повинен бути кратним 2 байтам.\n");
        fclose(inputFile);
    }
    
    // Обчислюємо кількість зашифрованих символів
    size_t encryptedSize = fileSize / 2;
    
    // Створюємо масив для зашифрованих символів
    unsigned short* encryptedData = (unsigned short*)malloc(fileSize);
    if (encryptedData == NULL) {
        fprintf(stderr, "Помилка: Не вдалося виділити пам'ять.\n");
        fclose(inputFile);
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
}

void chooseTask(){

    char ch;
    
    while (true)
    {
        MenuTask();

        cin >> ch;

        switch (ch)
        {
        case '1':
            task_01();
            break;
        case '2':
            task_02();
            break;
        case '3':
            task_02_1();
            break;
        case 'q':
            return;
            
        default:
            break;
        }
    }
}



int main(int argc, char const *argv[]){
    chooseTask();
    return 0;
}
