#include "gameplay.h"
#include <iostream>
#include <limits>

using namespace std;

int main() {
    cout << "=== BATTLESHIP GAME ===\n";
    cout << "Kích thước bàn: 10x10 | Tàu: 2,3,3,4,5\n";
    cout << "Chọn độ khó:\n1. Easy\n2. Normal\n3. Hard\n> ";

    int mode;
    while (!(cin >> mode) || mode < 1 || mode > 3) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Chọn 1, 2 hoặc 3: ";
    }
    Difficulty diff = (mode == 1) ? EASY : (mode == 2) ? NORMAL : HARD;

    BattleshipGame game(10, diff);

    cout << "\nBẮT ĐẦU TRẬN ĐẤU!\n";
    while (!game.isGameOver()) {
        game.printBoards();
        int x, y;
        while (true) {
            cout << "\nNhập toạ độ bắn (x y): ";
            if (cin >> x >> y && game.playerShoot(x, y)) break;
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        if (game.isGameOver()) break;

        cout << "\n--- LƯỢT AI ---\n";
        game.aiShoot();

        if (game.isGameOver()) {
            game.printBoards(true);
        }
    }

    cout << "\n=== KẾT THÚC TRẬN ĐẤU ===\n";
    game.printBoards(true);
    if (game.didPlayerWin()) {
        cout << "BẠN ĐÃ CHIẾN THẮNG AI!\n";
    } else {
        cout << "AI ĐÃ ĐÁNH BẠI BẠN!\n";
    }
    return 0;
}