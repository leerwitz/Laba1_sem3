#include <iostream>
#include "cache.hpp"

void Test_of_find(){
    UnorderedMap<char, int> map;
    char p = 'a';
    if(map.find(p) == map.end())
        std::cout << "Test passed" << "\n";
    else
        std::cout << "Test failed" << "\n";
}

void Test_of_insert(){
    UnorderedMap<char, int> map;
    map.insert(std::make_pair('a',1));
    char p = 'a';
    if(map.find(p) != map.end())
        std::cout << "Test passed" << "\n";
    else
        std::cout << "Test failed" << "\n";
}

void Test_of_square_brackets(){
    UnorderedMap<char, int> map;
    map.insert(std::make_pair('a',1));
    map['c'] = 2;
    if (map['a'] == 1 && map.find('c') != map.end())
        std::cout << "Test passed" << "\n";
    else
        std::cout << "Test failed" << "\n";
}

void Test_of_resize() {
    UnorderedMap<int, int> map;
    map.resize(10);
    for (int i = 0; i < 5; ++i) {
        map.insert(std::make_pair(i,i));
    }
    map.resize(20);
    for (int i = 0; i < 5; ++i) {
        if (map.find(i) == map.end()){
            std::cout << "Test failed" << "\n";
            break;
        }
    }
    if (map.capacity() == 20)
        std::cout << "Test passed" << "\n";
    else std::cout << "Test failed" << "\n";
}

void Test_of_grow() {
    UnorderedMap<int, int> map;
    map.resize(10);
    for (int i = 0; i < 12; ++i) {
        map.insert(std::make_pair(i,i));
    }
    for (int i = 0; i < 12; ++i) {
        if (map.find(i) == map.end()){
            std::cout << "Test failed" << "\n";
            break;
        }
    }
    if (map.capacity() == 32)
        std::cout << "Test passed" << "\n";
    else std::cout << "Test failed" << "\n";
}

void Test_of_cache_get (){
    cache<int, int> map = cache<int,int>(3);
    int a = map.get(2);
    std:: cout << "Test failed" << "\n";
}

void Test_of_cache_put (){
    cache<int, int> map = cache<int,int>(3);
    map.put(1,1);
    int a = map.get(1);
    if (a == 1)
        std::cout << "Test passed" << "\n";
    else std::cout << "Test failed" << "\n";
}

void Test_of_cache_clean(){
    cache<int, int> map = cache<int,int>(2);
    map.put(1,1);
    map.put(2,2);
    map.put(3,3);
    map.get(1);
    std::cout << "Test failed" << "\n";

}

void Run_All_Tests(){
    Test_of_find();
    Test_of_insert();
    Test_of_square_brackets();
    Test_of_resize();
    Test_of_grow();
    //Test_of_cache_get();
    Test_of_cache_put();
    //Test_of_cache_clean();
}