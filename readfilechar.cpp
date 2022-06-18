
#include <iostream>
#include <fstream>
#include <vector>

using std::cout; using std::cerr;
using std::endl; using std::string;
using std::ifstream; using std::vector;

int main()
{
    string filename("2parcial/exa1.c");
    vector<char> bytes;
    char byte = 0;

    ifstream input_file(filename);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << filename << "'" << endl;
        return EXIT_FAILURE;
    }

    while (input_file.get(byte)) {
        bytes.push_back(byte);
    }

    for(int i=0; i<bytes.size(); i++ ){
        cout << bytes[i];
    }
    cout << endl;

    input_file.close();

    return EXIT_SUCCESS;
}