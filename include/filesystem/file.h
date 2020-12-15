
#ifndef FILE_H
#define FILE_H
#include <string>

using namespace std;
class Disk;
class File {
    private:
        string data;
    
    public:
        File(string name,int id);
        string name;
        int id;
        
        string get_data();
        void set_data(string data);

        int write_to_file(Disk& disk,string text);
        int write_to_file(Disk& disk,int write_at,string text);

        string read_from_file();
        string read_from_file(int start,int size);

        void move_within_file(Disk& disk,int start,int size,int target);

        int truncate_file(Disk& disk,int max_size);


};
#endif


