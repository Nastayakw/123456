#include <iostream>
#include <concepts>
#include <cstring>
#include <locale>

// 2.1 Концепт для ограничения допустимых типов
template<typename T>
concept ValidType = requires(T a, T b, int n) {
    { a + b } -> std::same_as<T>;
    { a * b } -> std::same_as<T>;
    { a * n } -> std::same_as<T>;
    requires sizeof(T) <= 1000;
};

// 1. Шаблонный класс динамического вектора
template<ValidType T>
class DynamicVector {
private:
    T* data;
    size_t capacity;
    size_t currentSize;

    void resizeCapacity(size_t newCapacity) {
        T* newData = new T[newCapacity];
        for(size_t i = 0; i < currentSize; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    // Конструкторы
    DynamicVector() : data(nullptr), capacity(0), currentSize(0) {}
    
    explicit DynamicVector(size_t initialSize) : capacity(initialSize), currentSize(initialSize) {
        data = new T[capacity]{};
    }

    // Деструктор
    ~DynamicVector() {
        delete[] data;
    }

    // 1.1 Добавление элемента в конец
    void push_back(const T& value) {
        if(currentSize >= capacity) {
            resizeCapacity(capacity == 0 ? 1 : capacity * 2);
        }
        data[currentSize++] = value;
    }

    // 1.2 Удаление элемента по индексу
    void erase(size_t index) {
        if(index >= currentSize) return;
        
        for(size_t i = index; i < currentSize - 1; ++i) {
            data[i] = data[i + 1];
        }
        --currentSize;
    }

    // 1.3 Получение текущего размера
    size_t size() const {
        return currentSize;
    }

    // 1.4 Изменение размера
    void resize(size_t newSize) {
        if(newSize > capacity) {
            resizeCapacity(newSize);
        }
        currentSize = newSize;
    }

    // 1.6 Поиск элемента
    int find(const T& value) const {
        for(size_t i = 0; i < currentSize; ++i) {
            if(data[i] == value) return i;
        }
        return -1;
    }

    // 1.7 Оператор сложения
    DynamicVector operator+(const DynamicVector& other) const {
        DynamicVector result(currentSize);
        for(size_t i = 0; i < currentSize; ++i) {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    // 1.7 Оператор умножения
    DynamicVector operator*(const DynamicVector& other) const {
        DynamicVector result(currentSize);
        for(size_t i = 0; i < currentSize; ++i) {
            result.data[i] = data[i] * other.data[i];
        }
        return result;
    }

    // Доступ к элементам
    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

    // 2.3 Добавление списка элементов с умножением на их сумму
    template<typename... Args>
    void add_list(const Args&... args) {
        T sum = (args + ...);
        (push_back(args * sum), ...);
    }
};

// 1.5 Перегрузка вывода
template<ValidType T>
std::ostream& operator<<(std::ostream& os, const DynamicVector<T>& vec) {
    os << "[";
    for(size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
        if(i < vec.size() - 1) os << ", ";
    }
    os << "]";
    return os;
}

// 2.2 Класс матрицы для тестирования
class Matrix {
private:
    int mat[2][2];
public:
    Matrix(int a = 0, int b = 0, int c = 0, int d = 0) {
        mat[0][0] = a; mat[0][1] = b;
        mat[1][0] = c; mat[1][1] = d;
    }

    Matrix operator+(const Matrix& other) const {
        return Matrix(
            mat[0][0] + other.mat[0][0],
            mat[0][1] + other.mat[0][1],
            mat[1][0] + other.mat[1][0],
            mat[1][1] + other.mat[1][1]
        );
    }

    Matrix operator*(const Matrix& other) const {
        return Matrix(
            mat[0][0]*other.mat[0][0] + mat[0][1]*other.mat[1][0],
            mat[0][0]*other.mat[0][1] + mat[0][1]*other.mat[1][1],
            mat[1][0]*other.mat[0][0] + mat[1][1]*other.mat[1][0],
            mat[1][0]*other.mat[0][1] + mat[1][1]*other.mat[1][1]
        );
}

    Matrix operator*(int n) const {
        return Matrix(
            mat[0][0]*n, mat[0][1]*n,
            mat[1][0]*n, mat[1][1]*n
        );
    }

    bool operator==(const Matrix& other) const {
        return memcmp(mat, other.mat, sizeof(mat)) == 0;
    }

    friend std::ostream& operator<<(std::ostream& os, const Matrix& m) {
        os << "[[" << m.mat[0][0] << "," << m.mat[0][1] << "],"
           << "[" << m.mat[1][0] << "," << m.mat[1][1] << "]]";
        return os;
    }
};

// Демонстрация работы
int main() {
    // Установка локализации для русского языка
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());
    
    std::cout << "=== ТЕСТИРОВАНИЕ ДИНАМИЧЕСКОГО ВЕКТОРА ===" << std::endl;
    
    // Тестирование с целыми числами
    DynamicVector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    
    std::cout << "\n1. Тестирование с целыми числами:" << std::endl;
    std::cout << "Исходный вектор: " << vec << std::endl;
    
    vec.erase(1);
    std::cout << "После удаления элемента с индексом 1: " << vec << std::endl;
    
    std::cout << "Текущий размер вектора: " << vec.size() << std::endl;
    std::cout << "Поиск элемента 3: индекс = " << vec.find(3) << std::endl;
    std::cout << "Поиск элемента 5: индекс = " << vec.find(5) << std::endl;
    
    // Тестирование операций
    DynamicVector<int> vec1, vec2;
    vec1.push_back(1); vec1.push_back(2);
    vec2.push_back(3); vec2.push_back(4);
    
    auto sumVec = vec1 + vec2;
    auto mulVec = vec1 * vec2;
    
    std::cout << "\nОперации с векторами:" << std::endl;
    std::cout << "Вектор 1: " << vec1 << std::endl;
    std::cout << "Вектор 2: " << vec2 << std::endl;
    std::cout << "Сложение: " << sumVec << std::endl;
    std::cout << "Умножение: " << mulVec << std::endl;
    
    // Тестирование с матрицами
    std::cout << "\n2. Тестирование с матрицами:" << std::endl;
    DynamicVector<Matrix> matVec;
    matVec.push_back(Matrix(1,2,3,4));
    matVec.push_back(Matrix(2,3,4,5));
    
    std::cout << "Вектор матриц: " << matVec << std::endl;
    
    // Тестирование метода add_list
    std::cout << "\n3. Тестирование метода add_list:" << std::endl;
    DynamicVector<int> vec3;
    vec3.push_back(10);
    std::cout << "Вектор до add_list: " << vec3 << std::endl;
    
    vec3.add_list(1, 2, 3);
    std::cout << "Вектор после add_list(1, 2, 3): " << vec3 << std::endl;
    
    // Объяснение результата
    std::cout << "\nПояснение: sum = 1 + 2 + 3 = 6" << std::endl;
    std::cout << "Элементы добавляются как: 1*6=6, 2*6=12, 3*6=18" << std::endl;
    
    // Тестирование изменения размера
    std::cout << "\n4. Тестирование изменения размера:" << std::endl;
    DynamicVector<int> vec4;
    for(int i = 0; i < 5; ++i) {
        vec4.push_back(i*10);
    }
    std::cout << "Вектор из 5 элементов: " << vec4 << std::endl;
    
    vec4.resize(3);
    std::cout << "После уменьшения размера до 3: " << vec4 << std::endl;
    
    vec4.resize(6);
    std::cout << "После увеличения размера до 6: " << vec4 << std::endl;
    
    std::cout << "\n=== ТЕСТИРОВАНИЕ ЗАВЕРШЕНО ===" << std::endl;

    return 0;
}
