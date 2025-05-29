#include <iostream>
#include <vector>

using namespace std;

const int BOARD_SIZE = 10;

// Структура для хранения информации о корабле
struct Ship {
    int size;
    vector<pair<int, int>> coordinates;

    Ship(int _size) {
        size = _size;
        coordinates.resize(size, make_pair(-1, -1));
    }
};

// Вывод игрового поля на экран
void printBoard(const vector<vector<char>>& board) {
    cout << "  ";
    for (int i = 0; i < BOARD_SIZE; i++) {
        cout << i + 1 << " ";
    }
    cout << endl;

    char row = 'A';
    for (int i = 0; i < BOARD_SIZE; i++) {
        cout << row << " ";
        for (int j = 0; j < BOARD_SIZE; j++) {
            cout << board[i][j] << " ";
        }
        cout << endl;
        row++;
    }
}

// Проверка корректности координат выстрела
bool isValidMove(int x, int y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

// Проверка попадания по кораблю
bool isHitShip(int x, int y, const vector<Ship>& ships, int& shipIndex) {
    for (int i = 0; i < ships.size(); i++) {
        for (int j = 0; j < ships[i].size; j++) {
            if (ships[i].coordinates[j].first == x && ships[i].coordinates[j].second == y) {
                shipIndex = i;
                return true;
            }
        }
    }
    return false;
}

// Проверка, был ли корабль полностью потоплен
bool isShipSunk(const vector<Ship>& ships, int shipIndex) {
    for (int i = 0; i < ships[shipIndex].size; i++) {
        if (ships[shipIndex].coordinates[i].first != -1 && ships[shipIndex].coordinates[i].second != -1) {
            return false;
        }
    }
    return true;
}

// Пометка попадания на игровом поле
void markHit(vector<vector<char>>& board, int x, int y) {
    board[x][y] = 'X';
}

// Пометка промаха на игровом поле
void markMiss(vector<vector<char>>& board, int x, int y) {
    board[x][y] = 'O';
}

// Пометка потопления корабля на игровом поле
void markSunk(vector<vector<char>>& board, const vector<Ship>& ships, int shipIndex) {
    for (int i = 0; i < ships[shipIndex].size; i++) {
        int x = ships[shipIndex].coordinates[i].first;
        int y = ships[shipIndex].coordinates[i].second;
        if (x != -1 && y != -1) {
            board[x][y] = '#';
        }
    }
}

// Проверка, остались ли непотопленные корабли
bool isGameOver(const vector<Ship>& ships) {
    for (const Ship& ship : ships) {
        if (!isShipSunk(ships, &ship - &ships[0])) {
            return false;
        }
    }
    return true;
}

int main() {
    vector<vector<char>> player1Board(BOARD_SIZE, vector<char>(BOARD_SIZE, '-'));
    vector<vector<char>> player2Board(BOARD_SIZE, vector<char>(BOARD_SIZE, '-'));

    vector<vector<char>> player1BoardWork(BOARD_SIZE, vector<char>(BOARD_SIZE, '-'));//поля для отметки попаданий на чужом поле
    vector<vector<char>> player2BoardWork(BOARD_SIZE, vector<char>(BOARD_SIZE, '-'));

    vector<Ship> player1Ships = { Ship(4), Ship(3), Ship(3), Ship(2), Ship(2), Ship(2), Ship(1), Ship(1), Ship(1), Ship(1) };
    vector<Ship> player2Ships = { Ship(4), Ship(3), Ship(3), Ship(2), Ship(2), Ship(2), Ship(1), Ship(1), Ship(1), Ship(1) };

    // Размещение кораблей на игровом поле игрока 1
    player1Ships[0].coordinates = { {0, 0}, {0, 1}, {0, 2}, {0, 3} };
    player1Ships[1].coordinates = { {2, 0}, {2, 1}, {2, 2} };
    player1Ships[2].coordinates = { {4, 0}, {4, 1}, {4, 2} };
    player1Ships[3].coordinates = { {6, 0}, {6, 1} };
    player1Ships[4].coordinates = { {7, 0}, {7, 1} };
    player1Ships[5].coordinates = { {8, 0}, {8, 1} };
    player1Ships[6].coordinates = { {9, 0} };
    player1Ships[7].coordinates = { {1, 6} };
    player1Ships[8].coordinates = { {3, 6} };
    player1Ships[9].coordinates = { {5, 6} };

    // Размещение кораблей на игровом поле игрока 2

    player2Ships[0].coordinates = { {0, 2}, {0, 3}, {0, 4}, {0, 5} };
    player2Ships[1].coordinates = { {2, 3}, {2, 4}, {2, 5} };
    player2Ships[2].coordinates = { {4, 3}, {4, 4}, {4, 5} };
    player2Ships[3].coordinates = { {6, 3}, {6, 4} };
    player2Ships[4].coordinates = { {7, 3}, {7, 4} };
    player2Ships[5].coordinates = { {8, 3}, {8, 4} };
    player2Ships[6].coordinates = { {9, 3} };
    player2Ships[7].coordinates = { {1, 9} };
    player2Ships[8].coordinates = { {3, 9} };
    player2Ships[9].coordinates = { {5, 9} };

    int currentPlayer = 1;

    while (true) {
        cout << "Ход игрока " << currentPlayer << endl;
        cout << endl;

        if (currentPlayer == 1) {
            cout << "Игровое поле игрока 1:" << endl;
            printBoard(player1Board);
        } else {
            cout << "Игровое поле игрока 2:" << endl;
            printBoard(player2Board);
        }

        int x, y;
        cout << "Введите координаты выстрела: ";
        cin >> x >> y;

        if (currentPlayer == 1) {
            int shipIndex;
            if (isValidMove(x - 1, y - 1)) {
                if (isHitShip(x - 1, y - 1, player2Ships, shipIndex)) {
                    markHit(player2Board, x - 1, y - 1);
                    if (isShipSunk(player2Ships, shipIndex)) {
                        markSunk(player2Board, player2Ships, shipIndex);
                        cout << "Корабль потоплен!" << endl;
                    } else {
                        cout << "Корабль подбит!" << endl;
                    }
                } else {
                    markMiss(player2Board, x - 1, y - 1);
                    cout << "Мимо!" << endl;
                    currentPlayer = 2;
                }
            } else {
                cout << "Некорректные координаты выстрела!" << endl;
            }
        } else if (currentPlayer == 2) {
            int shipIndex;
            if (isValidMove(x - 1, y - 1)) {
                if (isHitShip(x - 1, y - 1, player1Ships, shipIndex)) {
                    markHit(player1Board, x - 1, y - 1);
                    if (isShipSunk(player1Ships, shipIndex)) {
                        markSunk(player1Board, player1Ships, shipIndex);
                        cout << "Корабль потоплен!" << endl;
                    } else {
                        cout << "Корабль подбит!" << endl;
                    }
                } else {
                    markMiss(player1Board, x - 1, y - 1);
                    cout << "Мимо!" << endl;
                    currentPlayer = 1;
                }
            } else {
                cout << "Некорректные координаты выстрела!" << endl;
            }
        }

        if (isGameOver(player1Ships) || isGameOver(player2Ships)) {
            break;
        }

        cout << endl;
    }

    if (isGameOver(player1Ships)) {
        cout << "Игрок 2 победил!" << endl;
    } else {
        cout << "Игрок 1 победил!" << endl;
    }

    return 0;
}
