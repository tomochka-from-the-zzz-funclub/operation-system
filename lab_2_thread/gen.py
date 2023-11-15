import random

# Размер матрицы
rows, cols = 100000, 100001
n = 20

# Генерация случайных чисел для матрицы
matrix = [[random.randint(1, 5) for _ in range(cols)] for _ in range(rows)]

print(rows, cols - 1)
# Вывод матрицы
for row in matrix:
    print(" ".join(map(str, row)))

print(n)