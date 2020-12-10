
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


map<string,pair<vector<string>,vector<int>>> Disk::get_dir_metadata(){
    return this->dir_metadata;
}   


string Disk::set_dir_metadata(map<string,pair<vector<string>,vector<int>>> data){
    string new_dir_metadata = "";
    map<string,pair<vector<string>,vector<int>>>::iterator itr; 
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



map<string,vector<int>> Disk::get_file_metadata(){
    return this->metadata;
}

string Disk::set_file_metadata(map<string,vector<int>> data){
    string new_metadata = "";
    map<string, vector<int>>::iterator itr; 
    for (itr = data.begin(); itr != data.end(); ++itr) { 
        string sfile_segments;
        ostringstream out;

        if (!itr->second.empty())
        {
            copy(itr->second.begin(), itr->second.end() - 1,ostream_iterator<int>(out, ","));
            out << itr->second.back();
        }


        new_metadata += (itr->first +"&" +out.str()+",&\n");
    }
    if(new_metadata.length() > this->meta_data_limit){
        return "-1";
    }
    this->metadata = data;
    new_metadata.push_back('|');

    new_metadata.resize(1001);

    return new_metadata;
}

//Done not tested
Disk::Disk(int meta_data_limit){
    this->meta_data_limit = meta_data_limit;
    map<string,vector<int>> data;
    map<string,pair<vector<string>,vector<int>>> dir_metadata;
    vector<int> occupied_segments,free_segments,total_segments;
    fstream fin("../file_system.txt",ios::in);
    ofstream fout;
    string metadata,field,s_dir_metadata;
    int file_count = 0;
    
    for(int i = 0;i < 90;i++){
        total_segments.push_back(i);
    }


    if (fin.fail()) {

        
        fout.open("../file_system.txt",ios::app); 
        this->set_file_metadata(data);
        this->dir_metadata = dir_metadata;
        this->free_segments = total_segments;

    } else  {
        getline(fin,metadata,'|');

        stringstream buffer(metadata);

        while(getline(buffer, field, '\n') && metadata.length() > 0){
            vector<int> segment_array = {};

            string entry ="",segments="";

            file_count += 1;

            stringstream field_buffer(field);

            getline(field_buffer,entry,'&');

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
            

            data.insert(pair<string,vector<int>>(entry,segment_array));
        }

        
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






        sort(total_segments.begin(), total_segments.end());
        sort(occupied_segments.begin(), occupied_segments.end());

        set_symmetric_difference(total_segments.begin(), total_segments.end(), 
                                occupied_segments.begin(), occupied_segments.end(), 
                                    back_inserter(free_segments));
        
        cout << "First Free Segment: " << *(free_segments.begin()) << endl;
                            

        sort(free_segments.begin(), free_segments.end()); 
        this->free_segments = free_segments;
        this->metadata = data;
        this->dir_metadata = dir_metadata;
        // string new_metadata = this->set_file_metadata(data);
        // if(new_metadata != "-1" && metadata.empty() != 0){
        //     fstream fout;
        //     fout.open("../file_system.txt");

        //     fout << new_metadata;
        // }
    }
}

int Disk::create(string fname){
    map<string,vector<int>> metadata = this->get_file_metadata();
 
    fstream fout;
    fout.open("../file_system.txt");
    
    string new_metadata = "";
    vector<int> file_segments;

    metadata.insert(pair<string,vector<int>>(fname,file_segments));

    new_metadata = this->set_file_metadata(metadata);
    if(new_metadata != "-1"){
        fout << new_metadata;
        return 0;
    } else {
        return -1;
    }
}

int Disk::del(string fname){
    map<string,vector<int>> metadata = this->get_file_metadata();
    vector<int> free_segments = this->free_segments;
    auto file = metadata.find(fname);
    vector<int> file_segments;

    if(file != metadata.end()){
        file_segments = file->second;
    }

    free_segments.insert( free_segments.end(), file_segments.begin(), file_segments.end() );
    sort(free_segments.begin(), free_segments.end()); 

    metadata.erase(fname);

    
    string new_metadata = this->set_file_metadata(metadata);

    if(new_metadata != "-1"){
        fstream fout;
        fout.open("../file_system.txt");

        fout << new_metadata;
        return 0;
    } else {
        return -1;
    }
}



File Disk::open(string fname){
    auto file_data = this->metadata.find(fname);
    vector<int> file_segments;
    if(file_data != this->metadata.end()){
        file_segments = file_data->second;
    }

    fstream fin("../file_system.txt",ios::in);

    string contents = "";

    for (auto i = file_segments.begin(); i != file_segments.end(); ++i){
        fin.seekg(1001 + ((*i) * 101));
        string buffer = "";
        getline(fin,buffer,'\n');
        contents += buffer;
    }
    
    File req_file = File(fname);
    req_file.set_data(contents);

    return req_file;

}

void Disk::close(string fname){
    cout << "Closing File: " + fname;
}



void Disk::memory_map(){
    if(!metadata.empty()){
        map<string, vector<int>>::iterator itr; 
        for (itr = this->metadata.begin(); itr != this->metadata.end(); ++itr) { 
            ostringstream out;

            if (!itr->second.empty()){
                copy(itr->second.begin(), itr->second.end() - 1,ostream_iterator<int>(out, ","));
                out << itr->second.back();
            }

            cout << "|\n|---- " + itr->first + ", Segments -> " + out.str() + "\n|";
        } 
    } else {
        cout << "No file found"<<endl;
    }
}





