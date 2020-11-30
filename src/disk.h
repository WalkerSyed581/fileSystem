#include <string>
#include <map>
#include <vector>


using namespace std;

class Disk {
    private:
        map<string,vector<int>> file_info;
        int meta_data_limit;
        static int seg_sequence;

    public:
        //Variables and Constructors
        Disk(int meta_data_limit);
        static int total_file_entries;
        static vector<int> free_segments;

        map<string,vector<int> get_file_metadata();    
        void set_file_metadata(map<string,vector<int> data);  
        int get_seg_sequence();    
        void set_seg_sequence(int seg_sequence);  

        void del(string fname);


}