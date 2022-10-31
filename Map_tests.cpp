#include "unit_test_framework.h"
#include "Map.h"
#include <utility>


TEST(test_map_empty) {
    Map<std::string, int> m;
    
    ASSERT_TRUE(m.empty());

    std::pair<std::string, int> element{"item", 1};
    m.insert(element);
    ASSERT_FALSE(m.empty());
}

TEST(test_map_size) {
    Map<std::string, int> m;

    ASSERT_EQUAL(m.size(), 0u);

    std::pair<std::string, int> element{"item", 1};
    m.insert(element);
    ASSERT_EQUAL(m.size(), 1u);   
}

TEST(test_map_find) {
    Map<std::string, int> m;

    ASSERT_EQUAL(m.find("item"), m.end());  // empty case

    std::pair<std::string, int> element{"item", 1};
    m.insert(element);
    std::pair<std::string, int> element2{"item2", 2};
    m.insert(element2);
    std::pair<std::string, int> element3{"item3", 3};
    m.insert(element3);
    ASSERT_EQUAL(*m.find("item2"), element2);
}

TEST(test_map_index_operator) {
    Map<std::string, int> m;
    
    int & default_val = m["item"];
    ASSERT_EQUAL(default_val, 0); // empty case
    
    default_val = 1;
    ASSERT_EQUAL(m["item"], 1); // test value was returned by reference 
}

TEST(test_map_insert) {
    Map<std::string, int> m;
    std::pair<std::string, int> element{"item", 1};
    std::pair<Map<std::string, int>::Iterator, bool> insert_pair = m.insert(element); // insert a new element
    
    ASSERT_TRUE(insert_pair.second);
    std::pair<std::string, int> inserted_element = *insert_pair.first;
    ASSERT_EQUAL(inserted_element.first, "item");
    ASSERT_EQUAL(inserted_element.second, 1);

    std::pair<Map<std::string, int>::Iterator, bool> duplicate_pair = m.insert(element); // insert duplicate element
    
    ASSERT_FALSE(duplicate_pair.second);
    std::pair<std::string, int> duplicate_element = *duplicate_pair.first;
    ASSERT_EQUAL(duplicate_element.first, "item");
    ASSERT_EQUAL(duplicate_element.second, 1);
}

TEST(test_map_iterator_end) {
    Map<std::string, int> m;
    Map<std::string, int>::Iterator null_it;

    ASSERT_EQUAL(m.end(), null_it);  
}

TEST(test_map_iterator_begin) {
    Map<std::string, int> m;
    
    ASSERT_EQUAL(m.begin(), m.end()); // empty case
    
    std::pair<std::string, int> element3{"item3", 3};
    m.insert(element3);
    std::pair<std::string, int> element2{"item2", 2};
    m.insert(element2);
    std::pair<std::string, int> element{"item", 1}; // shortest key
    m.insert(element);
   
    ASSERT_EQUAL(*m.begin(), element);  
}


TEST_MAIN()