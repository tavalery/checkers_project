// !!! Код содержит тучу ошибок, противоречий и т.д.
// Оставляю для будущих поколений, если они заходят 
// довести до ума пой первоначальный вариант.
// В отличие от шашек, тут уже отдельно структура 
// под фигуры и отдельно подструктуры для каждой из них.
// Возможно, можно прогнать через нейронку и т.д., 
// чтобы стало пологичнее, но это не точно. 

// Рекомендации от преподавателя и знакомых:
// 1. Доска всегда имеет один и тот же размер, 
//    мы меняем только ее содержимое, поэтому 
//    можно хранить в ином формате. Smart указатели?
// 2. НАДО! ПРОСТО НЕОБХОДИМО! разбить на несколько файлов. 
//    И это не только для общей читаемости
// 3. Мб оптимизировать присваивание символов каждой фигуры?
// 4. Можно, даже нужно использовать код для шашек. 
//    Там, как на более простом случае, у меня получилось улучшить
//    вывод и несколько нюансов в функциях. 

// На будущее:
// Счетчик суток, потраченных на это: 3
// Удачи :)


#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // Для std::transform

// UTF-8 символы для шахматных фигур через макросы
#define W_KING  U'\u2654'
#define W_QUEEN U'\u2655'
#define W_ROOK  U'\u2656'
#define W_BISHOP U'\u2657'
#define W_KNIGHT U'\u2658'
#define W_PAWN   U'\u2659'
#define B_KING  U'\u265A'
#define B_QUEEN U'\u265B'
#define B_ROOK  U'\u265C'
#define B_BISHOP U'\u265D'
#define B_KNIGHT U'\u265E'
#define B_PAWN   U'\u265F'

// Forward declarations
class Board;

// Абстрактный базовый класс для фигур
class Piece {
public:
    enum Type {
        KING,
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT,
        PAWN
    };

    enum Color {
        WHITE,
        BLACK
    };

    Color color;
    Type type;

    Piece(Color color, Type type) : color(color), type(type) {}

    virtual ~Piece() {}

    virtual bool isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const = 0;
    virtual char getSymbol() const = 0;
};

// Подклассы фигур
class King : public Piece {
public:
    King(Color color) : Piece(color, KING) {}
    bool isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const override;
    char getSymbol() const override { return (color == WHITE) ? W_KING : B_KING; }
};

class Queen : public Piece {
public:
    Queen(Color color) : Piece(color, QUEEN) {}
    bool isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const override;
    char getSymbol() const override { return (color == WHITE) ? W_QUEEN : B_QUEEN; }
};

class Rook : public Piece {
public:
    Rook(Color color) : Piece(color, ROOK) {}
    bool isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const override;
    char getSymbol() const override { return (color == WHITE) ? W_ROOK : B_ROOK; }
};

class Bishop : public Piece {
public:
    Bishop(Color color) : Piece(color, BISHOP) {}
    bool isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const override;
    char getSymbol() const override { return (color == WHITE) ? W_BISHOP : B_BISHOP; }
};

class Knight : public Piece {
public:
    Knight(Color color) : Piece(color, KNIGHT) {}
    bool isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const override;
    char getSymbol() const override { return (color == WHITE) ? W_KNIGHT : B_KNIGHT; }
};

class Pawn : public Piece {
public:
    Pawn(Color color) : Piece(color, PAWN) {}
    bool isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const override;
    char getSymbol() const override { return (color == WHITE) ? W_PAWN : B_PAWN; }
};

// Класс доски
class Board {
public:
    Piece* board[8][8];

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
    bool isInCheck(Piece::Color color) const;
private:
    bool isAttacked(int row, int col, Piece::Color attackerColor) const; // Вспомогательная функция для isInCheck

    //Вспомогательная функция, для проверки, что координаты в пределах доски
    bool isValidCoordinates(int row, int col) const{
        return (row >= 0 && row < 8 && col >= 0 && col < 8);
    }
};

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

// ----------------------- Piece -----------------------

bool King::isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const {
    int rowDiff = std::abs(endRow - startRow);
    int colDiff = std::abs(endCol - startCol);
    return (rowDiff <= 1 && colDiff <= 1 && (rowDiff + colDiff > 0)); // Ход на одну клетку в любом направлении
}

