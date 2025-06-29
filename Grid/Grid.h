#include <cstdint>
#include <vector>

using namespace std;

class Grid {
public:
	vector<uint64_t> grid; // 16-bit unsigned integers
	int rows, cols; // Dimensions of the grid
	Grid(int r, int c);
	Grid(int r, int c, vector<uint64_t> grid);
};
