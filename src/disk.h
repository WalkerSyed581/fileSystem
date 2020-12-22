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
       
        int meta_data_limit;

    public:
        //Variables and Constructors
        multimap<int,pair<string,vector<int>>> metadata;
        multimap<int,tuple<string,vector<int>,vector<int>>> dir_metadata;
        Disk(int meta_data_limit);
        vector<int> free_segments;
        string path;
        static int total_folders;
        static int total_files;

        //Utility Functions
        void update_metadata();
        static vector<string> parse_path(string& fname);


        //Getters & Setters
        multimap<int,pair<string,vector<int>>> get_file_metadata();    
        string set_file_metadata(multimap<int,pair<string,vector<int>>> data);  

        multimap<int,tuple<string,vector<int>,vector<int>>> get_dir_metadata();    
        string set_dir_metadata(multimap<int,tuple<string,vector<int>,vector<int>>> data);  

        //To Be Implemented
        int move(string source_fname,string target_fname);

        //Done
        int mkdir(string dirname,int curr_dir);
        multimap<int,tuple<string,vector<int>,vector<int>>>::iterator chdir(string path,int curr_dir);
        int create(string fname,int dir_id);
        int del(string fname,int id,int dir_id);
        File open(string fname,int id);
        void memory_map(int dir_id,int level=1);
        void close(string fname,int id);




};
#endif
