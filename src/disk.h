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
        multimap<string,pair<vector<string>,vector<int>>> dir_metadata;
        int meta_data_limit;

    public:
        //Variables and Constructors
        Disk(int meta_data_limit);
        vector<int> free_segments;
        string path;



        //Getters & Setters
        multimap<int,pair<string,vector<int>>> get_file_metadata();    
        string set_file_metadata(multimap<int,pair<string,vector<int>>> data);  

        multimap<string,pair<vector<string>,vector<int>>> get_dir_metadata();    
        string set_dir_metadata(multimap<string,pair<vector<string>,vector<int>>> data);  

        //To Be Implemented
        int move(string source_fname,string target_fname);

        //Done
        int mkdir(string dirname);
        multimap<string,pair<vector<string>,vector<int>>>::iterator chdir(string path);
        int create(string fname);
        int del(string fname,int id);
        File open(string fname,int id);
        void memory_map(string path,int level=1);
        void close(string fname,int id);

        static int total_files;
        static vector<string> parse_path(string& fname);
        static  multimap<string,pair<vector<string>,vector<int>>>::iterator
         find_dir(multimap<string,pair<vector<string>,vector<int>>>& dir_metadata,vector<string>& dir_list);


};
#endif
