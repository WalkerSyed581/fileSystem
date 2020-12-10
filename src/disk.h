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
        map<string,pair<vector<string>,vector<int>>> dir_metadata;
        int meta_data_limit;

    public:
        //Variables and Constructors
        Disk(int meta_data_limit);
        vector<int> free_segments;

        //Getters & Setters
        map<string,vector<int>> get_file_metadata();    
        string set_file_metadata(map<string,vector<int>> data);  

        map<string,pair<vector<string>,vector<int>>> get_dir_metadata();    
        string set_dir_metadata(map<string,pair<vector<string>,vector<int>>> data);  

        //To Be Implemented
        void mkdir(string dirname);
        void chdir(string path);
        int move(string source_fname,string target_fname);

        //Done
        int create(string fname);
        int del(string fname);
        File open(string fname);
        void memory_map();
        void close(string fname);

};
#endif
