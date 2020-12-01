#ifndef DISK_H
#define DISK_H
#include <memory>
#include <string>
#include <map>
#include <vector>


using namespace std;

class File;
class Disk {
    private:
        map<string,vector<int>> metadata;
        int meta_data_limit;

    public:
        //Variables and Constructors
        Disk(int meta_data_limit);
        static int total_file_entries;
        static vector<int> free_segments;

        //Getters & Setters
        map<string,vector<int>> get_file_metadata();    
        string set_file_metadata(map<string,vector<int>> data);  
        int move(string source_fname,string target_fname);
        int create(string fname);
        int del(string fname);
        File open(string fname,int mode);
        void memory_map();
        void close(string fname);

};
#endif
