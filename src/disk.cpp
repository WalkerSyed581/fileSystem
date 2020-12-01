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

using namespace std;


/*
File Delimeters:

Metadata Entry: \n 
File Info Element: &
Segment Separator: ,
FileSystem Starts After: |
FilesSeparator: \n*/



map<string,vector<int>> Disk::get_file_metadata(){
    return this->file_info;
}

string Disk::set_file_metadata(map<string,vector<int>> data){
    this->file_info = data;
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
    fstream fin("file_system.txt",ios::in);
    fstream fout;
    string metadata,field;
    int file_count = 0;
    
    for(int i = 0;i < 90;i++){
        free_segments.push_back(i);
    }


    if (fin.fail()) {
        fout.open("file_system.txt"); 
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

int Disk::create(File new_file){
    map<string,vector<int>> metadata = this->get_file_metadata();

 
    fstream fout;
    fout.open("file_system.txt");
    
    string text = new_file.get_data(),new_metadata = "";

    vector<string> splits;
    vector<int> file_segments;

    size_t file_length = text.length();
    if(file_length > 100){
        for (unsigned i = 0; i < file_length; i += 100) {
            if(file_length % 100 != 0 && i > (file_length - 100)){
                splits.push_back(text.substr(i,file_length % 100));
            } else {
                splits.push_back(text.substr(i,100));
            }
            file_segments.push_back(free_segments[0]);
            free_segments.erase(free_segments.begin());
        };
    } else {
        splits.push_back(text);
        file_segments.push_back(free_segments[0]);
        free_segments.erase(free_segments.begin());
    }

    this->free_segments = free_segments;
    metadata.insert(pair<string,vector<int>>(new_file.name,file_segments));

    new_metadata = this->set_file_metadata(metadata);

    fout << new_metadata;

    for (auto i = file_segments.begin(); i != file_segments.end(); ++i){
        fout.seekg(1001 + ((*i) * 101));
        string curr_string  = splits[0];
        curr_string.resize(100);
        fout << (curr_string + '\n');
        splits.erase(splits.begin());
    }


    return 1;
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

    
    this->set_file_metadata(metadata);
}



File Disk::open(string fname,int mode){

}

// Disk::close(string fname){

// }

// Disk::memMap(){
    
// }