bool Queen::isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const {
    // Создаем временные объекты ладьи и слона с тем же цветом, что и ферзь
    Rook rook(color);
    Bishop bishop(color);

    // Проверяем, допустим ли ход как для ладьи или слона
    return rook.isValidMove(startRow, startCol, endRow, endCol, board) ||
           bishop.isValidMove(startRow, startCol, endRow, endCol, board);
}

bool Rook::isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const {
    // 0. Проверка на выход за пределы доски
    if (endRow < 0 || endRow > 7 || endCol < 0 || endCol > 7) {
        return false;
    }
    // Движение по горизонтали или вертикали
    if (startRow == endRow || startCol == endCol) {
        // Проверка препятствий на пути
        int stepRow = (endRow > startRow) ? 1 : ((endRow < startRow) ? -1 : 0); // Направление движения по строке
        int stepCol = (endCol > startCol) ? 1 : ((endCol < startCol) ? -1 : 0); // Направление движения по столбцу

        int currentRow = startRow + stepRow;
        int currentCol = startCol + stepCol;

        while (currentRow != endRow || currentCol != endCol) {
            if (board.getPiece(currentRow, currentCol) != nullptr) {
                return false; // На пути есть фигура
            }
            currentRow += stepRow;
            currentCol += stepCol;
        }

        // Проверка, что в конечной клетке нет фигуры того же цвета
        Piece* endPiece = board.getPiece(endRow, endCol);
        if (endPiece != nullptr && endPiece->color == color) {
            return false; // Клетка занята фигурой того же цвета
        }

        return true; // Ход как у ладьи допустим
    }
    return false;
}

bool Bishop::isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const {
        // 0. Проверка на выход за пределы доски
    if (endRow < 0 || endRow > 7 || endCol < 0 || endCol > 7) {
        return false;
    }
    // 1. Проверка как для слона (диагональное движение)
    else if (std::abs(endRow - startRow) == std::abs(endCol - startCol)) {
        // Движение по диагонали

        // Проверка препятствий на пути
        int stepRow = (endRow > startRow) ? 1 : -1; // Направление движения по строке
        int stepCol = (endCol > startCol) ? 1 : -1; // Направление движения по столбцу

        int currentRow = startRow + stepRow;
        int currentCol = startCol + stepCol;

        while (currentRow != endRow) {
            if (board.getPiece(currentRow, currentCol) != nullptr) {
                return false; // На пути есть фигура
            }
            currentRow += stepRow;
            currentCol += stepCol;
        }

         // Проверка, что в конечной клетке нет фигуры того же цвета
        Piece* endPiece = board.getPiece(endRow, endCol);
        if (endPiece != nullptr && endPiece->color == color) {
            return false; // Клетка занята фигурой того же цвета
        }

        return true; // Ход как у слона допустим
    }
        // 2. Если ни одно из условий не выполнено, ход не допустим
    return false;
}

bool Knight::isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const {
    // 0. Проверка на выход за пределы доски
    if (endRow < 0 || endRow > 7 || endCol < 0 || endCol > 7) {
        return false;
    }

    // 1. Проверка, что конь переместился на правильное расстояние - буквой "Г"
    int rowDiff = std::abs(endRow - startRow);
    int colDiff = std::abs(endCol - startCol);

    if (!((rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2))) {
        return false; // Неверное расстояние для коня
    }

    // 2. Проверка, что конечная клетка не занята фигурой того же цвета
    Piece* endPiece = board.getPiece(endRow, endCol);
    if (endPiece != nullptr && endPiece->color == color) {
        return false; // Клетка занята фигурой того же цвета
    }

    return true;
}

