#include "avlbst.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

using namespace std;

struct Rectangle {
    int ID;
    int length;
    int height;
};

// Typedefs for you input and output map. Start with
// std::map but once the backtracking search algorithm is working
// try to use your AVL-tree map implementation by changing these
// typedef's
typedef AVLTree<int, Rectangle> InputMapType;
typedef AVLTree<int, std::pair<int, int>> OutputMapType;

// Allowed global variables: the dimensions of the grid
int n;  // X-dim size
int m;  // Y-dim size

// No other global variables are allowed

// A dummy operator << for Rectangles so the BST and AVL BST will
// compile with their printRoot() implementations
std::ostream& operator<<(std::ostream& os, const Rectangle& r) {
    os << r.ID;
    return os;
}

// A dummy operator << for pairs so the BST and AVL BST will
// compile with their printRoot() implementations
template<typename T, typename U>
std::ostream& operator<<(std::ostream& os, const std::pair<T, U>& p) {
    os << p.first << "," << p.second;
    return os;
}

void printSolution(std::ostream& os, InputMapType& input, OutputMapType& output) {
    for (OutputMapType::iterator it = output.begin(); it != output.end(); ++it) {
        InputMapType::iterator rbit = input.find(it->first);
        os << it->first << " ";
        os << it->second.first << " ";
        os << it->second.second << " ";
        os << rbit->second.length << " ";
        os << rbit->second.height << endl;
    }
}

// Changes the grid entries to their opposite values for the
// rectangle r starting at x1,y1
void flip(int x1, int y1, const Rectangle& r, vector<vector<bool>>& grid) {
    for (int x = x1; x < x1 + r.length; x++) {
        for (int y = y1; y < y1 + r.height; y++)
            grid[x][y] = !grid[x][y];
    }
}

// TODO: Write your backtracking search function here

// a helper function to check if such layout is safe
bool isSafe(int x, int y, int length, int height, vector<vector<bool>> grid) {

    for (int i = x; i < x + length; i++) {
        for (int j = y; j < y + height; j++) {

            if (i >= n || j >= m)
                return false;

            if (grid[i][j])
                return false;
        }
    }
    return true;
}

// mark grid
void markGrid(int x, int y, int length, int height, vector<vector<bool>>& grid) {

    for (int i = x; i < x + length; i++) {
        for (int j = y; j < y + height; j++) {

            grid[i][j] = true;
        }
    }
}

// unmark grid
void unmarkGrid(int x, int y, int length, int height, vector<vector<bool>>& grid) {

    for (int i = x; i < x + length; i++) {
        for (int j = y; j < y + height; j++) {
            grid[i][j] = false;
        }
    }
}

bool search(InputMapType::iterator it, InputMapType::iterator end, vector<vector<bool>> grid, OutputMapType& out) {

    // if reach the end return true
    if (it == end) {
        return true;
    }

    if (it->second.length > n || it->second.height > m)
        return false;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            // check if current location is taken
            if (!grid[i][j]) {

                // check if it is safe
                if (isSafe(i, j, it->second.length, it->second.height, grid)) {

                    markGrid(i, j, it->second.length, it->second.height, grid);
                    out.insert(std::make_pair(it->first, std::make_pair(i, j)));
                    InputMapType::iterator it_copy = it;
                    ++it;

                    // recursive call to allocate next rectangle
                    if (search(it, end, grid, out))
                        return true;
                    else {

                        // flip
                        it = it_copy;
                        if (isSafe(i, j, it->second.height, it->second.length, grid)) {

                            flip(i, j, it->second, grid);
                            int temp = it->second.length;
                            it->second.length = it->second.height;
                            it->second.height = temp;
                            out.insert(std::make_pair(it->first, std::make_pair(i, j)));
                            ++it;
                            if (search(it, end, grid, out))
                                return true;

                            it = it_copy;
                        }
                    }

                    // backtracking
                    unmarkGrid(i, j, it->second.length, it->second.height, grid);
                    out.remove(it->first);
                } else {

                    // check if flip version is safe
                    if (isSafe(i, j, it->second.height, it->second.length, grid)) {

                        int temp = it->second.length;
                        it->second.length = it->second.height;
                        it->second.height = temp;
                        markGrid(i, j, it->second.length, it->second.height, grid);

                        out.insert(std::make_pair(it->first, std::make_pair(i, j)));
                        InputMapType::iterator it_copy = it;
                        ++it;

                        if (search(it, end, grid, out))
                            return true;

                        // backtracking
                        it = it_copy;
                        unmarkGrid(i, j, it->second.length, it->second.height, grid);
                        out.remove(it->first);
                    }
                }
            }
        }
    }

    return false;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "please specify an input and output file";
        return 0;
    }
    ifstream ifile(argv[1]);
    stringstream ss;
    string line;
    ofstream ofile(argv[2]);
    int x;
    getline(ifile, line);
    ss << line;
    ss >> n;
    ss >> m;
    ss >> x;

    InputMapType input;
    OutputMapType output;
    for (int i = 0; i < x; i++) {
        getline(ifile, line);
        stringstream ss2(line);
        Rectangle r;
        ss2 >> r.ID;
        ss2 >> r.length;
        ss2 >> r.height;
        input.insert(std::make_pair(r.ID, r));
    }
    ifile.close();
    vector<vector<bool>> grid;

    for (int i = 0; i < n; i++) {
        grid.push_back(vector<bool>(m, false));
    }
    InputMapType::iterator it = input.begin();
    bool solution_exists = false;

    // TODO:  Call your backtracking search function here
    int total_area = 0;
    InputMapType::iterator area_it = input.begin();
    while (area_it != input.end()) {
        total_area += (area_it->second.length * area_it->second.height);
        ++area_it;
    }
    if (total_area <= (n * m)) {
        solution_exists = search(it, input.end(), grid, output);
    }

    if (!solution_exists) {
        ofile << "No solution found.";
    } else {
        printSolution(ofile, input, output);
    }
    ofile.close();
    return 0;
}
