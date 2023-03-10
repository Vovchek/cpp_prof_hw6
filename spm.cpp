/*! \mainpage Homework #6 Syllabus.

Спроектировать 2-мерную разреженную бесконечную матрицу, заполненную значениями по умолчанию. 
Матрица должна хранить только занятые элементы - значения которых хотя бы раз присваивались. 
Присвоение в ячейку значения по умолчанию освобождает ячейку.
Необходимо уметь отвечать на вопрос - сколько ячеек реально занято?
Необходимо уметь проходить по всем занятым ячейкам. Порядок не имеет значения. 
Возвращается позиция ячейки и ее значение.
При чтении элемента из свободной ячейки возвращать значение по умолчанию.

Пример: 

// бесконечная матрица int заполнена значениями -1 
Matrix<int, -1> matrix; 
assert(matrix.size() == 0); // все ячейки свободны 

auto a = matrix[0][0]; 
assert(a == -1); 
assert(matrix.size() == 0); 

matrix[100][100] = 314; 
assert(matrix[100][100] == 314); 
assert(matrix.size() == 1); 

// выведется одна строка 
// 100100314 
for(auto c: matrix) { 
    int x; 
    int y; 
    int v; 
    std::tie(x, y, v) = c; 
    std::cout << x << y << v << std::endl; 
}

При запуске программы необходимо создать матрицу с пустым значением 0, 
заполнить главную диагональ матрицы (от [0,0] до [9,9]) значениями от 0 до 9.
Второстепенную диагональ (от [0,9] до [9,0]) значениями от 9 до 0.
Необходимо вывести фрагмент матрицы от [1,1] до [8,8]. 
Между столбцами пробел. Каждая строка матрицы на новой строке консоли.
Вывести количество занятых ячеек.
Вывести все занятые ячейки вместе со своими позициями.
* Опционально реализовать N-мерную матрицу.
* Опционально реализовать каноническую форму оператора `=`, допускающую выражения 

`((matrix[100][100] = 314) = 0) = 217`

Самоконтроль
- индексация оператором `[]`
- количество занятых ячеек должно быть 18 после выполнения примера выше

Проверка
 **Задание считается выполненным успешно, если после анализа кода, установки пакета и запуска приложения появился фрагмент матрицы, количество ячеек и список всех значений с позициями.**

*/

#include <iostream>
#include "sparse_matrix.h"


int main()
{
// create matrix with (int)0 default value
    SparseMatrix<int, 0> sm;

// fill main diagonal 0 to 9 with 0 to 9 values
// fill secondary diagonal with 9 to 0 values
for(int i = 0; i <= 9; ++i) {
    sm[i][i] = i;
    sm[i][9 - i] = 9 - i;
}

// output [1,1] .. [8,8] submatrix
for(int i = 1; i <= 8; ++i) {
    for(int j = 1; j <= 8; ++j) {
        if(j > 1)
            std::cout << " ";
        std::cout << sm[i][j];
    }
    std::cout << std::endl;
}

// output number of filled cells
std::cout << sm.size() << std::endl;

// output all filled cells and their positions
for(auto c : sm) {
    std::cout << c.i << c.j << c.v << std::endl; 
}

// * optionally implement N-dimensioned matrix

/// unfortunately, it probably requires too much effort to
/// increase the number of dimensions using current implementation
/// of a sparse vector/matrix as a hierarchy of enclosed maps.
/// I think this is a deadlock - i need to rewrite the code completely.

    return 0;
}