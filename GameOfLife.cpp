#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include <random>
#include <vector>
#include <cstdint>
#include "GameOfLife.h"
#include <string>

using namespace std;
//Glider simple
std::vector<std::vector<uint64_t>> lwss() {
	int rows = 30, cols = 30;
    std::vector<std::vector<uint64_t>> grid(rows, std::vector<uint64_t>(cols, 0));
    int r = 5, c = 5;
    grid[r][c + 1] = 1;
    grid[r][c + 2] = 1;
    grid[r + 1][c] = 1;
    grid[r + 2][c] = 1;
    grid[r + 3][c] = 1;
    grid[r + 3][c + 1] = 1;
    grid[r + 3][c + 2] = 1;
    grid[r + 3][c + 3] = 1;
    grid[r + 2][c + 4] = 1;
    return grid;
}
//Dispara Gliders
std::vector<std::vector<uint64_t>> gliderGun(int rows, int cols) {
    std::vector<std::vector<uint64_t>> g(rows, std::vector<uint64_t>(cols, 0));
    int r = 10, c = 10;

    std::vector<std::pair<int, int>> coords = {
        {0,24},
        {1,22},{1,24},
        {2,12},{2,13},{2,20},{2,21},{2,34},{2,35},
        {3,11},{3,15},{3,20},{3,21},{3,34},{3,35},
        {4,0},{4,1},{4,10},{4,16},{4,20},{4,21},
        {5,0},{5,1},{5,10},{5,14},{5,16},{5,17},{5,22},{5,24},
        {6,10},{6,16},{6,24},
        {7,11},{7,15},
        {8,12},{8,13}
    };

    for (auto [dr, dc] : coords) {
        if (r + dr < rows && c + dc < cols)
            g[r + dr][c + dc] = 1;
    }

    return g;
}
//Grilla Random
std::vector<std::vector<uint64_t>> randomGrid(int rows, int cols, double density = 0.1) {
    std::vector<std::vector<uint64_t>> grid(rows, std::vector<uint64_t>(cols, 0));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(density);

    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            grid[r][c] = d(gen);
    return grid;
}
//Generador de Gliders Mientras Avanza
std::vector<std::vector<uint64_t>> backrake2(int rows, int cols) {
    std::vector<std::vector<uint64_t>> g(rows, std::vector<uint64_t>(cols, 0));
    int r = 10, c = 10;
    std::vector<std::pair<int, int>> pattern = {
        {0,0},{0,1},{1,0},{1,1},
        {0,9},{0,10},{1,8},{1,9},{2,10},
        {3,11},{4,12},{5,13},{6,14}
    };
    for (auto [dr, dc] : pattern)
        g[r + dr][c + dc] = 1;
    return g;
}
std::vector<std::vector<uint64_t>> quadPulsar() {
    int rows = 40, cols = 40;
    std::vector<std::vector<uint64_t>> g(rows, std::vector<uint64_t>(cols, 0));
    auto placePulsar = [&](int r, int c) {
        int d[] = { 0,1,2,4,5,6 };
        for (int i : d) {
            g[r + i][c + 2] = 1;
            g[r + i][c + 7] = 1;
            g[r + 2][c + i] = 1;
            g[r + 7][c + i] = 1;
        }
        };
    placePulsar(1, 1);
    placePulsar(1, 20);
    placePulsar(20, 1);
    placePulsar(20, 20);
    return g;
}
std::vector<std::vector<uint64_t>> epicShowcaseGrid() {
    int rows = 100, cols = 200;
    std::vector<std::vector<uint64_t>> g(rows, std::vector<uint64_t>(cols, 0));

    auto place = [&](int r, int c, const std::vector<std::pair<int, int>>& pattern) {
        for (auto [dr, dc] : pattern)
            if (r + dr < rows && c + dc < cols)
                g[r + dr][c + dc] = 1;
        };

    // Acorn
    place(5, 5, {
        {0,1}, {1,3},
        {2,0},{2,1},{2,4},{2,5},{2,6}
        });

    // Diehard
    place(5, 40, {
        {0,6},
        {1,0},{1,1},
        {2,1},{2,5},{2,6},{2,7}
        });

    // R-pentomino
    place(5, 80, {
        {0,1},{0,2},{1,0},{1,1},{2,1}
        });

    // Simkin glider gun
    place(30, 5, {
        {0,1},{1,0},{1,1},{1,2},{2,2},
        {4,2},{5,3},{6,1},{6,3},{7,2},
        {9,2},{10,2},{11,2},
        {13,1},{13,3},{14,1},{14,3},
        {15,0},{15,1},{15,3},{15,4}
        });

    // Pentadecathlon
    place(30, 60, [&]() {
        std::vector<std::pair<int, int>> p;
        for (int i = 0; i < 10; ++i) p.push_back({ 0, i });
        p.push_back({ -1,1 }); p.push_back({ 1,1 });
        p.push_back({ -1,8 }); p.push_back({ 1,8 });
        return p;
        }());

    // Breeder
    place(70, 5, {
        {0,0},{0,1},{1,0},{1,1},
        {10,0},{10,1},{11,0},{11,1},
        {12,2},{13,3},{14,4},{15,5},{16,6},{17,7},{18,8}
        });

    // Glider collision (logic AND visual)
    auto glider = [](int r, int c, bool flip = false) {
        std::vector<std::pair<int, int>> g;
        if (!flip) {
            g = { {0,1}, {1,2}, {2,0}, {2,1}, {2,2} };
        }
        else {
            g = { {0,0}, {1,-1}, {2,-2}, {2,-1}, {2,0} };
        }
        for (auto& [dr, dc] : g) {
            dr += r; dc += c;
        }
        return g;
        };
    for (auto [r, c] : glider(60, 120, false)) g[r][c] = 1;
    for (auto [r, c] : glider(60, 140, true)) g[r][c] = 1;

    return g;
}



