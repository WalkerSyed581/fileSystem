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
        multimap<string,vector<int>> metadata;
        multimap<string,pair<vector<string>,vector<int>>> dir_metadata;
        int meta_data_limit;

    public:
        //Variables and Constructors
        Disk(int meta_data_limit);
        vector<int> free_segments;

        int total_files;

        //Getters & Setters
        multimap<string,vector<int>> get_file_metadata();    
        string set_file_metadata(multimap<string,vector<int>> data);  

        multimap<string,pair<vector<string>,vector<int>>> get_dir_metadata();    
        string set_dir_metadata(multimap<string,pair<vector<string>,vector<int>>> data);  

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
