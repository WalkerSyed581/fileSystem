
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <iterator>
#include <map>
#include <fstream> 
#include <sstream>
#include <vector>

#include "disk.h"
#include "../include/filesystem/file.h"

using namespace std;


/*
File Delimeters:

Metadata Entry: \n 
File Info Element: &
Segment Separator: ,
FileSystem Starts After: |
FilesSeparator: \n*/


multimap<string,pair<vector<string>,vector<int>>> Disk::get_dir_metadata(){
    return this->dir_metadata;
}   


string Disk::set_dir_metadata(multimap<string,pair<vector<string>,vector<int>>> data){
    string new_dir_metadata = "";
    multimap<string,pair<vector<string>,vector<int>>>::iterator itr; 
    for (itr = data.begin(); itr != data.end(); ++itr) { 
        string sfile_segments;
        ostringstream out;
        vector<string> dirs;
        vector<int> files;


        if (!itr->second.first.empty()){
            dirs = itr->second.first;
            copy(dirs.begin(), dirs.end() - 1,ostream_iterator<int>(out, ","));
            out << dirs.back();
        }

        out << ",&";

        if(!itr->second.second.empty()){
            files = itr->second.second;
            copy(files.begin(), files.end() - 1,ostream_iterator<int>(out, ","));
            out << files.back();
        }


        new_dir_metadata += (itr->first +"&" +out.str()+",&\n");
    }
    if(new_dir_metadata.length() > this->meta_data_limit){
        return "-1";
    }
    this->dir_metadata = data;
    new_dir_metadata.push_back('|');

    new_dir_metadata.resize(1001);

    return new_dir_metadata;
}



multimap<int,pair<string,vector<int>>> Disk::get_file_metadata(){
    return this->metadata;
}

string Disk::set_file_metadata(multimap<int,pair<string,vector<int>>> data){
    string new_metadata = "";
    multimap<int,pair<string,vector<int>>>::iterator itr; 
    for (itr = data.begin(); itr != data.end(); ++itr) { 
        string sfile_segments;
        ostringstream out;

        if (!itr->second.second.empty())
        {
            copy(itr->second.second.begin(), itr->second.second.end() - 1,ostream_iterator<int>(out, ","));
            out << itr->second.second.back();
        }


        new_metadata += (itr->first +"&"  + itr->second.first + "&" +out.str()+",&\n");
    }
    if(new_metadata.length() > this->meta_data_limit){
        return "-1";
    }
    this->metadata = data;
    new_metadata.push_back('|');

    new_metadata.resize(1001);

    return new_metadata;
}

vector<string> Disk::parse_path(string& fname){
    size_t pos = 0;
    vector<string> dir_list;
    string token;
    string delimiter = "/";
    while ((pos = fname.find(delimiter)) != string::npos) {
        token = fname.substr(0, pos);
        if(token.empty()){
            break;
        }
        dir_list.push_back(token);
        fname.erase(0,pos + delimiter.size());
    }

    return dir_list;
}


 multimap<string,pair<vector<string>,vector<int>>>::iterator
Disk::find_dir(multimap<string,pair<vector<string>,vector<int>>>& dir_metadata,vector<string>& dir_list){
    
    multimap<string,pair<vector<string>,vector<int>>>::iterator dir = dir_metadata.begin();
    for(auto i = dir_list.begin();i != dir_list.end();++i){
        auto cur_dir = find(dir,dir_metadata.end(),*i);
        if(i+1 != dir_list.end() &&
            find(cur_dir->second.first.begin(),cur_dir->second.first.end(),*(i+1)) != cur_dir->second.first.end()){
            dir = find(dir,dir_metadata.end(),*(i+1));
        } else {
            dir = cur_dir;
            break;
        }
    }
        
    return dir;      
}