std::vector<std::vector<uint64_t>> koksGalaxy() {
    int rows = 10, cols = 10;
    std::vector<std::vector<uint64_t>> g(rows, std::vector<uint64_t>(cols, 0));
    int r = 3, c = 3;
    std::vector<std::pair<int, int>> pattern = {
        {0,1},{0,2},{1,0},{1,3},{2,0},{2,3},{3,1},{3,2}
    };
    for (int dx = -1; dx <= 1; dx += 2)
        for (int dy = -1; dy <= 1; dy += 2)
            for (auto [dr, dc] : pattern)
                g[r + dr * dy][c + dc * dx] = 1;
    return g;
}


int main()
{
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Juego de la Vida de Conway");
    int cellSize = 5;
    //int cols = window.getSize().x / cellSize;
    //int rows = window.getSize().y / cellSize;
    int cols = 1000;
    int rows = 1000;

    sf::View view;
    view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
    view.setSize(window.getSize().x, window.getSize().y);
    view.setCenter(view.getSize() / 2.f);
    uint64_t initialValue = 0;
    
    vector<vector<uint64_t>> grid(cols, vector<uint64_t>(rows, initialValue));
    vector<vector<uint64_t>> temporaryGrid(cols, vector<uint64_t>(rows, initialValue));

    grid = randomGrid(1000,1000,.3f);
    vector<vector<uint64_t>> initialGrid = grid;
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) return -1;

    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setString("FPS:0");
    fpsText.setCharacterSize(24);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(50, 50);

    int fps = 0;
    float fpsCount = 0.f;

    bool isPlaying = false;
    float thisTurn = 0.0f;
    float dt = 0;

    auto lastFrame = chrono::high_resolution_clock::now();
	while (window.isOpen())
	{
        auto thisFrame = chrono::high_resolution_clock::now();
        chrono::duration<float> delta = thisFrame - lastFrame;
        lastFrame = thisFrame;
        dt = delta.count();
        fps++;
        fpsCount += dt;

        if (fpsCount >= 1.f) {
            fpsCount = 0.f;
            fpsText.setString("FPS: " + to_string(fps));
            fps = 0;

        }
		sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                isPlaying = !isPlaying;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            view.move(sf::Vector2f(-500.0f * dt, 0.0f));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            view.move(sf::Vector2f(500.0f * dt, 0.0f));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            view.move(sf::Vector2f(0, -500.0f * dt));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            view.move(sf::Vector2f(0.0f, 500.0f * dt));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
            view.zoom(1.f - dt);  // Acerca (zoom in)
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
            view.zoom(1.f + dt );  // Aleja (zoom out)
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            grid = initialGrid;
        }
        
        thisTurn += dt;
        if (thisTurn > 0.1f && isPlaying) {
            thisTurn = 0.0f;

            for (size_t x = 0; x < grid.size(); x++) {
                if (grid[x][0]) {
                    for (vector<uint64_t>& v : grid) {
                        v.insert(v.begin(), 0);
                    }

                    view.move(sf::Vector2f(0.f, cellSize));

                    break;
                }
            }
            for (size_t y = 0; y < grid[0].size(); y++) {
                if (grid[0][y]) {
                    grid.insert(grid.begin(), vector<uint64_t>(grid[0].size(), 0));
                    view.move(sf::Vector2f(cellSize, 0.f));
                    break;

                }
            }

            for (size_t x = 0; x < grid.size(); x++) {
                if (grid[x][grid[0].size()-1]) {
                    for (vector<uint64_t>& v : grid) {
                        v.insert(v.end(), 0);
                    }
                    break;
                }
            }
            for (size_t y = 0; y < grid[0].size(); y++) {
                if (grid[grid.size()-1][y]) {
                    grid.insert(grid.end(), vector<uint64_t>(grid[0].size(), 0));
                    break;

                }
            }
            temporaryGrid = grid;
            for (size_t x = 0; x < grid.size(); x++) {
                for (size_t y = 0; y < grid[0].size(); y++) {

                    int aliveCellsAround = 0;
                    for (int dx = -1; dx <= 1; dx++) {
                        for (int dy = -1; dy <= 1; dy++) {
                            if (dx == 0 && dy == 0) continue;
                            if (x + dx < 0 || x + dx >= grid.size()) continue;
                            if (y + dy < 0 || y + dy >= grid[0].size()) continue;
                            aliveCellsAround += static_cast<int>(grid[x + dx][y + dy]);
                        }
                    }

                    if (grid[x][y]) {
                        if (aliveCellsAround >= 2 && aliveCellsAround <= 3) {
                            temporaryGrid[x][y] = 1;
                        }
                        else {
                            temporaryGrid[x][y] = 0;
                        }
                    }
                    else {
                        if (aliveCellsAround == 3) {
                            temporaryGrid[x][y] = 1;
                        }
                        else {
                            temporaryGrid[x][y] = 0;
                        }
                    }

                }
            }

            grid = temporaryGrid;
        }

        window.clear();
        window.setView(view);

        // DIBUJO DE LAS CELDAS
        for (size_t x = 0; x < grid.size(); x++) {
            for (size_t y = 0; y < grid[0].size(); y++) {
                if (!grid[x][y]) continue;
                sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
                cell.setPosition(sf::Vector2f(x * cellSize, y * cellSize));
                cell.setFillColor(sf::Color::White);
                window.draw(cell);
            }
        }
       
		window.setView(window.getDefaultView());

        window.draw(fpsText);

        window.display();

        cout << "En X: " << grid.size() << " En Y: " << grid[0].size() << endl;
        
	}
	return 0;
}
