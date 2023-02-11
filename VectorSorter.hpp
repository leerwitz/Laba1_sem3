#pragma once
#include <vector>
#include <iostream>

template<class T>

void copy_vec(std::vector<T>& orig, std::vector<T>& copy, int first, int last, int first_copy){
    int size = last - first + 1;
    int size_copy = copy.size();
    /*int size_copy;
    if (first_copy == 0) {size_copy = last - first + 1;}
    else {
        size_copy = last - first + first_copy + 1;
    }
    copy.resize(size_copy);*/
    for (int i = 0; i < size_copy - first_copy ; ++i) {
        copy[i + first_copy] = orig[first + i];
    }
}

template<class T>

void merge(std:: vector<T>& vec1, std:: vector<T>& vec2, int size1, int size2, std::vector<T>& vector ){
    int i = 0;
    int j = 0;

    while ((i < size1) && (j < size2)) {
        if (vec1[i] <= vec2[j]) {
            vector[i + j] = vec1[i];
            ++i;
        } else {
            vector[i + j] = vec2[j];
            ++j;
        }
    }
    if (j == size2) {
        copy_vec(vec1, vector, i, size1, j + i);
        //for (i; i < size1; ++i) { vector[i + j] = vec1[i]; }
    } else {
        copy_vec(vec2, vector, j, size2, i + j);
        //for (j; j < size2; ++j) { vector[i + j] = vec2[j]; }
    }

}


template<class T>

void merge_sort(std:: vector<T>& vector){
    int last = vector.size() - 1;
    int mid = (last)/2;
    int size1 = mid  + 1;
    int  size2 = last - size1 + 1;
    if (last > 0) {
        std:: vector<T> *vec1 = new std:: vector<T>();
        std:: vector<T> *vec2 = new std::vector<T>();
        vec1->resize(size1);
        vec2->resize(size2);
        copy_vec(vector, *vec1, 0, mid, 0);
        copy_vec(vector, *vec2, mid + 1 , last, 0);
        merge_sort(*vec1);
        merge_sort(*vec2);
        merge(*vec1, *vec2, size1, size2, vector );
    }

}
template<class T>

void print_vec(std:: vector<T>& vector, int  size){
    for (int i = 0; i < size ; ++i) {
        std:: cout << vector[i] << " ";
    }
    std:: cout << "\n";
}

template<class T>

void swap(T& elem1,T& elem2){
    T a;
    a = std::move(elem1);
    elem1 = std::move(elem2);
    elem2 = std::move(a);

}

template<class T, class V>

void quick_sort(std::vector<std::pair<T,V>>& mas, int last, int first ){
    int pivot = first;
    int tail = first;
    int tmp = first;
    while (tmp < last ){
        if (mas[tmp].second < mas[last].second){
            swap(mas[tail] , mas[tmp] );
            pivot = tail;
            ++tail;
        }
        ++tmp;
    }
    swap(mas[tail], mas[last]);
    if ( pivot-first > 0) {quick_sort(mas, pivot, first);}
    if (last- pivot > 1){
        quick_sort(mas, last, pivot + 1);
    }

}

template<class T>
void bubble_sort(std::vector<T>& vec){
    int current;
    int next;
    bool flag = true;
    while (flag){
        flag = false;
        current = 0;
        next = 1;
        while(next < vec.size()) {
            if (vec[current] > vec[next]) {
                swap(vec[current], vec[next]);
                flag = true;
            }
            ++next;
            ++current;
        }
    }
}