#include <iostream>
#include <unordered_set>
#include <iterator>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <time.h>
#include <map>
using namespace std;

//Important - Compile with -march=native and -O3 flags. 
//for example g++ clean_up.cpp -march=native -O3. Will make program run a lot faster
//-match=native -> compiles for your native architecture
//-O3 - enables third level optimization for the compiler.
//If for any reason it doesn't compile with -O3(it should) try -O2 before giving up on it




//This Document_Info should be changed to a struct. print_set() is now pretty useless 
//A class should hold data and methods. Without that print_set() it will only hold data
//for which a data structure specified by a struct is more useful. Along with print_set()
//ptr to set should also probably go since upon improvement to the program its now pretty useless.
class Document_Info{

	public:
		string name_of_document;
		unordered_set<string> *ptr_to_set;	
		void print_set();
		vector< unordered_set<string>* > *vec_set;
};

void Document_Info::print_set(){
	unordered_set<string>::iterator itr = ptr_to_set->begin();
		
	for(itr; itr!=ptr_to_set->end(); itr++){
		cout << *itr << endl;
	}
}

int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

//This class can be made nicer. Add a constructor that takes in the docs?
//Give it a method to delete the document_info objects after use?
//
//explanation of members. 
//docs -> holds the vector of all your Document_Info objects
//results -> holds a map hashtable with the results of running comparator.
//Comparator -> methods compares all the sets in every document. 
//map_printer ->useful method for printing the results.
class RunsComparison{

	public: 
		vector<Document_Info *> docs;
		map<string, int > results ;
		void comparator(vector<Document_Info *>*, int);
		void map_printer();
};

void RunsComparison::comparator(vector<Document_Info*>* vec_docs, int num_sets){
	int num_of_docs = vec_docs->size();

	//So how does this work?
	//SECTION 1: Well you have a vector of document_info objects each of them 
	//holding their own vector of sets. You have to comapre every 
	//document_info object with every other document_info object
	//so something like this [doc_info1,doc_info2,doc_info3,doc_info4]. How would you go about doing this?
	//compare doc_info1 to doc_info2,doc_info3, and doc_info4. Next compare doc_info2 to doc_info3 and doc_info4
	//and finally doc_info3 to doc_info4. Thats those two for loops j and k ie compare j to all objects after j 
	//and then move on to j+1 and compare to all objects to the end. On the previous example notice how after we run through
	//with doc_info1 we don't need it anymore because it wont be compared against again. In order to run big_doc_set which takes up
	//a lot of memory I delete the doc_info_1 after using it. Downside? Our document objects are deleted so we can rerun as easily.
	//upsides? Saves us od memory. 
	//
	//SECTION 2: So now we have to compare each and every object. Each object holds a vector of hashtables specifically sets.
	//There are n sets in every object. Lets say the two vectors of sets for docs A, and B are [set1A,set2A,set3A],
	//and [set1B,set2B,set3B]. Well how about we compare set1A to set1B,Set2B,set3B and then set2A to set1B,set2B,set3B
	//and same for set3A. well those are the next two four loops with incrementers l and m. set_ptrj is the current set
	//in vector vec_setj we're working on and set_ptrk is the current_set in vector vec_setk we're working on.
	//We compare set_ptrj (incremented by l) to every set_ptrk. 
	//
	//SECTION 3: All that's left is to count how many elements in set_ptrk are in set_ptrk. We use set::count() for this task.
	//count returns 1 if the element in set_ptrk is in set_ptrj and 0 otherwise. So we use an iterator object to
	//go through set_ptrk ([] and ->at() wont work for sets. We need an iterator to go through them). If the 
	//element is in the set. We increment our counter. and after comparing all the sets between the two we add it to our map
	//
	//
	//a little math. 
	//
	//Looking back on section 1 we can see for doc_info1 there are 3 comparisons, then 2 for doc_info2 and then 1 
	//for doc_info3. In general given m documents there will be m-1 + m-2 + m-3 + ... + 1 = m(m-1)/2 document comparisons.
	//
	//Looking back on section 2 for set1A we have three document comparisons,then 3 more for Set2A(against every set in the other 
	//vector) and then 3 more for set3A = 9. In general if we're comparing n sets with n sets we'll have n+n+n+n...(n times) = n*n = n^2
	//sets that have to compared for every document pair.
	//
	//Looking back on section 3 lets say we have an average of w_bar words per document. or floor(w/n) chunks per set
	//
	//In total how many set comparisons? Well we have m(m-1)/2 document comparisons and for each we have to do n^2 set comparisons so
	//in total we'll have m(m-1)/2 *n^2 set comparisons or if the average is w_bar words per document m(m-1)/2*n^2*w_bar set lookups
	//
	//This comes out to something like O(m^2*n^2*floor(w_bar/n)) but even this can be further simplified since 
	//floor(w_bar/n) = w_bar/n - (w_bar%n)/n. But this would just look messy in the big oh and im not doing it ;) 
	for(int j = 0; j < num_of_docs -1; j++){
		
		Document_Info* doc_ptrj = vec_docs->at(j);
		vector< unordered_set<string>* > *vec_set_j = doc_ptrj->vec_set;

		for(int k = j+1; k < num_of_docs ; k++){
			Document_Info* doc_ptrk = vec_docs->at(k);
			
			int counter = 0;
			vector< unordered_set<string>* > *vec_set_k = doc_ptrk->vec_set;
			
			for(int l = 0; l < num_sets ; l++){			
				unordered_set<string> *set_ptrj = vec_set_j->at(l);
				for(int m = 0; m < num_sets ; m++){
					unordered_set<string> *set_ptrk = vec_set_k->at(m);
					
					unordered_set<string>::iterator itr_start_loop = set_ptrk->begin();
					unordered_set<string>::iterator itr_end_loop = set_ptrk->end();
           	
					for(itr_start_loop ; itr_start_loop != itr_end_loop ;itr_start_loop++){
               						if(!set_ptrj->count(*itr_start_loop)) {
                    					;
						}
							else{
								counter++;
							}

					}
				}
			}
			string name_together = doc_ptrj->name_of_document + " " + doc_ptrk->name_of_document;
			if(counter > 100){
			results.insert(pair<string,int> (name_together,counter));
			}
		}

		//Time to delete object j
		//Start by deleting all the sets and then the vector of objects and then the object itself
		for(int p; p < num_sets ; p++){
			delete vec_set_j->at(p);
			vec_set_j->at(p)=NULL;
		}
		delete vec_set_j;
		vec_set_j = NULL;
		delete doc_ptrj;
		doc_ptrj=NULL;
		
				
	
	}		
}
void RunsComparison::map_printer(){
	map<string,int>::iterator itr = results.begin();
        for(itr;itr != results.end();itr++){
                cout <<itr->first << " : " << itr->second << endl;
        }


}



