
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



map<string,vector<int>> Disk::get_file_metadata(){
    return this->metadata;
}

string Disk::set_file_metadata(map<string,vector<int>> data){
    this->metadata = data;
    string new_metadata = "";
    map<string, vector<int>>::iterator itr; 
    for (itr = data.begin(); itr != data.end(); ++itr) { 
        string sfile_segments;
        ostringstream out;

        if (!itr->second.empty())
        {
            copy(itr->second.begin(), itr->second.end() - 1,ostream_iterator<int>(out, ";"));
            out << itr->second.back();
        }


        new_metadata += (itr->first +"&" +out.str()+"\n");
    }
    new_metadata.resize(1001);
    new_metadata[1000] = '|';

    return new_metadata;
}

//Done not tested
Disk::Disk(int meta_data_limit){
    this->meta_data_limit = meta_data_limit;
    map<string,vector<int>> data;
    vector<int> hello;
    fstream fin("../file_system.txt",ios::in);
    fstream fout;
    string metadata,field;
    int file_count = 0;
    
    for(int i = 0;i < 90;i++){
        free_segments.push_back(i);
    }


    if (fin.fail()) {
        fout.open("../file_system.txt"); 
        this->set_file_metadata(data);
    } else {

        getline(fin,metadata,'|');

        stringstream buffer(metadata);
        vector<int> segment_array;


        while(getline(buffer, field, '\n')){

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
                segment_array.push_back(stoi(token));
                free_segments.erase(free_segments.begin()+stoi(token));
                segments.erase(0, pos + delimiter.length());
            }
            segment_array.push_back(stoi(segments));
            


            data.insert(pair<string,vector<int>>(entry,segment_array));
        }

        sort(free_segments.begin(), free_segments.end()); 
        this->free_segments = free_segments;
        this->total_file_entries = file_count;
        this->set_file_metadata(data);


    }
}

void Disk::create(string fname){
    map<string,vector<int>> metadata = this->get_file_metadata();
 
    fstream fout;
    fout.open("../file_system.txt");
    
    string new_metadata = "";
    vector<int> file_segments;

    metadata.insert(pair<string,vector<int>>(fname,file_segments));

    new_metadata = this->set_file_metadata(metadata);

    fout << new_metadata;
}

void Disk::del(string fname){
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

    fstream fout;
    fout.open("../file_system.txt");

    fout << new_metadata;
}

File Disk::open(string fname,int mode){
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

}

void Disk::memory_map(){

    map<string, vector<int>>::iterator itr; 
    for (itr = this->metadata.begin(); itr != this->metadata.end(); ++itr) { 
        ostringstream out;

        if (!itr->second.empty()){
            copy(itr->second.begin(), itr->second.end() - 1,ostream_iterator<int>(out, ";"));
            out << itr->second.back();
        }

        cout << "|\n|---- " + itr->first + ", Segments -> " + out.str() + "\n";
    } 
}





