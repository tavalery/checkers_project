#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // Для std::transform

// version 4

// Чтобы юникод читался 
#ifdef _WIN32
#include <windows.h>
#endif

// Определения символов для отображения фигур через макросы
#define WHITE_CHECKER 'W'
#define BLACK_CHECKER 'B'

// Forward declarations
class Board;

// Базовый класс для фигур
class Piece {
public:
    enum Color {
        WHITE,
        BLACK
    };

    Color color;

    Piece(Color color) : color(color) {}

    // virtual ~Piece() {}

    bool isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const;
    char getSymbol() const { return (color == WHITE) ? WHITE_CHECKER : BLACK_CHECKER;};
};




// Класс доски
class Board {
public:
    Piece* board[8][8]; // Массив указателей на Piece

    Board() {
        // Инициализация доски (пустой)
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                board[i][j] = nullptr;
            }
        }
    }

    ~Board() {
        // Освобождение памяти
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (board[i][j] != nullptr) {
                    delete board[i][j];
                    board[i][j] = nullptr;
                }
            }
        }
    }

    void setupBoard(); // Устанавливает начальное положение фигур
    void printBoard() const; // Выводит доску в консоль

    Piece* getPiece(int row, int col) const { return board[row][col]; }
    void setPiece(int row, int col, Piece* piece) { board[row][col] = piece; }

    bool isLegalMove(int startRow, int startCol, int endRow, int endCol, Piece::Color currentPlayer) const;
    bool makeMove(int startRow, int startCol, int endRow, int endCol);
    void undoMove(int startRow, int startCol, int endRow, int endCol, Piece* capturedPiece);
private:
    bool isValidCoordinates(int row, int col) const{
        return (row >= 0 && row < 8 && col >= 0 && col < 8);
    }
};

bool Piece::isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const {
    // 0. Проверка на выход за пределы доски
    if (endRow < 0 || endRow > 7 || endCol < 0 || endCol > 7) {
        return false;
    }

    // 1. Движение вперед по диагонали на одну клетку
    if (std::abs(endRow - startRow) == 1 && std::abs(endCol - startCol) == 1) {
        // Проверяем, что клетка свободна
        if (board.getPiece(endRow, endCol) == nullptr) {
            return true; // Обычный ход
        }
    }

    // 2. Взятие фигуры противника (прыжок через фигуру)
    if (std::abs(endRow - startRow) == 2 && std::abs(endCol - startCol) == 2) {
        int jumpedRow = (startRow + endRow) / 2;
        int jumpedCol = (endCol > startCol) ? startCol + 1 : startCol - 1;
        Piece* jumpedPiece = board.getPiece(jumpedRow, jumpedCol);

        // Проверяем, что есть фигура противника и что конечная клетка свободна
        if (jumpedPiece != nullptr && jumpedPiece->color != color && board.getPiece(endRow, endCol) == nullptr) {
            return true; // Взятие
        }
    }

    return false; // Ход не допустим
}

// Класс игры
class Game {
public:
    Board board;
    Piece::Color currentPlayer;

    Game() : currentPlayer(Piece::WHITE) {}

    void startNewGame();
    void playGame();
private:
    std::string getUserInput();
    bool processMove(const std::string& move);
    void switchPlayer();

    // Преобразует шахматную нотацию ("a2") в координаты (row, col)
    std::pair<int, int> algebraicToCoord(const std::string& square) const;

    // Преобразует координаты (row, col) в шахматную нотацию ("a2")
    std::string coordToAlgebraic(int row, int col) const;
};

// Реализация методов классов

// ----------------------- Board -----------------------

void Board::setupBoard() {
    // Расстановка белых шашек
    for (int i = 0; i < 3; ++i) {
        for (int j = (i % 2 == 0) ? 1 : 0; j < 8; j += 2) {
            board[i][j] = new Piece(Piece::WHITE);
        }
    }

    // Расстановка черных шашек
    for (int i = 5; i < 8; ++i) {
        for (int j = (i % 2 == 0) ? 1 : 0; j < 8; j += 2) {
            board[i][j] = new Piece(Piece::BLACK);
        }
    }
}   

void Board::printBoard() const {
    std::string hyphens;
        for (int i = 0; i < 50; ++i) {
            hyphens += "=";
          }
    std::cout << hyphens << std::endl;// Разделитель вывода между ходов
    std::cout << "   "; // Отступ для верхних букв
    for (char c = 'a'; c <= 'h'; ++c) {
        std::cout << " " << c << "  ";
    }
    std::cout << std::endl;

    std::cout << "  +" << std::string(31, '-') << "+" << std::endl; // Верхняя граница

    for (int row = 7; row >= 0; --row) {
        std::cout << row + 1 << " |";
        for (int col = 0; col < 8; ++col) {
            Piece* piece = board[row][col];
            if (piece) {
                std::cout << " " << piece->getSymbol() << " |";
            } else {
                std::cout << "   |"; // Три пробела, чтобы клетка оставалась квадратной
            }
        }
        std::cout << " " << row + 1 << std::endl;
        std::cout << "  +" << std::string(31, '-') << "+" << std::endl;
    }
    std::cout << "   "; // Отступ для нижних букв
    for (char c = 'a'; c <= 'h'; ++c) {
        std::cout << " " << c << "  ";
    }
    std::cout << std::endl;
}

