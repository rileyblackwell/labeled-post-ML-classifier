#include "BinarySearchTree.h"
#include "unit_test_framework.h"
#include <sstream>
#include <string>

TEST(test_bst_ctor) {
    BinarySearchTree<int> b;
    ASSERT_TRUE(b.empty());
}

TEST(test_bst_copy) {
    BinarySearchTree<int> b;
    b.insert(1);
    b.insert(2);
    b.insert(3);
    BinarySearchTree<int> b2(b);
    
    auto b_it = b.begin();
    auto b2_it = b2.begin();
    ASSERT_EQUAL(*b_it, *b2_it++);
    ASSERT_EQUAL(++*b_it, *b2_it++);
    ASSERT_EQUAL(++*b_it, *b2_it);
}

TEST(test_bst_assignment_operator) {
    BinarySearchTree<int> b;
    b.insert(1);
    b.insert(2);
    b.insert(3);
    BinarySearchTree<int> b2;
    b2 = b;

    auto b_it = b.begin();
    auto b2_it = b2.begin();
    ASSERT_EQUAL(*b_it, *b2_it++);
    ASSERT_EQUAL(++*b_it, *b2_it++);
    ASSERT_EQUAL(++*b_it, *b2_it);
}

TEST(test_bst_empty) {
    BinarySearchTree<int> b;
    ASSERT_TRUE(b.empty());

    b.insert(2);
    ASSERT_FALSE(b.empty());
}

TEST(test_bst_height) {
    BinarySearchTree<int> b;
    ASSERT_EQUAL(b.height(), 0u);

    b.insert(1);
    b.insert(2);
    ASSERT_EQUAL(b.height(), 2u);
}

TEST(test_bst_size) {
    BinarySearchTree<int> b;  
    ASSERT_EQUAL(b.size(), 0u);

    b.insert(1);
    b.insert(2);
    b.insert(3);
    ASSERT_EQUAL(b.size(), 3u);       
}

TEST(test_iterator_increment_operator) {
    BinarySearchTree<int> b;
    b.insert(3);
    b.insert(2);
    b.insert(1);

    auto b_it = b.begin();
    ASSERT_EQUAL(++*b_it, 2);
    b_it++;
    ASSERT_EQUAL(*b_it, 3);    
}

TEST(test_bst_iterator_begin) {
    BinarySearchTree<int> b;
    b.insert(1);
    b.insert(2);
    b.insert(3);

    ASSERT_EQUAL(*b.begin(), 1);
}

TEST(test_bst_iterator_find) {
    BinarySearchTree<int> b;
    ASSERT_EQUAL(b.find(2), b.end()); // empty case

    b.insert(1);
    b.insert(2);
    b.insert(3);
    b.insert(4);
    ASSERT_EQUAL(*b.find(3), 3); // finds item
    
    ASSERT_EQUAL(b.find(5), b.end()); // doesn't find item
}

TEST(test_bst_iterator_insert) {
    BinarySearchTree<int> b;
    b.insert(4); // insert root
    b.insert(5); // insert right subtree
    b.insert(2); // insert left subtree
    b.insert(1);
    b.insert(3);
    auto b_it = b.begin();
    ASSERT_EQUAL(*b_it, 1);
    ASSERT_EQUAL(++*b_it, 2); 
    ASSERT_EQUAL(++*b_it, 3);
    ASSERT_EQUAL(++*b_it, 4); 
    ASSERT_EQUAL(++*b_it, 5);              
}

TEST(test_bst_iterator_min_element) {
    BinarySearchTree<int> b;
    b.insert(1);
    b.insert(2);
    b.insert(3);

    ASSERT_EQUAL(*b.min_element(), 1);        
}

TEST(test_bst_iterator_max_element) {
    BinarySearchTree<int> b;
    b.insert(1);
    b.insert(2);
    b.insert(3);

    ASSERT_EQUAL(*b.max_element(), 3);        
}

TEST(test_bst_check_sorting_invarient) {
    BinarySearchTree<int> b;
    ASSERT_TRUE(b.check_sorting_invariant()); // empty case
    
    b.insert(1);
    b.insert(2);
    b.insert(3);
     
    ASSERT_TRUE(b.check_sorting_invariant()); // correct case
 
    auto b_it = b.begin();
    *b_it = 4;
    ASSERT_FALSE(b.check_sorting_invariant()); // incorrect left subtree

    *b_it = 1; // fix invariant
    ASSERT_TRUE(b.check_sorting_invariant());
    ++b_it;
    ++b_it;
    *b_it = 0; // break invariant
    ASSERT_FALSE(b.check_sorting_invariant()); // incorrect right subtree  
}

TEST(test_traverse_inorder) {
    BinarySearchTree<int> b;
    b.insert(2);
    b.insert(1);
    b.insert(3);

    std::string correct_output = "1 2 3 ";
    std::ostringstream output;
    b.traverse_inorder(output);
    ASSERT_EQUAL(output.str(), correct_output);    
}

TEST(test_traverse_preorder) {
    BinarySearchTree<int> b;
    b.insert(2);
    b.insert(1);
    b.insert(3);

    std::string correct_output = "2 1 3 ";
    std::ostringstream output;
    b.traverse_preorder(output);
    ASSERT_EQUAL(output.str(), correct_output); 
}

TEST(test_bst_iterator_min_greater_than) {
    BinarySearchTree<int> b; 
    b.insert(4);
    b.insert(2);
    b.insert(3);
    b.insert(1);
    b.insert(5);
    b.insert(6);
    b.insert(7);
    
    ASSERT_EQUAL(*b.min_greater_than(0), 1);
    ASSERT_EQUAL(*b.min_greater_than(3), 4);
    ASSERT_EQUAL(*b.min_greater_than(6), 7);     
}

TEST(test_bst_insertion_operator) {
    BinarySearchTree<int> b;
    b.insert(3);
    b.insert(2);
    b.insert(1);
    b.insert(4);
    b.insert(6);
    b.insert(5);

    std::string correct_output = "[ 1 2 3 4 5 6 ]";
    std::ostringstream output;
    output << b;
    ASSERT_EQUAL(correct_output, output.str());
}

TEST_MAIN()
