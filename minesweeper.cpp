#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iomanip>

using namespace std;

class Minesweeper {
private:
    int rows, cols, mines;
    vector<vector<int>> board;           // 0 = safe, -1 = mine
    vector<vector<bool>> revealed;       // Track revealed cells
    vector<vector<bool>> flagged;        // Track flagged cells
    bool gameOver;
    bool won;

public:
    Minesweeper(int r, int c, int m) : rows(r), cols(c), mines(m), gameOver(false), won(false) {
        board.assign(rows, vector<int>(cols, 0));
        revealed.assign(rows, vector<bool>(cols, false));
        flagged.assign(rows, vector<bool>(cols, false));
        
        // Validate mines count
        if (mines > rows * cols - 1) {
            this->mines = (rows * cols - 1);
        }
        
        placeMines();
        calculateNumbers();
    }

    void placeMines() {
        int placed = 0;
        while (placed < mines) {
            int r = rand() % rows;
            int c = rand() % cols;
            
            // Don't place mine if already exists
            if (board[r][c] != -1) {
                board[r][c] = -1;
                placed++;
            }
        }
    }

    void calculateNumbers() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (board[i][j] != -1) {
                    int count = 0;
                    // Check all 8 adjacent cells
                    for (int di = -1; di <= 1; di++) {
                        for (int dj = -1; dj <= 1; dj++) {
                            int ni = i + di;
                            int nj = j + dj;
                            if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                                if (board[ni][nj] == -1) {
                                    count++;
                                }
                            }
                        }
                    }
                    board[i][j] = count;
                }
            }
        }
    }

    void displayBoard(bool showMines = false) {
        cout << "\n";
        cout << "   ";
        for (int j = 0; j < cols; j++) {
            cout << setw(3) << j;
        }
        cout << "\n";
        cout << "   " << string(cols * 3, '-') << "\n";

        for (int i = 0; i < rows; i++) {
            cout << setw(2) << i << "|";
            for (int j = 0; j < cols; j++) {
                cout << " ";
                if (!revealed[i][j]) {
                    if (flagged[i][j]) {
                        cout << "F";
                    } else {
                        cout << ".";
                    }
                } else {
                    if (board[i][j] == -1) {
                        cout << "*";
                    } else if (board[i][j] == 0) {
                        cout << " ";
                    } else {
                        cout << board[i][j];
                    }
                }
                cout << " ";
            }
            cout << "|\n";
        }
        cout << "   " << string(cols * 3, '-') << "\n";

        if (showMines) {
            cout << "\nSemua ranjau:\n";
            cout << "   ";
            for (int j = 0; j < cols; j++) {
                cout << setw(3) << j;
            }
            cout << "\n";
            cout << "   " << string(cols * 3, '-') << "\n";

            for (int i = 0; i < rows; i++) {
                cout << setw(2) << i << "|";
                for (int j = 0; j < cols; j++) {
                    cout << " ";
                    if (board[i][j] == -1) {
                        cout << "*";
                    } else if (board[i][j] == 0) {
                        cout << " ";
                    } else {
                        cout << board[i][j];
                    }
                    cout << " ";
                }
                cout << "|\n";
            }
            cout << "   " << string(cols * 3, '-') << "\n";
        }
    }

    void revealCell(int r, int c) {
        if (r < 0 || r >= rows || c < 0 || c >= cols) {
            cout << "Koordinat tidak valid!\n";
            return;
        }

        if (revealed[r][c]) {
            cout << "Sel sudah dibuka!\n";
            return;
        }

        if (flagged[r][c]) {
            cout << "Sel ini sudah ditandai!\n";
            return;
        }

        if (board[r][c] == -1) {
            revealed[r][c] = true;
            gameOver = true;
            won = false;
            return;
        }

        // Flood fill algorithm for empty cells
        floodFill(r, c);
        checkWin();
    }

    void floodFill(int r, int c) {
        if (r < 0 || r >= rows || c < 0 || c >= cols) {
            return;
        }

        if (revealed[r][c] || flagged[r][c]) {
            return;
        }

        revealed[r][c] = true;

        // If cell has no adjacent mines, reveal neighbors
        if (board[r][c] == 0) {
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    floodFill(r + di, c + dj);
                }
            }
        }
    }

    void toggleFlag(int r, int c) {
        if (r < 0 || r >= rows || c < 0 || c >= cols) {
            cout << "Koordinat tidak valid!\n";
            return;
        }

        if (revealed[r][c]) {
            cout << "Sel sudah dibuka!\n";
            return;
        }

        flagged[r][c] = !flagged[r][c];
        if (flagged[r][c]) {
            cout << "Sel ditandai sebagai ranjau\n";
        } else {
            cout << "Tanda dihapus\n";
        }
    }

    void checkWin() {
        int revealedCount = 0;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (revealed[i][j]) {
                    revealedCount++;
                }
            }
        }

        if (revealedCount == (rows * cols - mines)) {
            gameOver = true;
            won = true;
        }
    }

    bool isGameOver() {
        return gameOver;
    }

    bool hasWon() {
        return won;
    }

    void play() {
        cout << "\n=== MINESWEEPER GAME ===\n";
        cout << "Grid: " << rows << "x" << cols << ", Ranjau: " << mines << "\n";
        cout << "\nPerintah:\n";
        cout << "  o [row] [col] - Buka sel\n";
        cout << "  f [row] [col] - Tandai/Hapus tanda ranjau\n";
        cout << "  q             - Keluar dari game\n";

        while (!gameOver) {
            displayBoard();
            
            string command;
            int row, col;
            
            cout << "Masukkan perintah: ";
            cin >> command;

            if (command == "q") {
                cout << "Terima kasih telah bermain!\n";
                break;
            } else if (command == "o") {
                cin >> row >> col;
                revealCell(row, col);
            } else if (command == "f") {
                cin >> row >> col;
                toggleFlag(row, col);
            } else {
                cout << "Perintah tidak dikenali!\n";
            }
        }

        if (gameOver) {
            displayBoard(true);
            if (won) {
                cout << "\n🎉 SELAMAT! ANDA MENANG! 🎉\n";
            } else {
                cout << "\n💣 GAME OVER! ANDA KENA RANJAU! 💣\n";
            }
        }
    }
};

int main() {
    srand(static_cast<unsigned>(time(0)));

    int choice;
    cout << "=== MINESWEEPER GAME ===\n\n";
    cout << "Pilih tingkat kesulitan:\n";
    cout << "1. Mudah (6x6, 5 ranjau)\n";
    cout << "2. Sedang (8x8, 10 ranjau)\n";
    cout << "3. Sulit (10x10, 20 ranjau)\n";
    cout << "4. Kustom\n";
    cout << "Pilihan: ";
    cin >> choice;

    int rows, cols, mines;

    switch (choice) {
        case 1:
            rows = 6;
            cols = 6;
            mines = 5;
            break;
        case 2:
            rows = 8;
            cols = 8;
            mines = 10;
            break;
        case 3:
            rows = 10;
            cols = 10;
            mines = 20;
            break;
        case 4:
            cout << "Masukkan baris: ";
            cin >> rows;
            cout << "Masukkan kolom: ";
            cin >> cols;
            cout << "Masukkan jumlah ranjau: ";
            cin >> mines;
            if (rows < 3 || cols < 3) {
                cout << "Ukuran grid minimal 3x3\n";
                return 1;
            }
            break;
        default:
            cout << "Pilihan tidak valid!\n";
            return 1;
    }

    Minesweeper game(rows, cols, mines);
    game.play();

    return 0;
}
