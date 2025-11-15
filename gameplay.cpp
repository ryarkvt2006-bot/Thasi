#include "gameplay.h"
#include <algorithm>
#include <cstdlib>   
#include <ctime>
using namespace std;

BattleshipGame::BattleshipGame(int n, Difficulty diff)
    : size(n), difficulty(diff) {
    srand(static_cast<unsigned>(time(nullptr)));  // Sửa để tránh warning
    playerBoard.assign(size, vector<Cell>(size));
    aiBoard.assign(size, vector<Cell>(size));

    placeShipsRandom(playerBoard, playerShips);
    placeShipsRandom(aiBoard, aiShips);
}

void BattleshipGame::placeShipsRandom(vector<vector<Cell>>& board, vector<Ship>& ships) {
    vector<int> lengths = {2, 3, 3, 4, 5};
    for (int len : lengths) {
        bool placed = false;
        int attempts = 0;
        while (!placed && attempts < 1000) {
            int x = rand() % size;
            int y = rand() % size;
            bool horizontal = rand() % 2;
            if (canPlaceShip(board, x, y, len, horizontal)) {
                placeShip(board, ships, x, y, len, horizontal);
                placed = true;
            }
            attempts++;
        }
        if (!placed) cout << "Warning: Could not place ship of length " << len << endl;
    }
}

bool BattleshipGame::canPlaceShip(const vector<vector<Cell>>& board, int x, int y, int len, bool horizontal) {
    if (horizontal) {
        if (y + len > size) return false;
        for (int i = 0; i < len; i++) {
            if (board[x][y + i].hasShip) return false;
            if (x > 0 && board[x-1][y+i].hasShip) return false;
            if (x < size-1 && board[x+1][y+i].hasShip) return false;
            if (y+i > 0 && board[x][y+i-1].hasShip) return false;
            if (y+i < size-1 && board[x][y+i+1].hasShip) return false;
        }
    } else {
        if (x + len > size) return false;
        for (int i = 0; i < len; i++) {
            if (board[x + i][y].hasShip) return false;
            if (y > 0 && board[x+i][y-1].hasShip) return false;
            if (y < size-1 && board[x+i][y+1].hasShip) return false;
            if (x+i > 0 && board[x+i-1][y].hasShip) return false;
            if (x+i < size-1 && board[x+i+1][y].hasShip) return false;
        }
    }
    return true;
}

void BattleshipGame::placeShip(vector<vector<Cell>>& board, vector<Ship>& ships, int x, int y, int len, bool horizontal) {
    Ship ship;
    ship.length = len;
    if (horizontal) {
        for (int i = 0; i < len; i++) {
            board[x][y + i].hasShip = true;
            ship.cells.emplace_back(x, y + i);
        }
    } else {
        for (int i = 0; i < len; i++) {
            board[x + i][y].hasShip = true;
            ship.cells.emplace_back(x + i, y);
        }
    }
    ships.push_back(ship);
}

bool BattleshipGame::isValid(int x, int y) const {
    return x >= 0 && x < size && y >= 0 && y < size;
}

void BattleshipGame::printBoards(bool revealAI) const {
    cout << "\n=== BẢNG NGƯỜI CHƠI ===\n   ";
    for (int i = 0; i < size; i++) cout << i << (i < 10 ? " " : "");
    cout << "\n";
    for (int i = 0; i < size; i++) {
        cout << i << (i < 10 ? "  " : " ");
        for (int j = 0; j < size; j++) {
            if (playerBoard[i][j].hit) {
                cout << (playerBoard[i][j].hasShip ? "X " : "o ");
            } else if (playerBoard[i][j].hasShip) {
                cout << "S ";
            } else {
                cout << ". ";
            }
        }
        cout << "\n";
    }

    cout << "\n=== BẢNG AI (Ẩn) ===\n   ";
    for (int i = 0; i < size; i++) cout << i << (i < 10 ? " " : "");
    cout << "\n";
    for (int i = 0; i < size; i++) {
        cout << i << (i < 10 ? "  " : " ");
        for (int j = 0; j < size; j++) {
            if (aiBoard[i][j].hit) {
                cout << (aiBoard[i][j].hasShip ? "X " : "o ");
            } else if (revealAI && aiBoard[i][j].hasShip) {
                cout << "S ";
            } else {
                cout << ". ";
            }
        }
        cout << "\n";
    }
    cout << "Tàu bạn còn: " << playerShips.size()
         << " | Tàu AI còn: " << aiShips.size() << "\n\n";
}

bool BattleshipGame::playerShoot(int x, int y) {
    if (!isValid(x, y)) {
        cout << "Tọa độ không hợp lệ! (0-" << size-1 << ")\n";
        return false;
    }
    if (aiBoard[x][y].hit) {
        cout << "Ô này đã bắn rồi!\n";
        return false;
    }
    aiBoard[x][y].hit = true;

    if (aiBoard[x][y].hasShip) {
        cout << "BẠN TRÚNG TÀU AI!\n";
        for (auto& ship : aiShips) {
            bool allHit = true;
            for (auto [sx, sy] : ship.cells) {
                if (!aiBoard[sx][sy].hit) { allHit = false; break; }
            }
            if (allHit && !ship.cells.empty()) {
                cout << "BẠN ĐÁNH CHÌM TÀU DÀI " << ship.length << "!\n";
                ship.cells.clear();
            }
        }
        aiShips.erase(remove_if(aiShips.begin(), aiShips.end(),
            [](const Ship& s) { return s.cells.empty(); }), aiShips.end());
    } else {
        cout << "Bạn bắn trượt!\n";
    }
    return true;
}

