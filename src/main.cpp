#include <cctype>
#include <cstdio>
#include <map>
#include <string>
#include <string_view>
#include <numeric>
#include <fstream>
#include <thread>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <mutex>
#include <fcntl.h>
#include "disk.h"
#include "../include/filesystem/file.h"

using namespace std;

Disk filesystem = Disk(1000);
string sentinel = "0";
mutex data_lock;

void print_dir_metadata(){
    multimap<int,tuple<string,vector<int>,vector<int>>>dir_metadata = filesystem.dir_metadata;
    for(auto i = dir_metadata.begin();i != dir_metadata.end();i++){
        cout << i->first << " : " << get<0>(i->second) << "&";
        for(int j = 0;j < get<1>(i->second).size();j++){
            cout<<get<1>(i->second)[j] << ",";
        }
        cout << "&";
        for(int j = 0;j < get<2>(i->second).size();j++){
            cout<<get<2>(i->second)[j] << ",";
        }
        cout << "&";
    }
}
void print_metadata(){
    multimap<int,pair<string,vector<int>>>metadata = filesystem.metadata;
    for(auto i = metadata.begin();i != metadata.end();i++){
        cout << i->first << " : " << i->second.first << endl;
    }
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

vector<vector<string>> read_script(string path){
    fstream fin(path,ios::in);
    string entry,temp;
    vector<vector<string>> commands;

    while (!fin.eof()) { 
        getline(fin, entry);
        stringstream buffer(entry);
        string word;
        vector<string> command;
        command.clear();
        while(getline(buffer,word,'|')){
            command.push_back(word);
        }
        commands.push_back(command);
    }   
    fin.close(); 
    return commands;
}

vector<string> parse_command(vector<string> command,int mode = 0){
    vector<string> arguments;
    if(mode == 0){
        if(command[0] == "create"){
            arguments.push_back("1"); 
            arguments.push_back(command[1]);
        } else if(command[0] == "delete"){
            arguments.push_back("2"); 
            arguments.push_back(command[1]);
        } else if(command[0] == "open"){
            arguments.push_back("3");
            arguments.push_back(command[1]);
        } else if(command[0] == "show_map"){
            arguments.push_back("4");
        } else if(command[0] == "chdir"){
            arguments.push_back("5");
            arguments.push_back(command[1]);
        } else if(command[0] == "mkdir"){
            arguments.push_back("6");
            arguments.push_back(command[1]);
        }
    } else if (mode == 1){
        if(command[0] == "write"){
            arguments.push_back("1"); 
            arguments.push_back(command[1]);
            arguments.push_back(command[2]);
        } else if(command[0] == "write_at"){
            arguments.push_back("2"); 
            arguments.push_back(command[1]);
            arguments.push_back(command[2]);
            arguments.push_back(command[3]);
        } else if(command[0] == "read"){
            arguments.push_back("3");
            arguments.push_back(command[1]);
        } else if(command[0] == "read_at"){
            arguments.push_back("4");
            arguments.push_back(command[1]);
            arguments.push_back(command[2]);
            arguments.push_back(command[3]);
        } else if(command[0] == "truncate"){
            arguments.push_back("5");
            arguments.push_back(command[1]);
            arguments.push_back(command[2]);
        } else if(command[0] == "move"){
            arguments.push_back("6");
            arguments.push_back(command[1]);
            arguments.push_back(command[2]);
            arguments.push_back(command[3]);
            arguments.push_back(command[4]);
        } else if(command[0] == "close"){
            arguments.push_back("-1");
            arguments.push_back(command[1]);

        }
    }
    return arguments;
}

string call_file_functions(vector<string> arguments,File& file,bool& is_file_open,int& curr_dir,int mode = 0){
    data_lock.lock();
    filesystem.update_metadata();
    data_lock.unlock();
    curr_dir = filesystem.dir_metadata.find(curr_dir)->first;
    file.update_file(filesystem.metadata);
    if(arguments[0] == "1"){
        //Write to file
        string text;
        if(mode == 1){
            cout << "Enter the text to write into the file: ";
            cin.clear();
            fflush(stdin);
            getline(cin,text,'\n');
            cin.clear();
        } else {
            if(arguments[2].empty()){
                return "\nError: No text found\n";
            } else {
                text = arguments[2];
            }
        }
        data_lock.lock();
        int result = file.write_to_file(filesystem,text);
        data_lock.unlock();
        if(result != 0 && mode == 1){
            cout << "\nError: Hard Disk is Full\n";
            return "";
        } else if(result != 0 && mode != 1){
            return "\nError: Hard Disk is Full\n";
        }
    } else if(arguments[0] == "2"){
        //Write to file at a position
        string text,buffer;
        int pos;

        if(mode == 1){
            cout << "Enter the position to write the text into the file: ";
            cin.clear();
            fflush(stdin);
            getline(cin,buffer,'\n');
            cin.clear();
            pos = stoi(buffer);
            cout << "Enter the text to write into the file: ";
            cin.clear();
            fflush(stdin);
            getline(cin,text,'\n');
            cin.clear();
        } else {
            if(arguments[2].empty() || arguments[3].empty()){
                return "\nError: Data not found\n";
            } else {
                pos = stoi(arguments[2]);
                text = arguments[3];
            }
        }
        
        
        if(pos > file.get_data().length() && mode == 1){
            cout << "\nError: Invalid Value\n";
            return "";
        } else if(pos > file.get_data().length() && mode == 1){
            return "\nError: Invalid Value\n";
        }
        data_lock.lock();
        int result = file.write_to_file(filesystem,pos,text);
        data_lock.unlock();
        if(result != 0 && mode == 1){
            cout << "\nError: Hard Disk is Full\n";
            return "";
        } else if(result != 0 && mode != 0){
            return  "\nError: Hard Disk is Full\n";
        }
    } else if(arguments[0] == "3"){
        string content = file.read_from_file();
        if(content.empty()){
            cout << "Error: No data found"<<endl;
        } else {
            if(mode == 1){
                cout << "\n\n" + file.name + " : " + content + "\n\n\n";
            } else {
                return "\n\n" + file.name + " : " + content + "\n\n\n";
            }
        }
    } else if(arguments[0] == "4"){
        int start,size;
        string buffer;
        string contents = file.get_data();
        if(mode == 1){
            cout << "Enter the position to start reading data from the file: ";
            cin.clear();
            fflush(stdin);
            getline(cin,buffer,'\n');
            start = stoi(buffer);
            cin.clear();
            cout << "Enter the size of data  to be read from the file: ";
            cin.clear();
            fflush(stdin);
            getline(cin,buffer,'\n');
            size = stoi(buffer);
            cin.clear();
        } else {
            if(arguments[2].empty() || arguments[3].empty()){
                return "\nError: Data not found\n";
            } else {
                start = stoi(arguments[2]);
                size = stoi(arguments[3]);
            }
        }
        
        if(start > contents.length() || size + start > contents.length()){
            cout << "\nError: Invalid value\n";
            return "";
        }
        

        string content = file.read_from_file(start,size);
        if(content.empty()){
            cout << "Error: No data found"<<endl;
        } else {
            cout << "\n\nThe contents are: "<<content<<"\n\n" << endl;
        }

    } else if(arguments[0] == "5"){
        string buffer;
        int max_size;
        if(mode == 1){
            cout << "Enter the max size of the file: ";
            cin.clear();
            fflush(stdin);
            getline(cin,buffer,'\n');
            max_size = stoi(buffer);
            cin.clear();
        } else {
            if(arguments[2].empty()){
                return "\nError: Data not found\n";
            } else {
                max_size = stoi(arguments[2]);
            }
        }
        
        if((max_size > file.get_data().length() || max_size == 0) && mode == 1){
            cout << "\nError: Invalid value\n";
            return "";
        } else if((max_size > file.get_data().length() || max_size == 0) && mode != 1){
            return "\nError: Invalid value\n";
        }
        data_lock.lock();
        file.truncate_file(filesystem,max_size);
        data_lock.unlock();
    } else if(arguments[0] == "6"){
        // Move within file
        int start,size,target;
        string buffer;
        string contents = file.get_data();
        if(mode == 1){
            cout << "Enter the position to start reading data from the file: ";
            cin.clear();
            fflush(stdin);
            getline(cin,buffer,'\n');
            start = stoi(buffer);
            cin.clear();
            cout << "Enter the size of data  to be read from the file: ";
            cin.clear();
            fflush(stdin);
            getline(cin,buffer,'\n');
            size = stoi(buffer);
            cin.clear();
            cout << "Enter the position to put the data into within the file: ";
            cin.clear();
            fflush(stdin);
            getline(cin,buffer,'\n');
            target = stoi(buffer);
            cin.clear();
        } else {
            if(arguments[2].empty() || arguments[3].empty() || arguments[4].empty()){
                return "\nError: Data not found\n";
            } else {
                start = stoi(arguments[2]);
                size = stoi(arguments[3]);
                target = stoi(arguments[4]);
            }
        }
        
        if(start > contents.length() || size + start > contents.length() || target + size > contents.length()){
            if(mode == 1){
                cout << "\nError: Invalid value\n";
                return "";
            } else {
                return "\nError: Invalid value\n";
            }
        }
        data_lock.lock();
        int result = file.move_within_file(filesystem,start,size,target);
        data_lock.unlock();
        if(result == -1 || result == -2){
            cout << "\nError: Hard Disk Full"<<endl;
        } 
    } else if(arguments[0] == "-1"){
        if(mode == 1){
            cout << "\n\nClosing File..."<<endl;
            cout << "\n\n";
            return "";

        } else if(mode != 1) {
            is_file_open = false;
            return "";
        }
        
        
    }
    return "";
}

string call_disk_functions(vector<string> arguments,File& file,bool& is_file_open,int& curr_dir,int mode = 0){
    string fname;
    data_lock.lock();
    filesystem.update_metadata();
    data_lock.unlock();
    if(arguments[0] == "1"){
        //Create File
        if(mode == 1){
            cout << "Enter file name (Invalid Characters : |,& or a comma): ";
            cin.clear();
            fflush(stdin);
            getline(cin,fname,'\n');
        } else {
            if(arguments[1].empty()){
                return "\nError: Invalid File Name\n";
            } else {
                fname = arguments[1];
            }
        }
        
        if(!validate_file_name(fname)){
            if(mode == 1){
                cout << "\nError: Invalid File Name"<<endl;
                return "";
            } else {
                return "\nError: Invalid File Name\n";

            }
        }
        vector<int> curr_dir_files = get<2>(filesystem.dir_metadata.find(curr_dir)->second);
        multimap<int,pair<string,vector<int>>>::iterator file_ptr;
        for(auto i = curr_dir_files.begin();i != curr_dir_files.end();i++){
            file_ptr = filesystem.metadata.find(*i);
            if(file_ptr->second.first == fname){
                if(mode == 1){
                    cout << "\nError: File name already exists\n";
                    return "";
                } else {
                    return "\nError: File name already exists\n";
                }
            } 
        }
        data_lock.lock();
        int result = filesystem.create(fname,curr_dir);
        data_lock.unlock();
        if(result == -1){
            if(mode == 1){
                cout <<"\nError: Hard Disk is full\n";
                return "";
            } else {
                return "\nError: Hard Disk is full\n";
            }
        }
    } else if (arguments[0] == "2"){
        // Delete File
        if(mode == 1){
            cout << "Enter file name (Invalid Characters : |,& or a comma): ";
            cin.clear();
            fflush(stdin);
            getline(cin,fname,'\n');
        } else {
            if(arguments[1].empty()){
                return "\nError: Invalid File Name\n";
            } else {
                fname = arguments[1];
            }
        }
        
        if(!validate_file_name(fname)){
            if(mode == 1){
                cout << "\nError: Invalid File Name"<<endl;
                return "";
            } else {
                return "\nError: Invalid File Name\n";
            }
        }
        
        vector<int> curr_dir_files = get<2>(filesystem.dir_metadata.find(curr_dir)->second);
        multimap<int,pair<string,vector<int>>>::iterator file_ptr;
        for(auto i = curr_dir_files.begin();i != curr_dir_files.end();i++){
            file_ptr = filesystem.metadata.find(*i);
            if(file_ptr->second.first == fname){
                break;
            }
        }
        if(file_ptr->second.first != fname){
            if(mode == 1){
                cout << "\nError: File does not exist\n" << endl;
                return "";
            } else {
                return "\nError: File does not exist\n\n";
            }
        }
        data_lock.lock();
        int result = filesystem.del(fname,file_ptr->first,curr_dir);
        data_lock.unlock();
        if(result == -1){
            if(mode == 1){
                cout <<"\nError: Hard Disk is full\n";
                return "";
            } else {
                return "\nError: Hard Disk is full\n";
            }
        }
    } else if (arguments[0] == "3"){
        //Open File
        if(mode == 1){
            cout << "Enter file name (Invalid Characters : |,& or a comma): ";
            cin.clear();
            fflush(stdin);
            getline(cin,fname,'\n');
        } else {
            if(arguments[1].empty()){
                return "\nError: Invalid File Name\n";
            } else {
                fname = arguments[1];
            }
        }
        
        if(!validate_file_name(fname)){
            if(mode == 1){
                cout << "\nError: Invalid File Name"<<endl;
                return "";
            } else {
                return "\nError: Invalid File Name\n";
            }
        }

        vector<int> curr_dir_files = get<2>(filesystem.dir_metadata.find(curr_dir)->second);
        multimap<int,pair<string,vector<int>>>::iterator file_entry;
        for(auto i = curr_dir_files.begin();i != curr_dir_files.end();i++){
            file_entry = filesystem.metadata.find(*i);
            if(file_entry->second.first == fname){
                break;
            }
        }
        if(file_entry->second.first != fname){
            if(mode == 1){
                cout << "\nError: File does not exist\n" << endl;
                return "";
            } else {
                return "\nError: File does not exist\n\n";
            }
        }
        if(file_entry != filesystem.metadata.end() && mode == 1){
            File file = filesystem.open(fname,file_entry->first);
            string nest_sentinel;
            while(nest_sentinel != "-1"){
                if(!nest_sentinel.empty() && mode == 1){
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
                file.update_file(filesystem.metadata);
                vector<string> arguments;

                if(nest_sentinel != "-1"){
                    arguments.clear();
                    arguments.push_back(nest_sentinel);
                    bool bro;
                    call_file_functions(arguments,file,bro,curr_dir,1);
                } else {
                    cout << "\n\nClosing File..."<<endl;
                    cout << "\n\n";
                    break;
                }
                
            }

        } else if(file_entry != filesystem.metadata.end() && mode != 1) {
            file = filesystem.open(fname,file_entry->first);
            is_file_open = true;
            return "";
        } else if(file_entry == filesystem.metadata.end()){
            if(mode == 1){
                cout << "\nError: File name not found\n\n";
                return "";
            } else {
                return "\nError: File name not found\n\n";
            }
        }
    } else if (arguments[0] == "4"){
        //Memory Map
        cout << "\n";
        filesystem.memory_map(curr_dir,1);
    } else if(arguments[0] == "5"){
        //Change Directory
        if(mode == 1){
            cout << "Enter the name of the folder (Invalid Characters : |,& or a comma): ";
            cin.clear();
            fflush(stdin);
            getline(cin,fname,'\n');
        } else {
            if(arguments[1].empty()){
                return "\nError: Invalid File Name\n";
            } else {
                fname = arguments[1];
            }
        }
        
        if(!validate_file_name(fname)){
            if(mode == 1){
                cout << "\nError: Invalid File Name"<<endl;
                return "";
            } else {
                return "\nError: Invalid File Name\n";
            };
        }
        int new_dir = filesystem.chdir(fname,curr_dir);
        if(new_dir == -1){
            if(mode == 1){
                cout << "\nError: Invalid Path\n"<<endl;
                return "";
            } else {
                return "\nError: Invalid Path\n\n";
            }
        } else {
            curr_dir = new_dir;
            filesystem.path = filesystem.path + get<0>(filesystem.dir_metadata.find(curr_dir)->second) + "/";
        }
    } else if(arguments[0] == "6"){
        //Make New Directory
        if(mode == 1){
            cout << "Enter the name of the folder (Invalid Characters : |,& or a comma): ";
            cin.clear();
            fflush(stdin);
            getline(cin,fname,'\n');
        } else {
            if(arguments[1].empty()){
                return "\nError: Invalid File Name\n";
            } else {
                fname = arguments[1];
            }
        }
        
        if(!validate_file_name(fname)){
            if(mode == 1){
                cout << "\nError: Invalid File Name"<<endl;
                return "";
            } else {
                return "\nError: Invalid File Name\n";
            }
        }
        data_lock.lock();
        int result = filesystem.mkdir(fname,curr_dir);
        data_lock.unlock();
        if(result == -1){
            if(mode == 1){
                cout << "\nError: Folder name already exists\n"<<endl;
                return "";
            } else {
                return "\nError: Folder name already exists\n\n";
            }
        }
    } else if(arguments[0] == "-1"){
        cout << "\nExiting...\n";
        return "";
    }
    return "";
}

void process_script(string path){
    vector<vector<string>> commands = read_script(path);
    
    File file;
    bool is_file_open = false;

    int curr_dir = 0;
    for(int i = 0;i < commands.size();i++){
        if(!is_file_open){
            //Execute Disk Functions
            vector<string> arguments = parse_command(commands[i]);
            string result = call_disk_functions(arguments,file,is_file_open,curr_dir);
            if(result != "" && result.find("Error") != string::npos){
                cout << result << endl;
                break;
            } 
        } else {
            //Execute File Functions
            vector<string> arguments = parse_command(commands[i],1);
            string result = call_file_functions(arguments,file,is_file_open,curr_dir);
            if(result != "" && result.find("Error") != string::npos){
                cout << result << endl;
                break;
            } else if(result != "" && result.find("Error") == string::npos){
                cout << result <<endl;
                continue;
            }
        }
    }
    curr_dir = filesystem.chdir("root",0);
    filesystem.memory_map(curr_dir);
}




int Disk::total_files = 0;
int Disk::total_folders = 0;

int main(int argc,char * argv[]){
    if(argc > 1){
        //Create threads if arguments are passed
        int thread_count = argc-1;
        thread threads[thread_count];

        for(int i = 0;i < thread_count;i++){
            string path = argv[i + 1];
            cout << path;
            threads[i] = thread(process_script,path);
        }

        for(int i = 0;i < thread_count;i++){
            threads[i].join();
        }
    } else {
        vector<string> arguments;
        File file;
        bool is_file_open;
        int curr_dir = 0;

        while(sentinel != "-1"){

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


            if(sentinel != "-1"){
                
                arguments.clear();
                arguments.push_back(sentinel);
                call_disk_functions(arguments,file,is_file_open,curr_dir,1);
            } else{
                cout << "\nExiting...\n";
            }

            
        }
    }
    return 0;
}
