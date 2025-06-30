#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include <random>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include "GameOfLife.h"
#include <string>
#include <robin_hood.h>
using namespace std;

//Grilla Random
struct hasher {
    std::size_t operator()(std::pair<int, int> p) const {
        std::hash<int> hasher;
        return hasher(p.first) ^ (hasher(p.second) << 1); // XOR de los hashes de las coordenadas
    }
};
robin_hood::unordered_set<std::pair<int,int>, hasher> randomGrid(int rows, int cols, double density = 0.1) {
    robin_hood::unordered_set<std::pair<int,int>, hasher> grid;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(density);

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            if (d(gen))
            {
                grid.insert({ r,c });
            }
        }
    }
    return grid;
}

void updateVertices(sf::VertexArray& vertices, const robin_hood::unordered_set<std::pair<int, int>, hasher>& aliveCells, int cellSize) {
    vertices.clear();
    vertices.resize(aliveCells.size() * 4);
    int index = 0;
    for (const auto& cell : aliveCells) {
        int x = cell.first;
        int y = cell.second;
        vertices[index + 0].position = sf::Vector2f(x * cellSize, y * cellSize);
        vertices[index + 1].position = sf::Vector2f(x * cellSize + cellSize, y * cellSize);
        vertices[index + 2].position = sf::Vector2f(x * cellSize + cellSize, y * cellSize + cellSize);
        vertices[index + 3].position = sf::Vector2f(x * cellSize, y * cellSize + cellSize);
        index += 4;
    }
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Juego de la Vida de Conway");
    int cellSize = 5;
    //int cols = window.getSize().x / cellSize;
    //int rows = window.getSize().y / cellSize;
    int cols = 1500;
    int rows = 1500;

    sf::View view;
    view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
    view.setSize(window.getSize().x, window.getSize().y);
    view.setCenter(view.getSize() / 2.f);

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
    sf::VertexArray cellsVertices(sf::Quads);
    auto lastFrame = chrono::high_resolution_clock::now();

    robin_hood::unordered_set<std::pair<int, int>, hasher> aliveCells;
    robin_hood::unordered_map<std::pair<int, int>,int, hasher> neighborCells;

    robin_hood::unordered_set<std::pair<int, int>, hasher> temporaryAliveCells;

	aliveCells = randomGrid(rows, cols, 0.1);
   
    aliveCells.insert({ 10, 10 });

    int finalAlliveCells = aliveCells.size();
    cellsVertices.clear();
    cellsVertices.resize(finalAlliveCells * 4);
    int index = 0;
    
	updateVertices(cellsVertices, aliveCells, cellSize);
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
        }

        window.clear();
        window.setView(view);

		// DIBUJO DE LAS CELDAS y ACTUALIZACION DE CELDAS
        thisTurn += dt;
        if(thisTurn >= .1f && isPlaying){
            auto aliveCellsStart = chrono::high_resolution_clock::now();

            thisTurn = 0.0f;

			temporaryAliveCells.clear();
            neighborCells.clear();
            for (const pair<int, int>& cell : aliveCells) {
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
						if (dx == 0 && dy == 0) continue; // Skip the cell itself
                       
						neighborCells[{ cell.first + dx, cell.second + dy }]++;
                    

                    }
                }
            }

            for (auto& [cell, neigbhors] : neighborCells) {
                if (aliveCells.contains(cell)) 
                {
                    if (neigbhors == 2 || neigbhors == 3) {
                        temporaryAliveCells.insert(cell);
                    }
                }
                else
                {
                    if (neigbhors == 3) {
                        temporaryAliveCells.insert(cell);
                    }
                }
            }

            aliveCells = temporaryAliveCells;
       		auto aliveCellsEnd = chrono::high_resolution_clock::now();

		    cout << "Tiempo de actualizacion de celdas vivas: " << chrono::duration_cast<chrono::microseconds>(aliveCellsEnd - aliveCellsStart).count() << " microsegundos" << endl;
            auto clearNow = chrono::high_resolution_clock::now();

            updateVertices(cellsVertices, aliveCells, cellSize);

            auto clearEnd = chrono::high_resolution_clock::now();
            std::cout << "Tiempo de limpieza de pantalla: " << chrono::duration_cast<chrono::microseconds>(clearEnd - clearNow).count() << " microsegundos" << endl;
        }
        window.draw(cellsVertices);
		window.setView(window.getDefaultView());

        window.draw(fpsText);

        window.display();
		
        
	}
	return 0;
}
