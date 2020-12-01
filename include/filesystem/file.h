#include <string>

using namespace std;

class File {
    private:
        string data;
    
    public:
        File(string data,string name);
        string name;
        string get_data();
};