void BattleshipGame::aiShoot() {
    if (difficulty == EASY) easyAI();
    else if (difficulty == NORMAL) normalAI();
    else hardAI();
}

void BattleshipGame::easyAI() {
    int x, y;
    do {
        x = rand() % size; y = rand() % size;
    } while (playerBoard[x][y].hit);
    playerBoard[x][y].hit = true;
    cout << "[EASY] AI bắn (" << x << ", " << y << "): ";
    cout << (playerBoard[x][y].hasShip ? "TRÚNG!\n" : "TRƯỢT!\n");
}

void BattleshipGame::normalAI() {
    int x = -1, y = -1;
    if (!aiHits.empty()) {
        auto [hx, hy] = aiHits.back();
        vector<pair<int,int>> dirs = {{0,1},{1,0},{0,-1},{-1,0}};
        for (auto [dx, dy] : dirs) {
            int nx = hx + dx, ny = hy + dy;
            if (isValid(nx, ny) && !playerBoard[nx][ny].hit) {
                x = nx; y = ny; break;
            }
        }
        if (x == -1) aiHits.pop_back();
    }
    if (x == -1) {
        do { x = rand() % size; y = rand() % size; } while (playerBoard[x][y].hit);
    }

    playerBoard[x][y].hit = true;
    cout << "[NORMAL] AI bắn (" << x << ", " << y << "): ";
    if (playerBoard[x][y].hasShip) {
        cout << "TRÚNG!\n";
        aiHits.push_back({x, y});
        for (auto& ship : playerShips) {
            bool allHit = true;
            for (auto [sx, sy] : ship.cells) {
                if (!playerBoard[sx][sy].hit) { allHit = false; break; }
            }
            if (allHit && !ship.cells.empty()) {
                cout << "AI ĐÁNH CHÌM TÀU CỦA BẠN!\n";
                ship.cells.clear();
            }
        }
        playerShips.erase(remove_if(playerShips.begin(), playerShips.end(),
            [](const Ship& s) { return s.cells.empty(); }), playerShips.end());
    } else {
        cout << "TRƯỢT!\n";
    }
}

int BattleshipGame::calculateProbability(int x, int y) {
    if (playerBoard[x][y].hit) return 0;
    int score = 0;
    vector<int> lengths = {2, 3, 3, 4, 5};  // Sửa: thêm tàu 3 thứ hai
    for (int len : lengths) {
        // Horizontal right
        if (y + len <= size) { 
            bool valid = true; 
            for (int i = 0; i < len; i++) 
                if (playerBoard[x][y+i].hit && !playerBoard[x][y+i].hasShip) { valid = false; break; } 
            if (valid) score += 10; 
        }
        // Horizontal left
        if (y - len + 1 >= 0) { 
            bool valid = true; 
            for (int i = 0; i < len; i++) 
                if (playerBoard[x][y-len+1+i].hit && !playerBoard[x][y-len+1+i].hasShip) { valid = false; break; } 
            if (valid) score += 10; 
        }
        // Vertical down
        if (x + len <= size) { 
            bool valid = true; 
            for (int i = 0; i < len; i++) 
                if (playerBoard[x+i][y].hit && !playerBoard[x+i][y].hasShip) { valid = false; break; } 
            if (valid) score += 10; 
        }
        // Vertical up
        if (x - len + 1 >= 0) { 
            bool valid = true; 
            for (int i = 0; i < len; i++) 
                if (playerBoard[x-len+1+i][y].hit && !playerBoard[x-len+1+i][y].hasShip) { valid = false; break; } 
            if (valid) score += 10; 
        }
    }
    return score;
}

void BattleshipGame::hardAI() {
    int bestX = -1, bestY = -1, bestScore = -1;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (playerBoard[i][j].hit) continue;
            int score = calculateProbability(i, j);
            if (score > bestScore) { bestScore = score; bestX = i; bestY = j; }
        }
    }
    if (bestX == -1) {
        do { bestX = rand() % size; bestY = rand() % size; } while (playerBoard[bestX][bestY].hit);
    }
    playerBoard[bestX][bestY].hit = true;
    cout << "[HARD] AI bắn (" << bestX << ", " << bestY << "): ";
    if (playerBoard[bestX][bestY].hasShip) {
        cout << "TRÚNG!\n";
        for (auto& ship : playerShips) {
            bool allHit = true;
            for (auto [sx, sy] : ship.cells) {
                if (!playerBoard[sx][sy].hit) { allHit = false; break; }
            }
            if (allHit && !ship.cells.empty()) {
                cout << "AI ĐÁNH CHÌM TÀU CỦA BẠN!\n";
                ship.cells.clear();
            }
        }
        playerShips.erase(remove_if(playerShips.begin(), playerShips.end(),
            [](const Ship& s) { return s.cells.empty(); }), playerShips.end());
    } else {
        cout << "TRƯỢT!\n";
    }
}

bool BattleshipGame::isGameOver() const {
    return playerShips.empty() || aiShips.empty();
}

bool BattleshipGame::didPlayerWin() const {
    return aiShips.empty();
}

bool BattleshipGame::allShipsSunk(const std::vector<Ship>& ships) const {
    return ships.empty();
}