//Done not tested
Disk::Disk(int meta_data_limit){
    this->meta_data_limit = meta_data_limit;
    multimap<int,pair<string,vector<int>>> data;
    multimap<string,pair<vector<string>,vector<int>>> dir_metadata;
    vector<int> occupied_segments,free_segments,total_segments;
    fstream fin("../file_system.txt",ios::in);
    ofstream fout;
    string metadata,field,s_dir_metadata;
    int file_count = 0;
    
    for(int i = 0;i < 144;i++){
        total_segments.push_back(i);
    }


    if (fin.fail()) {

        vector<int> root_files;
        vector<string> root_dirs;
        fout.open("../file_system.txt",ios::app); 
        this->set_file_metadata(data);
        dir_metadata.insert(pair<string,pair<vector<string>,vector<int>>>("root",pair<vector<string>,vector<int>>(root_dirs,root_files)));
        this->set_dir_metadata(dir_metadata);
        this->total_files = 0;
        this->free_segments = total_segments;
        this->path = "root/";

    } else  {

        //Attempting to read the file metadata
        getline(fin,metadata,'|');

        stringstream buffer(metadata);

        while(getline(buffer, field, '\n') && metadata.length() > 0){
            vector<int> segment_array = {};

            string id ="",name="",segments="";

            file_count += 1;

            stringstream field_buffer(field);

            getline(field_buffer,id,'&');

            getline(field_buffer,name,'&');

            getline(field_buffer,segments,'&');


            size_t pos = 0;
            string token;
            string delimiter = ",";
            while ((pos = segments.find(delimiter)) != string::npos) {
                token = segments.substr(0, pos);
                if(token.empty()){
                    break;
                }
                segment_array.push_back(stoi(token));
                occupied_segments.push_back(stoi(token));
                segments.erase(0, pos + delimiter.length());

            }
            
            pair<string,vector<int>> value(name,segment_array);
            data.insert(pair<int,pair<string,vector<int>>>(stoi(id),value));
        }

        //Attempting to read the folder metadata        
        getline(fin,s_dir_metadata,'|');

        stringstream dir_buffer(s_dir_metadata);

        while(getline(dir_buffer, field, '\n') && metadata.length() > 0){
            vector<int> file_array = {};
            vector<string> dir_array = {};

            string entry ="",dirs="",files="";


            stringstream field_buffer(field);

            getline(field_buffer,entry,'&');

            getline(field_buffer,dirs,'&');

            getline(field_buffer,files,'&');


            size_t pos = 0;
            string token;
            string delimiter = ",";
            while ((pos = dirs.find(delimiter)) != string::npos) {
                token = dirs.substr(0, pos);
                if(token.empty()){
                    break;
                }
                dir_array.push_back(token);
                dirs.erase(0, pos + delimiter.length());
            }

            while ((pos = files.find(delimiter)) != string::npos) {
                token = files.substr(0, pos);
                if(token.empty()){
                    break;
                }
                file_array.push_back(stoi(token));
                dirs.erase(0, pos + delimiter.length());
            }
            
            pair<vector<string>,vector<int>> value(dir_array,file_array);
            dir_metadata.insert(pair<string,pair<vector<string>,vector<int>>>(entry,value));
        }





        //Tinkering out the free segments from the occupied one
        sort(total_segments.begin(), total_segments.end());
        sort(occupied_segments.begin(), occupied_segments.end());

        set_symmetric_difference(total_segments.begin(), total_segments.end(), 
                                occupied_segments.begin(), occupied_segments.end(), 
                                    back_inserter(free_segments));
        sort(free_segments.begin(), free_segments.end()); 

        //Setting values
        this->free_segments = free_segments;
        this->metadata = data;
        this->dir_metadata = dir_metadata;
        this->total_files = 0;


        // Commented out writing back to the file
        // string new_metadata = this->set_file_metadata(data);
        // if(new_metadata != "-1" && metadata.empty() != 0){
        //     fstream fout;
        //     fout.open("../file_system.txt");

        //     fout << new_metadata;
        // }
    }
}

int Disk::create(string fname){
    multimap<int,pair<string,vector<int>>> metadata = this->get_file_metadata();
    multimap<string,pair<vector<string>,vector<int>>> dir_metadata =this->get_dir_metadata();

    
    vector<string> dir_list = this->parse_path(fname);
    

 
    fstream fout;
    fout.open("../file_system.txt");
    
    string new_metadata = "";
    vector<int> file_segments;

    this->total_files += 1;


    file_segments.push_back(total_files);

    pair<string,vector<int>> value(fname,file_segments);
    metadata.insert(pair<int,pair<string,vector<int>>>(total_files,value));

    new_metadata = this->set_file_metadata(metadata);

    if(new_metadata != "-1"){
        fout << new_metadata;
        map<string,pair<vector<string>,vector<int>>>::iterator dir = find_dir(dir_metadata,dir_list);

        dir->second.second.push_back(total_files);

        string s_dir_metadata = this->set_dir_metadata(dir_metadata);

        fout.seekg(1001);
        fout << s_dir_metadata;


        return 0;
    } else {
        --total_files;

        return -1;
    }
}