int main(){ 
	int n;
        cout << "prints out every nth word. What do you want n to be? \n";
        cin >> n;

        string Directory_Name;
        string File_Name;
        
	cout << "Where's the directory where your files are kept \n";
        cin >> Directory_Name;
	
	clock_t start,end;
	start = clock();
	
	//Hacky fix. When we run getdir these strings pop up in the files vector
	//what are these? These are hidden folders in every linux directory. 
	//They're what allow you to do things like "cd .." to go back to the parent directory 
	//since .. is really just the parent directory. We want to ignore these when we're constructing the 
	//sets so save them here so we can put an if statement in the set construction loop and ignore them
	//if we see them. 
	string bad_string_one = ".";
	string bad_string_two = "..";
	

	vector<string> files = vector<string>(); //Vector to hold all the file names
        getdir(Directory_Name,files); // files vector  should now be populated with the file names


	//Vector to hold your objects
	vector<Document_Info *>* objects = new vector<Document_Info*>;
       	string str1,str2;

	//All objects made here (Document_Info,vector,and set) are allocated on the heap
	//Two advantages over allocating on the stack 
	//1)These sets contain hundereds of entries for every document. More space on the heap
	//2)If we move this main over to another function the objects will still be accessible.
	//Note: Care has to be taken in deleting these objects after their use
	
	for (int i = 0; i <files.size();i++){
        	
		File_Name = files[i];
		
		if (File_Name != bad_string_one  && File_Name  != bad_string_two ){
                	string filepath = Directory_Name+ "/" +File_Name;
			
			string file_no_ext = File_Name.substr(0,File_Name.size()-4); //Hacky way to remove .txt from the end of file
             
			//Create the object to hold the chunks for this text
			Document_Info* doc_ptr = new Document_Info();
			doc_ptr->name_of_document = file_no_ext;
			
			//Create the vector that'll hold the n sets
			vector<unordered_set<string>*> *vector_of_sets = new vector<unordered_set<string>*>;
			doc_ptr->vec_set = vector_of_sets;
					
			for(int cc = 0; cc < n ; cc++){	

			ifstream Txt_File;
                        Txt_File.open(filepath);

                        if (!Txt_File){
                                cout << "unable to open file";
                        }
			unordered_set<string>* set_str_ptr = new unordered_set<string>; //Make A new set for holding the chunks
            		int c = cc; 
                	while(Txt_File >> str1){
                        
			if (c != n){
			str2 += " " + str1; //Really recommend making a string formatting function for better matches.
					    //This is the current limitation of this program.
		       			    //Convert all the strings to lowercase -> matching should not be case sensetive
					    //Remove all special characters -> as per the problem description matching shouldn't
					    //depend on this either	
                        c += 1;}

			else{
                        set_str_ptr->insert(str2); // str2 is now an n letter sequence Add it to the set
                        str2 = ""; //Reset Str2
                        c = 0; //Reset the incrementer
                        }
                }
   		
		str1 = "";str2=""; //Empty str1 and str2 so that they're ready for the next iteration
	
	//	cout <<set_str_ptr->size() << " " + to_string(cc) <<  endl; This line is good for debugging
	
		vector_of_sets->push_back(set_str_ptr); //Put the set into the vector of sets for this object
                Txt_File.close(); //Close it so that on the next iteration we have a fresh file to iterate over
			}
		objects -> push_back(doc_ptr); //Put the Object in the vector. I would argue that this vector
					       //Should be replaced with a class that can hold them all and
					       //do the comparisoons. UPDATE ->RunsComparison now has such an attribute->docs
	
        	}
	}

    RunsComparison cecil;
    cecil.comparator(objects,n);
    cecil.map_printer();


    end = clock();
    double time_taken = double(end-start)/double(CLOCKS_PER_SEC);
    cout << "the time taken for this program is :"<<to_string(time_taken) << endl;
    //To get it as much as 2-3 times faster compile with
    // -march=native  . This compiles for your native architecture
    // -O3  . This compiles with 3 level optimzation. Turns on a bunch of optimization flags
    return 0;
}
