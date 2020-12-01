#include <string>
#include <map>
#include <vector>
#include "../include/filesystem/file.h"


using namespace std;


class Disk {
    private:
        map<string,vector<int>> metadata;
        int meta_data_limit;

    public:
        //Variables and Constructors
        Disk(int meta_data_limit);
        static int total_file_entries;
        static vector<int> free_segments;

        map<string,vector<int>> get_file_metadata();    
        string set_file_metadata(map<string,vector<int>> data);  

        int create(File new_file);
        void del(string fname);
        File open(string fname,int mode);
        void memory_map();

};