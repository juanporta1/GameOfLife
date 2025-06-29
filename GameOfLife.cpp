#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include <random>
#include <vector>
#include <cstdint>
#include "GameOfLife.h"
#include <string>
#include <Grid.h>
using namespace std;

//Grilla Random
std::vector<uint64_t> randomGrid(int rows, int cols, double density = 0.1) {
    std::vector<uint64_t> grid(rows * cols,0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(density);

    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            grid[r * cols + c] = d(gen);
    return grid;
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
    

    vector<uint64_t> g = randomGrid(rows, cols, .3f);
	Grid mainGrid(rows, cols, g);
    Grid temporaryGrid(rows,cols, g);
    vector<uint64_t> initialGrid = mainGrid.grid;

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
            mainGrid.grid = initialGrid;
        }
        
        thisTurn += dt;
        if (thisTurn > 0.1f && isPlaying) {
            thisTurn = 0.0f;


           for(int x = 0; x < mainGrid.rows; x++) {
                if (mainGrid.grid[x * mainGrid.cols]) {
                    for (int i = 0; i < mainGrid.rows; i++) {
                        
                        mainGrid.grid.insert(mainGrid.grid.begin() + mainGrid.cols * i + i, 0);
                    }
                    mainGrid.cols++;
                    view.move(sf::Vector2f(0.f, cellSize));

                    break;
                }
			}

            for (int y = 0; y < mainGrid.cols; y++) {
                if (mainGrid.grid[y]) {
                    mainGrid.grid.insert(mainGrid.grid.begin(),mainGrid.cols, 0);
                    mainGrid.rows++;
                    view.move(sf::Vector2f(cellSize, 0.f));

                    break;

                }
            }

            for (int x = 0; x < mainGrid.rows; x++) {
                if (mainGrid.grid[x * mainGrid.cols + mainGrid.cols - 1]) {
                    for (int i = 0; i < mainGrid.rows; i++) {
                        mainGrid.grid.insert(mainGrid.grid.begin() + (i + 1) * mainGrid.cols + i - 1, 0);
                    }
                    mainGrid.cols++;
                    break;
                }
            }

            for (int y = 0; y < mainGrid.cols; y++) {
                if (mainGrid.grid[(mainGrid.rows - 1) * mainGrid.cols + y]) {
                    mainGrid.grid.insert(mainGrid.grid.end(), mainGrid.cols, 0);
                    mainGrid.rows++;
                    break;
                }
            }
           
            temporaryGrid = mainGrid;
            for (size_t x = 0; x < mainGrid.rows; x++) {
                for (size_t y = 0; y < mainGrid.cols; y++) {

                    int aliveCellsAround = 0;
                    for (int dx = -1; dx <= 1; dx++) {
                        for (int dy = -1; dy <= 1; dy++) {
                            if (dx == 0 && dy == 0) continue;
                            if (x + dx < 0 || x + dx >= mainGrid.rows) continue;
                            if (y + dy < 0 || y + dy >= mainGrid.cols) continue;
                            aliveCellsAround += static_cast<int>(mainGrid.grid[(x + dx) * mainGrid.cols + (y+dy)]);
                        }
                    }

                    if (mainGrid.grid[x * mainGrid.cols + y]) {
                        if (aliveCellsAround >= 2 && aliveCellsAround <= 3) {
                            temporaryGrid.grid[x * temporaryGrid.cols + y] = 1;
                        }
                        else {
                            temporaryGrid.grid[x * temporaryGrid.cols + y] = 0;
                        }
                    }
                    else {
                        if (aliveCellsAround == 3) {
                            temporaryGrid.grid[x * temporaryGrid.cols + y] = 1;
                        }
                        else {
                            temporaryGrid.grid[x * temporaryGrid.cols + y] = 0;
                        }
                    }

                }
            }

            mainGrid = temporaryGrid;
        }

        window.clear();
        window.setView(view);

        // DIBUJO DE LAS CELDAS
        for (size_t x = 0; x < mainGrid.rows; x++) {
            for (size_t y = 0; y < mainGrid.cols; y++) {
                if (!mainGrid.grid[x * mainGrid.cols + y]) continue;
                sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
                cell.setPosition(sf::Vector2f(x * cellSize, y * cellSize));
                cell.setFillColor(sf::Color::White);
                window.draw(cell);
            }
        }
       
		window.setView(window.getDefaultView());

        window.draw(fpsText);

        window.display();

        cout << "En X: " << mainGrid.rows << " En Y: " << mainGrid.cols << endl;
        
	}
	return 0;
}
