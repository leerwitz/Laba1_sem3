#include "VectorSorter.hpp"
#include "Tests.hpp"
#include "story.hpp"

template<class T>
const T& min(const T& a, const T& b){
    if (a < b)
        return a;
    return b;
}

int main()
{
    /*UnorderedMap<int, char> a;
    UnorderedMap<std::string, std::string> b;
    a.insert(std::make_pair(1, 'c'));
    if (a.find(1) != a.end())
        std::cout << a.size() <<" ";
    a.rehash(12);*/

    UnorderedMap<std::string , int> top;
    std::string book(book_());
    std::string current_word;
    std::vector<char> pointer = {' ','.','!','?', ';',':',',',')','(','/','\n','"'};
    char current_letter;
    for (int i = 0; i < book.size() ; ++i) {
        current_letter = book[i];
        int j = 0;
        while ((j < pointer.size()) && (current_letter != pointer[j]) ){
            ++j;
        }
        if (j == pointer.size())
        current_word.push_back(current_letter); else{
            if (top.find(current_word) != top.end() ){
                ++top[current_word];// через вектор сделать все слова, без знаков, и методом find искать эти слова.
                current_word.clear();
            } else{
                top[current_word] = 1;
                current_word.clear();
            }
        }
    }
    std::vector<std::pair<std::string , int>> Top_words;
    for (auto p : top) {
        Top_words.push_back(p);
    }

    quick_sort(Top_words,Top_words.size(),0);


    for (int i = 1; i <=  min<int> (100,Top_words.size()); ++i) {
        std::cout << Top_words[Top_words.size() - i].first << " " << Top_words[Top_words.size() - i].second << '\n';
    }


    Run_All_Tests();


}