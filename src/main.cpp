#include <cctype>
#include <cstdio>
#include <map>
#include <string>
#include <string_view>
#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "disk.h"
#include "../include/filesystem/file.h"

using namespace std;

int file_is_modified(const char* path, time_t oldMTime) {
    struct stat file_stat;
    int err = stat(path, &file_stat);
    if (err != 0) {
        perror(" [file_is_modified] stat");
        exit(errno);
    }
    return file_stat.st_mtime > oldMTime;
}

int validate_file_name(string& fname){
    if(fname.find('|') != string::npos || 
       fname.find('&') != string::npos || 
       fname.find(',') != string::npos ||
       fname.find('\n') != string::npos || 
       fname.find('\t') != string::npos){
        return 0;
    } else {
        return 1;
    }
}


int Disk::total_files = 0;
int Disk::total_folders = 0;

//Remeber to pass in forward slash when a finding a directory
int main(int arc,char * argv[]){
    Disk filesystem = Disk(1000);
    string sentinel = "0";
    filesystem.memory_map(0,1);
    multimap<int,tuple<string,vector<int>,vector<int>>>::iterator curr_dir = filesystem.chdir("root");
    multimap<int,pair<string,vector<int>>> metadata;
    multimap<int,tuple<string,vector<int>,vector<int>>> dir_metadata;

    while(sentinel != "-1"){
        string fname;

        cout << "\nEnter the number written corresponding to the action to move further"<<endl;
        cout << "Current Path: " << filesystem.path << endl;
        cout << "1\t-> Create File"<<endl;
        cout << "2\t-> Delete File"<<endl;
        cout << "3\t-> Open File"<<endl;
        cout << "4\t-> View Map"<<endl;
        cout << "5\t-> Change Directory"<< " Current: "<< filesystem.path << endl;
        cout << "6\t-> Make Directory"<<endl;
        cout << "-1\t-> Quit"<<endl;
        cout << "Enter an action: ";
        cin.clear();
        getline(cin,sentinel,'\n');
        cin.clear();

        filesystem.update_metadata();
        metadata = filesystem.get_file_metadata();
        dir_metadata = filesystem.get_dir_metadata();

        if(sentinel == "1"){
            //Create File
            cout << "Enter file name (Invalid Characters : |,& or a comma): ";
            cin.clear();
            fflush(stdin);
            getline(cin,fname,'\n');
            if(!validate_file_name(fname)){
                cout << "\nError: Invalid File Name"<<endl;
                continue;
            }
            vector<int> curr_dir_files = get<2>(curr_dir->second);
            multimap<int,pair<string,vector<int>>>::iterator file;
            for(auto i = curr_dir_files.begin();i != curr_dir_files.end();i++){
                file = metadata.find(*i);
                if(file->second.first == fname){
                    cout << "\nError: File name already exists\n";
                } 
            }
            int result = filesystem.create(fname);
            if(result == -1){
                cout <<"\nError: Hard Disk is full\n";
            }
        } else if (sentinel == "2"){
            // Delete File
            cout << "Enter file name (Invalid Characters : |,& or a comma): ";
            cin.clear();
            fflush(stdin);
            getline(cin,fname,'\n');
            if(!validate_file_name(fname)){
                cout << "\nError: Invalid File Name"<<endl;
                continue;
            }
            vector<int> curr_dir_files = get<2>(dir_metadata.find(filesystem.curr_dir)->second);
            multimap<int,pair<string,vector<int>>>::iterator file;
            for(auto i = curr_dir_files.begin();i != curr_dir_files.end();i++){
                file = metadata.find(*i);
                if(file->second.first == fname){
                    break;
                }
            }
            if(file->second.first != fname){
                cout << "\nError: File does not exist\n" << endl;
                continue;
            }
            int result = filesystem.del(fname,file->first);
            if(result == -1){
                cout <<"\nError:  Hard Disk is full\n";
            }
        } else if (sentinel == "3"){
            //Open File
            cout << "Enter file name (Invalid Characters : |,& or a comma): ";
            cin.clear();
            fflush(stdin);
            getline(cin,fname,'\n');
            if(!validate_file_name(fname)){
                cout << "\nError: Invalid File Name"<<endl;
                continue;
            }

            vector<int> curr_dir_files = get<2>(dir_metadata.find(filesystem.curr_dir)->second);
            multimap<int,pair<string,vector<int>>>::iterator file_entry;
            for(auto i = curr_dir_files.begin();i != curr_dir_files.end();i++){
                file_entry = metadata.find(*i);
                if(file_entry->second.first == fname){
                    break;
                }
            }
            if(file_entry->second.first != fname){
                cout << "\nError: File does not exist\n" << endl;
                continue;
            }
            if(file_entry != metadata.end()){
                File file = filesystem.open(fname,file_entry->first);
                string nest_sentinel;
                while(nest_sentinel != "-1"){
                    if(!nest_sentinel.empty()){
                        string yn;
                        cout << "Do you want to continue? (Y/N): "<<endl;
                        cin.clear();
                        fflush(stdin);
                        getline(cin,yn,'\n');
                        cin.clear();
                        if(yn[0] == 'N'){
                            cout << "\n\nClosing File..."<<endl;
                            cout << "\n\n";
                            break;
                        }
                    }
                    cout << "\nEnter the number written corresponding to the action to move further"<<endl;
                    cout << "1 -> Write To File"<<endl;
                    cout << "2 -> Write To File at a specific position"<<endl;
                    cout << "3 -> Read File"<<endl;
                    cout << "4 -> Read File from a specific position"<<endl;
                    cout << "5 -> Truncate File"<<endl;
                    cout << "6 -> Move Within File"<<endl;
                    cout << "-1 -> Quit"<<endl;
                    cin.clear();
                    cout << "Enter an action: ";
                    fflush(stdin);
                    getline(cin,nest_sentinel,'\n');
                    cin.clear();

                    filesystem.update_metadata();
                    metadata = filesystem.get_file_metadata();
                    dir_metadata = filesystem.get_dir_metadata();
                    file.update_file(metadata);
                    if(nest_sentinel == "1"){
                        //Write to file
                        string text;
                        cout << "Enter the text to write into the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,text,'\n');
                        cin.clear();
                        int result = file.write_to_file(filesystem,text);
                        if(result != 0){
                            cout << "\nError: Hard Disk is Full\n";
                            continue;
                        }
                        metadata = filesystem.get_file_metadata();
                    } else if(nest_sentinel == "2"){
                        //Write to file at a position
                        string text,buffer;
                        int pos;
                        cout << "Enter the position to write the text into the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,buffer,'\n');
                        cin.clear();
                        pos = stoi(buffer);
                        if(pos > file.get_data().length()){
                            cout << "\nError: Invalid value\n";
                            continue;
                        }
                        cout << "Enter the text to write into the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,text,'\n');
                        cin.clear();
                        int result = file.write_to_file(filesystem,pos,text);
                        if(result != 0){
                            cout << "\nError: Hard Disk is Full\n";
                            continue;
                        }
                        metadata = filesystem.get_file_metadata();
                    } else if(nest_sentinel == "3"){
                        string content = file.read_from_file();
                        if(content.empty()){
                            cout << "No data found"<<endl;
                        } else {
                            cout <<"\n\nFile Data: " + content + "\n\n"<< endl;
                        }
                    } else if(nest_sentinel == "4"){
                        int start,size;
                        string buffer;
                        string contents = file.get_data();
                        cout << "Enter the position to start reading data from the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,buffer,'\n');
                        start = stoi(buffer);
                        cin.clear();
                        if(start > contents.length()){
                            cout << "\nError: Invalid value\n";
                            continue;
                        }
                        cout << "Enter the size of data  to be read from the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,buffer,'\n');
                        size = stoi(buffer);
                        cin.clear();
                        if(size + start > contents.length()){
                            cout << "\nError: invalid values\n";
                            continue;
                        }

                        string content = file.read_from_file(start,size);
                        if(content.empty()){
                            cout << "No data found"<<endl;
                        } else {
                            cout << "\n\nThe contents are: "<<content<<"\n\n" << endl;
                        }

                    } else if(nest_sentinel == "5"){
                        string buffer;
                        int max_size;
                        cout << "Enter the max size of the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,buffer,'\n');
                        max_size = stoi(buffer);
                        cin.clear();
                        if(max_size > file.get_data().length() || max_size == 0){
                            cout << "\nError: Invalid value\n";
                            continue;
                        }
                        file.truncate_file(filesystem,max_size);
                    } else if(nest_sentinel == "6"){
                        // Move within file
                        int start,size,target;
                        string buffer;
                        string contents = file.get_data();
                        cout << "Enter the position to start reading data from the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,buffer,'\n');
                        start = stoi(buffer);
                        cin.clear();
                        if(start > contents.length()){
                            cout << "\nError: Invalid value\n";
                            continue;
                        }
                        cout << "Enter the size of data  to be read from the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,buffer,'\n');
                        size = stoi(buffer);
                        cin.clear();
                        if(size + start > contents.length()){
                            cout << "\nError: invalid values\n";
                            continue;
                        }

                        cout << "Enter the position to put the data into within the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,buffer,'\n');
                        target = stoi(buffer);
                        cin.clear();
                        if(target + size > contents.length()){
                            cout << "\nError: Invalid value\n";
                            continue;
                        }
                        int result = file.move_within_file(filesystem,start,size,target);
                        if(result == -1 || result == -2){
                            cout << "\nHard Disk Full"<<endl;
                        } 
                    } else if(nest_sentinel == "-1"){
                        cout << "\n\nClosing File..."<<endl;
                        cout << "\n\n";

                        break;
                    }
                }
            } else {
                cout << "\nError: File name not found\n";
                continue;
            }
        } else if (sentinel == "4"){
            //Memory Map
            cout << "\n";
            filesystem.memory_map(curr_dir->first,1);
        } else if(sentinel == "5"){
            //Change Directory
            cout << "Enter the name of the folder (Invalid Characters : |,& or a comma): ";
            cin.clear();
            fflush(stdin);
            getline(cin,fname,'\n');
            if(!validate_file_name(fname)){
                cout << "\nError: Invalid Input\n"<<endl;
                continue;
            }
            multimap<int,tuple<string,vector<int>,vector<int>>>::iterator new_dir = filesystem.chdir(fname);
            if(new_dir == dir_metadata.end()){
                cout << "\nError: Invalid Path\n"<<endl;
                continue;
            } else {
                curr_dir = new_dir;
                filesystem.path = filesystem.path + get<0>(curr_dir->second) + "/";
            }
        } else if(sentinel == "6"){
            //Make New Directory
            cout << "Enter name of the folder (Invalid Characters : |,& or a comma): ";
            cin.clear();
            fflush(stdin);
            getline(cin,fname,'\n');
            if(!validate_file_name(fname)){
                cout << "\nError: Invalid Input\n"<<endl;
                continue;
            }
            int result = filesystem.mkdir(fname);
            if(result == -1){
                cout << "\nError: Folder name already exists\n"<<endl;
            }
        } else if(sentinel == "-1"){
            cout << "\nExiting...\n";
        }
    }
    return 0;
}