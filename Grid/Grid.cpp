#include <Grid.h>
#include <vector>
#include <cstdint>

using namespace std;
Grid::Grid(int rows, int cols) : rows(rows), cols(cols) {

	grid = vector<uint64_t>(rows * cols, 0);;
}

Grid::Grid(int r, int c, vector<uint64_t> grid) : rows(r), cols(c), grid(grid) {

}