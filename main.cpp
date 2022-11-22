#include <iostream>
#include "csvstream.h"
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
        Classifier(csvstream &train_csv, const bool enable_debug) : debug(enable_debug), num_training_posts(0) {
            if (debug) {
                cout << "training data:" << endl;
            }
            
            map<string, string> row;
            string training_content;
            string training_labels;
            while (train_csv >> row) {
                posts.push_back({row["tag"], unique_words(row["content"])});
                num_training_posts += 1;
                training_content += row["content"] + " ";
                training_labels += row["tag"] + " ";
                
                if (debug) {
                    cout << "  label = " << row["tag"] 
                         << ", content = "<< row["content"] << endl;
                } 
            }
            
            cout << "trained on " << num_training_posts << " examples" << endl;
            words = unique_words(training_content);
            labels = unique_words(training_labels); 
            if (debug) {    
                cout << "vocabulary size = " << words.size() << endl;
            }
            cout << endl; // print extra new line    
        }

        void train() {
            for (const string &word : words) {
                for (const auto &post : posts) {
                    if(post.second.find(word) != post.second.end()) {
                        num_posts_with_word[word] += 1;
                        num_posts_with_label_and_word[post.first][word] += 1;
                    }   
                }
            }
              
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
                    const double log_prior = log(num_labels / static_cast<double>(num_training_posts));
                    cout << "  " << label << ", " << num_labels << " examples, log-prior = " 
                         << log_prior << endl;
                }
                cout << "classifier parameters:" << endl;
                for (const string &label : labels) {
                    for (const string &word : words) {                      
                        if (num_posts_with_label_and_word[label].find(word) != num_posts_with_label_and_word[label].end()) {
                            const double num_labels_and_words = num_posts_with_label_and_word[label][word];
                            const double log_likelihood = log(num_labels_and_words / static_cast<double>(num_posts_with_label[label]));
                            cout << "  " << label << ":" << word << ", count = " << num_labels_and_words
                             << ", log-likelihood = " << log_likelihood << endl;            
                        }    
                    }
                }
                cout << endl; // print extra new line                           
            }        
        }

        void prediction(csvstream &test_csv) {
            map<string, string> row;
            double best_lp_score = 0;
            string best_lp_label;
            bool first_lp = true;
            int num_correct_posts = 0;
            int num_posts = 0;
            cout << "test data:" << endl;
            while (test_csv >> row)  {
                num_posts += 1;
                best_lp_score = 0;
                best_lp_label = "";
                first_lp = true;
                for (const string &label : labels) {
                    double log_prior = log(static_cast<double>(num_posts_with_label[label]) 
                                                 / static_cast<double>(num_training_posts));
                    set<string> post_words = unique_words(row["content"]);
                    for (const string &word : post_words) {
                        if (num_posts_with_label_and_word[label].find(word) != num_posts_with_label_and_word[label].end()) {
                            const double log_likelihood = log(static_cast<double>(num_posts_with_label_and_word[label][word]) 
                                                              / static_cast<double>(num_posts_with_label[label]));
                            log_prior += log_likelihood;
                        } else if (num_posts_with_word.find(word) != num_posts_with_word.end()) {
                            const double log_likelihood = log(static_cast<double>(num_posts_with_word[word]) 
                                                              / static_cast<double>(num_training_posts));
                            log_prior += log_likelihood;    
                        } else {
                            const double log_likelihood = log(1 / static_cast<double>(num_training_posts));
                            log_prior += log_likelihood;    
                        }
                    }
                    if (first_lp) {
                        best_lp_score = log_prior;
                        best_lp_label = label;
                        first_lp = false;
                    } else if (log_prior > best_lp_score) {
                        best_lp_score = log_prior;
                        best_lp_label = label;    
                    } else if (log_prior == best_lp_score) {
                        if (label > best_lp_label) {
                            best_lp_score = log_prior;
                            best_lp_label = label; 
                        }
                    }
                }
                cout << "  correct = " << row["tag"] << ", predicted = " << best_lp_label
                     << ", log-probability score = " << best_lp_score << endl
                     << "  content = " << row["content"] << endl << endl;
                if (best_lp_label == row["tag"]) {
                    num_correct_posts += 1;
                }
            }
            cout << "performance: " << num_correct_posts << " / " << num_posts << " posts predicted correctly" << endl;             
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
        int num_training_posts;
        vector<pair<string, set<string>>> posts; 
        set<string> words;
        set<string> labels;
        map<string, int> num_posts_with_word;
        map<string, map<string, int>> num_posts_with_label_and_word;
        map<string, int> num_posts_with_label;         
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
        classifier.prediction(test_csv);
    } catch (csvstream_exception &e) {
        cout << "Error opening file: " << argv[1] << endl;
        return 1;
    }
   
    return 0;
}

