#include <iostream>
#include "csvstream.h"
// #include "classifier.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <utility>
#include <cmath>

using namespace std;


class Classifier {
    public:
        // MODIFIES: train_csv
        // EFFECTS: Reads in and stores the training posts from train_csv.  Creates a set of unique labels
        // and a set of unique words.
        Classifier(csvstream &train_csv, const bool enable_debug) : debug(enable_debug), total_training_posts(0) {
            if (debug) {
                cout << "training data:" << endl;
            }
            
            map<string, string> training_data_row;
            string training_content;
            string training_labels;
            while (train_csv >> training_data_row) {
                posts.push_back({training_data_row["tag"], unique_words(training_data_row["content"])});
                total_training_posts += 1;
                training_content += training_data_row["content"] + " ";
                training_labels += training_data_row["tag"] + " ";
                
                if (debug) {
                    cout << "  label = " << training_data_row["tag"] 
                         << ", content = "<< training_data_row["content"] << endl;
                } 
            }
            
            cout << "trained on " << total_training_posts << " examples" << endl;
            words = unique_words(training_content);
            labels = unique_words(training_labels); 
            if (debug) {    
                cout << "vocabulary size = " << words.size() << endl;
            }
            cout << endl; // print extra new line    
        }

        void train() {
            map<string, int> num_posts_with_word;
            for (const string &word : words) {
                for (const auto &post : posts) {
                    if(post.second.find(word) != post.second.end()) {
                        num_posts_with_word[word] += 1;
                    }   
                }
            }
            
            map<string, int> num_posts_with_label;
            for (const string &label : labels) {
                for (const auto &post : posts) {
                    if(post.first == label) {
                        num_posts_with_label[label] += 1;
                    }   
                }    
            }
            
            if (debug) {
                cout << "classes:" << endl;
                for (const string &label : labels) {
                    const double num_labels = num_posts_with_label[label];
                    const double log_prior = log(num_labels / static_cast<double>(total_training_posts));
                    cout << "  " << label << ", " << num_labels << " examples, log-prior = " 
                         << log_prior << endl;
                }
            }
            
        }
    
    private:
        // EFFECTS: Return a set of unique whitespace delimited words.x
        set<string> unique_words(const string &str) {
          istringstream source(str);
          set<string> words;
          string word;
          while (source >> word) {
            words.insert(word);
          }
          return words;
        }

        bool debug;
        int total_training_posts;
        vector<pair<string, set<string>>> posts; 
        set<string> words;
        set<string> labels;         
};
 
int main(int argc, char *argv[]) {
    cout.precision(3);    
    
    if (!(argc == 3 || argc == 4)) {
        cout << "Usage: main.exe TRAIN_FILE TEST_FILE [--debug]" << endl;
        return 1;         
    }

    try {
        csvstream train_csv(argv[1]);
        csvstream test_csv(argv[2]);
        bool debug = false;
        if (argc == 4) { // debug
            string debug_arg = argv[3];
            if (debug_arg != "--debug") {
                cout << "Usage: main.exe TRAIN_FILE TEST_FILE [--debug]" << endl; 
                return 1;        
            } 
            debug = true;     
        }
        
        Classifier classifier(train_csv, debug);
        classifier.train();
       

    } catch (csvstream_exception &e) {
        cout << "Error opening file: " << argv[1] << endl;
        return 1;
    }
   

    return 0;
}



 