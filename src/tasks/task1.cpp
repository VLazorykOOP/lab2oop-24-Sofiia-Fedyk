#include <cstdio>
#include <iostream>

// #include "../task.h"

using namespace std;

int calculate_expression(int a, int b, int c, int d) {
    // Множення на 15 як (16-1)*a = (a << 4) - a
    int term1 = (a << 4) - a;

    // Множення на 32 як (a << 5)
    int term2 = b << 5;

    // Додавання перших двох доданків
    int sum_ab = term1 + term2;

    // Ділення на 64 як зсув вправо на 6 позицій
    int div_ab = sum_ab >> 6;

    // Множення на 120 як (128 - 8)*c = (c << 7) - (c << 3)
    int term_c = (c << 7) - (c << 3);

    // Множення на 121 як (128 - 8 + 1)*d = (d << 7) - (d << 3) + d
    int term_d = (d << 7) - (d << 3) + d;

    // Кінцеве обчислення виразу
    return div_ab - term_c + term_d;
}

void task_01() {
    int a, b, c, d;
    
    // Введення змінних за допомогою scanf
    // if (scanf("%d %d %d %d", &a, &b, &c, &d) != 4) {
        
    cin >> a >> b >> c >> d;
    // Обробка помилки введення
        // printf("Помилка введення даних\n");
        // return 1;
    // }
    
    // Обчислення результату
    int result = calculate_expression(a, b, c, d);
    
    // Виведення результату
    printf("%d\n", result);
}