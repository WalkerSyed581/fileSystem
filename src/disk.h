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
        multimap<int,pair<string,vector<int>>> metadata;
        multimap<int,tuple<string,vector<int>,vector<int>>> dir_metadata;
        int meta_data_limit;

    public:
        //Variables and Constructors
        Disk(int meta_data_limit);
        vector<int> free_segments;
        string path;
        int curr_dir;
        static int total_folders;
        static int total_files;



        //Getters & Setters
        multimap<int,pair<string,vector<int>>> get_file_metadata();    
        string set_file_metadata(multimap<int,pair<string,vector<int>>> data);  

        multimap<int,tuple<string,vector<int>,vector<int>>> get_dir_metadata();    
        string set_dir_metadata(multimap<int,tuple<string,vector<int>,vector<int>>> data);  

        //To Be Implemented
        int move(string source_fname,string target_fname);

        //Done
        int mkdir(string dirname);
        multimap<int,tuple<string,vector<int>,vector<int>>>::iterator chdir(string path);
        int create(string fname);
        int del(string fname,int id);
        File open(string fname,int id);
        void memory_map(int dir_id,int level=1);
        void close(string fname,int id);

        static vector<string> parse_path(string& fname);


};
#endif