int Disk::del(string fname,int id){
    multimap<int,pair<string,vector<int>>> metadata = this->get_file_metadata();
    multimap<string,pair<vector<string>,vector<int>>> dir_metadata =this->get_dir_metadata();


    vector<string> dir_list = this->parse_path(fname);
    

    vector<int> free_segments = this->free_segments;
    auto file = metadata.find(id);
    vector<int> file_segments;

    if(file != metadata.end()){
        file_segments = file->second.second;
    }

    free_segments.insert( free_segments.end(), file_segments.begin(), file_segments.end() );
    sort(free_segments.begin(), free_segments.end()); 

    metadata.erase(file);

    
    string new_metadata = this->set_file_metadata(metadata);

    if(new_metadata != "-1"){
        fstream fout;
        fout.open("../file_system.txt");


        fout << new_metadata;

        map<string,pair<vector<string>,vector<int>>>::iterator dir = find_dir(dir_metadata,dir_list);


        
        string s_dir_metadata = this->set_dir_metadata(dir_metadata);

        fout.seekg(1001);
        fout << s_dir_metadata;

        return 0;
    } else {
        return -1;
    }
}



File Disk::open(string fname,int id){
    auto file_data = this->metadata.find(id);
    vector<int> file_segments;
    if(file_data != this->metadata.end()){
        file_segments = file_data->second.second;
    }

    fstream fin("../file_system.txt",ios::in);

    string contents = "";

    for (auto i = file_segments.begin(); i != file_segments.end(); ++i){
        fin.seekg(1001 + ((*i) * 101));
        string buffer = "";
        getline(fin,buffer,'\n');
        contents += buffer;
    }
    
    File req_file = File(fname,id);
    req_file.set_data(contents);

    return req_file;

}

void Disk::close(string fname,int id){
    cout << "Closing File: " + fname;
}



void Disk::memory_map(string path,int level){
    if(!metadata.empty()){
        multimap<int,pair<string,vector<int>>>::iterator itr;
        string path_ = path;
        vector<string> dir_list = this->parse_path(path); 
        auto curr_dir = find_dir(this->dir_metadata, dir_list);
        
        cout<< "|\n|----- ";
        for(int i = 1;i < level;i++){
            cout << "\t";
        }
        cout << curr_dir->first << "\n|";

        
        for(auto i = dir_list.begin();i != dir_list.end() && !curr_dir->second.second.empty(); i++){
            this->memory_map(path_ + "/" + *i,level + 1);
        }
        for (itr = this->metadata.begin(); itr != this->metadata.end(); ++itr) { 
            ostringstream out;

            if (!itr->second.second.empty()){
                copy(itr->second.second.begin(), itr->second.second.end() - 1,ostream_iterator<int>(out, ","));
                out << itr->second.second.back();
            }

            cout << "|\n|---- ";
            for(int i = 0;i < level;i++){
                cout << "\t";
            }
            cout << itr->second.first + ", Segments -> " + out.str() + "\n|";
        } 
    } else {
        cout << "No file found"<<endl;
    }
}

int Disk::mkdir(string dir_name){
    multimap<string,pair<vector<string>,vector<int>>> dir_metadata = this->dir_metadata;
    vector<string> dir_list = parse_path(dir_name);
    auto dir = find_dir(dir_metadata,dir_list);

    //Checking for a duplicate folder name
    for(auto i = dir->second.first.begin(); i != dir->second.first.end(); i++){
        if(*i == dir_name){
            return -1;
        }   
    }

    dir->second.first.push_back(dir_name);
    this->set_dir_metadata(dir_metadata);
    return 0;
}

//Opening a directory 
multimap<string,pair<vector<string>,vector<int>>>::iterator Disk::chdir(string dir_name){
    //Finding the directory
    multimap<string,pair<vector<string>,vector<int>>> dir_metadata = this->dir_metadata;
    vector<string> dir_list = parse_path(dir_name);
    multimap<string,pair<vector<string>,vector<int>>>::iterator dir = find_dir(dir_metadata,dir_list);

    return dir;
    
}