bool Pawn::isValidMove(int startRow, int startCol, int endRow, int endCol, const Board& board) const {
    // 0. Проверка на выход за пределы доски
    if (endRow < 0 || endRow > 7 || endCol < 0 || endCol > 7) {
        return false;
    }

    // Определяем направление движения пешки в зависимости от ее цвета
    int direction = (color == WHITE) ? 1 : -1; // 1 для белых (вверх), -1 для черных (вниз)
    int initialRow = (color == WHITE) ? 1 : 6; // Начальная позиция для белых и черных пешек

    // 1. Движение вперед на одну клетку
    if (endCol == startCol && endRow == startRow + direction) {
        // Проверяем, что клетка свободна
        if (board.getPiece(endRow, endCol) == nullptr) {
            return true;
        }
    }

    // 2. Движение вперед на две клетки с начальной позиции
    if (startRow == initialRow && endCol == startCol && endRow == startRow + 2 * direction) {
        // Проверяем, что обе клетки свободны
        if (board.getPiece(endRow, endCol) == nullptr && board.getPiece(startRow + direction, startCol) == nullptr) {
            return true;
        }
    }

    // 3. Атака по диагонали
    if (std::abs(endCol - startCol) == 1 && endRow == startRow + direction) {
        // Проверяем, что в клетке есть фигура противника
        Piece* endPiece = board.getPiece(endRow, endCol);
        if (endPiece != nullptr && endPiece->color != color) {
            return true;
        }
    }

    return false;
}

// ----------------------- Board -----------------------

void Board::setupBoard() {
    // Расстановка белых фигур
    board[0][0] = new Rook(Piece::WHITE);
    board[0][1] = new Knight(Piece::WHITE);
    board[0][2] = new Bishop(Piece::WHITE);
    board[0][3] = new Queen(Piece::WHITE);
    board[0][4] = new King(Piece::WHITE);
    board[0][5] = new Bishop(Piece::WHITE);
    board[0][6] = new Knight(Piece::WHITE);
    board[0][7] = new Rook(Piece::WHITE);
    for (int i = 0; i < 8; ++i) {
        board[1][i] = new Pawn(Piece::WHITE);
    }

    // Расстановка черных фигур
    board[7][0] = new Rook(Piece::BLACK);
    board[7][1] = new Knight(Piece::BLACK);
    board[7][2] = new Bishop(Piece::BLACK);
    board[7][3] = new Queen(Piece::BLACK);
    board[7][4] = new King(Piece::BLACK);
    board[7][5] = new Bishop(Piece::BLACK);
    board[7][6] = new Knight(Piece::BLACK);
    board[7][7] = new Rook(Piece::BLACK);
    for (int i = 0; i < 8; ++i) {
        board[6][i] = new Pawn(Piece::BLACK);
    }
}

void Board::printBoard() const {
    std::cout << "  "; // Отступ для верхних букв
    for (char c = 'a'; c <= 'h'; ++c) {
        std::cout << " " << c << " ";
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
    std::cout << "  "; // Отступ для нижних букв
    for (char c = 'a'; c <= 'h'; ++c) {
        std::cout << " " << c << " ";
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

void Board::makeMove(int startRow, int startCol, int endRow, int endCol) {
    // 1. Получить указатель на фигуру, которую перемещаем
    Piece* pieceToMove = board[startRow][startCol];

    // 2. Получить указатель на фигуру, которую захватываем (если есть)
    Piece* capturedPiece = board[endRow][endCol];

    // 3. Переместить фигуру
    board[endRow][endCol] = pieceToMove;
    board[startRow][startCol] = nullptr; // Освободить клетку, откуда переместили

    // 4. Удалить захваченную фигуру (если есть)
    if (capturedPiece != nullptr) {
        delete capturedPiece;
    }
}

void Board::undoMove(int startRow, int startCol, int endRow, int endCol, Piece* capturedPiece) {
    // 1. Получить указатель на фигуру, которую вернули
    Piece* pieceToMove = board[endRow][endCol];

    // 2. Вернуть фигуру на исходную позицию
    board[startRow][startCol] = pieceToMove;
    board[endRow][endCol] = capturedPiece; // Вернуть захваченную фигуру (если была)
}

bool Board::isInCheck(Piece::Color color) const {
    // TODO: шах?
    return false;
}

bool Board::isAttacked(int row, int col, Piece::Color attackerColor) const {
    // TODO: атакована ли клетка фигурами цвета attackerColor
    return false;
}

// ----------------------- Game -----------------------

void Game::startNewGame() {
    board.setupBoard();
    currentPlayer = Piece::WHITE;
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
    std::cout << "Введите ход или 'exit' для выхода: ";
    std::string input;
    std::getline(std::cin, input);

    // Преобразование ввода в нижний регистр (для удобства)
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

    // 4. Проверка возможности хода
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