bool Board::isLegalMove(int startRow, int startCol, int endRow, int endCol, Piece::Color currentPlayer) const {
    // 0. Проверка на выход за пределы доски
    if (!isValidCoordinates(startRow, startCol) || !isValidCoordinates(endRow, endCol)) {
        return false;
    }

    // 1. Проверка, что клетка начала хода содержит фигуру текущего игрока
    Piece* piece = getPiece(startRow, startCol);
    if (!piece || piece->color != currentPlayer) {
        return false;
    }

    // 2. Проверка, что ход допустим для данной фигуры (вызов isValidMove)
    if (!piece->isValidMove(startRow, startCol, endRow, endCol, *this)) {
        return false;
    }
     
    return true;
}

bool Board::makeMove(int startRow, int startCol, int endRow, int endCol) {
    // 1. Получить указатель на фигуру, которую перемещаем
    Piece* pieceToMove = board[startRow][startCol];

    // 2. Определяем, был ли прыжок (взятие)
    int rowDiff = std::abs(endRow - startRow);
    int colDiff = std::abs(endCol - startCol);

    if (rowDiff == 2 && colDiff == 2) {
        // Взяттие шашки
        int jumpedRow = (startRow + endRow) / 2;
        int jumpedCol = (startCol + endCol) / 2;
        Piece* jumpedPiece = board[jumpedRow][jumpedCol];

        if (jumpedPiece != nullptr) {
            // Удаляем взятую шашку
            delete jumpedPiece;
            board[jumpedRow][jumpedCol] = nullptr;
        }
    }

    // 3. Переместить фигуру
    board[endRow][endCol] = pieceToMove;
    board[startRow][startCol] = nullptr; // Освободить клетку, откуда переместили

    return true; // Ход выполнен успешно
}

void Board::undoMove(int startRow, int startCol, int endRow, int endCol, Piece* capturedPiece) {
    // TODO: откат хода?
}

// ----------------------- Game -----------------------

void Game::startNewGame() {
    board.setupBoard();
    currentPlayer = (Piece::Color)0;
}

void Game::playGame() {
    startNewGame();
    while (true) {
        board.printBoard();
        std::cout << (currentPlayer == Piece::WHITE ? "Ход белых" : "Ход черных") << std::endl;
        std::string move = getUserInput();

        // Прерывание игры вручную
        if (move == "exit") {
            std::cout << "Игра прервана." << std::endl;
            break;
        }

        if (processMove(move)) {
            switchPlayer();
        } else {
            std::cout << "Неверный ход!" << std::endl;
        }
    }
}

std::string Game::getUserInput() {
    std::cout << "Введите ход (a2 b3) или 'exit' для выхода: ";
    std::string input;
    std::getline(std::cin, input);

    // Преобразование ввода в нижний регистр
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);

    return input;
}

bool Game::processMove(const std::string& move) {
    // 1. Проверка формата ввода (должно быть два поля, разделенных пробелом)
    size_t spacePos = move.find(' ');
    if (spacePos == std::string::npos) {
        return false;
    }

    std::string startSquare = move.substr(0, spacePos);
    std::string endSquare = move.substr(spacePos + 1);

    // 2. Преобразование шахматной нотации в координаты
    std::pair<int, int> startCoord = algebraicToCoord(startSquare);
    std::pair<int, int> endCoord = algebraicToCoord(endSquare);

    int startRow = startCoord.first;
    int startCol = startCoord.second;
    int endRow = endCoord.first;
    int endCol = endCoord.second;

    // 3. Проверка корректности координат
    if (startRow == -1 || endRow == -1) {
        return false; // Некорректный формат координат
    }

    // 4. Проверка легальности хода
    if (!board.isLegalMove(startRow, startCol, endRow, endCol, currentPlayer)) {
        return false;
    }

    // 5. Совершение хода
    board.makeMove(startRow, startCol, endRow, endCol);
    return true;
}

void Game::switchPlayer() {
    currentPlayer = (currentPlayer == Piece::WHITE) ? Piece::BLACK : Piece::WHITE;
}

// Преобразует шахматную нотацию ("a2") в координаты (row, col)
std::pair<int, int> Game::algebraicToCoord(const std::string& square) const {
    if (square.length() != 2) {
        return {-1, -1}; // Неверный формат
    }

    char file = square[0]; // 'a' - 'h'
    char rank = square[1]; // '1' - '8'

    int col = file - 'a';
    int row = rank - '1';

    if (col < 0 || col > 7 || row < 0 || row > 7) {
        return {-1, -1}; // Выход за пределы доски
    }

    return {row, col};
}

// Преобразует координаты (row, col) в шахматную нотацию ("a2")
std::string Game::coordToAlgebraic(int row, int col) const {
    char file = 'a' + col;
    char rank = '1' + row;
    return std::string{file, rank};
}


int main() {
    // Set console output to UTF-8 (for Windows)
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    Game game;
    game.playGame();

    return 0;
}
