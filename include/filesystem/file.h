
#ifndef FILE_H
#define FILE_H
#include <string>

using namespace std;
class Disk;
class File {
    private:
        string data;
    
    public:
        File(string name);
        string name;
        string get_data();
        void set_data(string data);

        void write_to_file(Disk disk,string text);
        void write_to_file(Disk disk,int write_at,string text);

        string read_from_file();
        string read_from_file_at(int start,int size);

        void move_within_file(int start,int size,int target);

        void truncate_file(int max_size);
};
#endif


