#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <vector>
#include <utility>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

enum Difficulty { EASY, NORMAL, HARD };

struct Cell {
    bool hasShip = false;
    bool hit = false;
};

struct Ship {
    int length;
    std::vector<std::pair<int, int>> cells;
    bool isSunk() const { return cells.empty(); }  // Định nghĩa hàm
};

class BattleshipGame {
private:
    int size;
    Difficulty difficulty;
    std::vector<std::vector<Cell>> playerBoard;
    std::vector<std::vector<Cell>> aiBoard;
    std::vector<Ship> playerShips;
    std::vector<Ship> aiShips;

    // AI state
    std::vector<std::pair<int, int>> aiHits;  // NORMAL: hunt mode
    std::vector<std::pair<int, int>> aiHuntStack; // HARD: priority queue (chưa dùng, nhưng giữ)

    void placeShipsRandom(std::vector<std::vector<Cell>>& board, std::vector<Ship>& ships);
    bool canPlaceShip(const std::vector<std::vector<Cell>>& board, int x, int y, int len, bool horizontal);
    void placeShip(std::vector<std::vector<Cell>>& board, std::vector<Ship>& ships, int x, int y, int len, bool horizontal);

    bool isValid(int x, int y) const;
    bool allShipsSunk(const std::vector<Ship>& ships) const;

    // AI
    void easyAI();
    void normalAI();
    void hardAI();
    int calculateProbability(int x, int y);

public:
    BattleshipGame(int n = 10, Difficulty diff = NORMAL);
    void printBoards(bool revealAI = false) const;
    bool playerShoot(int x, int y);  // return true if valid
    void aiShoot();
    bool isGameOver() const;
    bool didPlayerWin() const;
    int getPlayerShipsRemaining() const { return playerShips.size(); }
    int getAIShipsRemaining() const { return aiShips.size(); }
};

#endif