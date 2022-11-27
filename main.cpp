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
        // MODIFIES: debug, num_trainig_posts
        // EFFECTS: Sets debug to true or false. Initializes the number of trainging posts to be 0.
        Classifier(const bool enable_debug) : debug(enable_debug), num_training_posts(0) {}

        // MODIFIES: num_posts_with_word, num_posts_with_label, num_posts_with_label_and_word
        // EFFECTS: Reads in the training posts from train_csv.  Calculates and then stores the training results
        // in the maps- num_posts_with_word, num_posts_with_label, and num_posts_with_label_and_word.    
        void train(csvstream &train_csv) {
            if (debug) {
                cout << "training data:" << endl;
            }

            map<string, string> post;     
            while (train_csv >> post) {
                set<string> words_in_post = unique_words(post["content"]);
                const string label = post["tag"];
                auto &words_with_label = num_posts_with_label_and_word[label];
                for (const string &word : words_in_post) {
                    num_posts_with_word[word]++;
                    words_with_label[word]++;
                }
                num_posts_with_label[label]++;
                num_training_posts += 1;
                
                if (debug) {
                    cout << "  label = " << post["tag"] 
                         << ", content = "<< post["content"] << endl;
                } 
            }     
            
            cout << "trained on " << num_training_posts << " examples" << endl;
           
            if (debug) {    
                cout << "vocabulary size = " << num_posts_with_word.size() << endl;
            }
            
            cout << endl; // print extra new line  
            
            if (debug) {
                print_classes_and_classifier_parameters();                          
            }                
        }

        // EFFECTS: Reads in the testing posts from test_csv.  Calculates a
        // log-probability score for each of the possible labels for a given post.
        // Predicts the label with the highest log-prob score for each post.   
        void prediction(csvstream &test_csv) { 
            cout << "test data:" << endl;
            
            map<string, string> post;
            int num_testing_posts = 0;
            int num_correct_posts = 0;
            bool first_log_prob_score = true;
            double best_log_prob_score = 0;
            string best_log_prob_label; 
            while (test_csv >> post)  {
                num_testing_posts += 1;
                first_log_prob_score = true;
                best_log_prob_score = 0;
                best_log_prob_label = "";
                for (const auto &label : num_posts_with_label) {
                    double log_prob_score = log(static_cast<double>(label.second) 
                                                 / static_cast<double>(num_training_posts));
                    
                    set<string> post_words = unique_words(post["content"]);
                    for (const string &word : post_words) {
                        log_prob_score += calculate_log_likelihood(label.first, word);
                    }
                    
                    if (first_log_prob_score) {
                        best_log_prob_score = log_prob_score;
                        best_log_prob_label = label.first;
                        first_log_prob_score = false;
                    } else {
                        max_log_prob_score(best_log_prob_score, best_log_prob_label, log_prob_score, label.first);
                    }
                }

                cout << "  correct = " << post["tag"] << ", predicted = " << best_log_prob_label
                     << ", log-probability score = " << best_log_prob_score << endl
                     << "  content = " << post["content"] << endl << endl;
                if (best_log_prob_label == post["tag"]) {
                    num_correct_posts += 1;
                }
            }

            cout << "performance: " << num_correct_posts << " / " << num_testing_posts << " posts predicted correctly" << endl;                 
        }
    
    private:
        // EFFECTS: Returns a set of unique whitespace delimited words.x
        set<string> unique_words(const string &str) {
          istringstream source(str);
          set<string> words;
          string word;
          while (source >> word) {
            words.insert(word);
          }
          return words;
        }

        void print_classes_and_classifier_parameters() {
            cout << "classes:" << endl;
               for (const auto &label : num_posts_with_label) {
                   const double num_labels = label.second;
                   const double log_prior = log(num_labels / static_cast<double>(num_training_posts));
                   cout << "  " << label.first << ", " << num_labels << " examples, log-prior = " 
                        << log_prior << endl;
               }
               
               cout << "classifier parameters:" << endl;
               for (const auto &label : num_posts_with_label) {
                   for (const auto &word : num_posts_with_word) {                      
                       if (num_posts_with_label_and_word[label.first].find(word.first) != num_posts_with_label_and_word[label.first].end()) {
                           const double num_labels_and_words = num_posts_with_label_and_word[label.first][word.first];
                           const double log_likelihood = log(num_labels_and_words / static_cast<double>(num_posts_with_label[label.first]));
                           cout << "  " << label.first << ":" << word.first << ", count = " << num_labels_and_words
                            << ", log-likelihood = " << log_likelihood << endl;            
                       }    
                   }
               }
               cout << endl; // print extra new line    
        }

        // EFFECTS: Returns the log-likelihood of word.
        double calculate_log_likelihood(const string &label, const string &word) {
            if (num_posts_with_label_and_word[label].find(word) != num_posts_with_label_and_word[label].end()) {
                return log(static_cast<double>(num_posts_with_label_and_word[label][word]) 
                                                  / static_cast<double>(num_posts_with_label[label]));                           
            } else if (num_posts_with_word.find(word) != num_posts_with_word.end()) {
                return log(static_cast<double>(num_posts_with_word[word]) 
                                                  / static_cast<double>(num_training_posts));                            
            } else {
                return log(1 / static_cast<double>(num_training_posts));                          
            }
        }

        // EFFECTS: Sets best_log_prob_score and best_log_prob_label as the max log prob score and corresponding label.
        void max_log_prob_score(double &best_log_prob_score, string &best_log_prob_label, const double log_prob_score, const string &label) {
            if (log_prob_score > best_log_prob_score) {
                best_log_prob_score = log_prob_score;
                best_log_prob_label = label;    
            } else if (log_prob_score == best_log_prob_score) {
                if (label > best_log_prob_label) {
                    best_log_prob_score = log_prob_score;
                    best_log_prob_label = label; 
                }
            }    
        }

        bool debug; // print debug output
        int num_training_posts;
        map<string, int> num_posts_with_word;
        map<string, int> num_posts_with_label;  
        map<string, map<string, int>> num_posts_with_label_and_word;          
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
        
        Classifier classifier(debug);
        classifier.train(train_csv);
        classifier.prediction(test_csv);

    } catch (csvstream_exception &e) {
        cout << "Error opening file: " << argv[1] << endl;
        return 1;
    }
  
    return 0;
}

