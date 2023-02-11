#include <vector>
#include <iostream>
#include <functional>
#include <cstring>

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <type_traits>



template<typename T, typename Allocator=std::allocator<T>>
class List {
public:
    explicit List(const Allocator& alloc = Allocator()) :
            allocator(alloc),
            head(ATraits::allocate(allocator, 1)),
            sz(0),
            tail(ATraits::allocate(allocator, 1)) {
        head->r = tail;
        tail->l = head;
    }

    explicit List(size_t count) :
            allocator(),
            head(ATraits::allocate(allocator, 1)),
            sz(0),
            tail(ATraits::allocate(allocator, 1)) {
        head->r = tail;
        tail->l = head;
        for (size_t i = 0; i < count; ++i) {
            node* newNode = ATraits::allocate(allocator, 1);
            ATraits::construct(allocator, newNode);
            insertBetween(tail->l, tail, newNode);
        }
    }

    List(size_t count, const T& value, const Allocator& alloc = Allocator()) :
            allocator(alloc),
            head(ATraits::allocate(allocator, 1)),
            sz(0),
            tail(ATraits::allocate(allocator, 1)) {
        head->r = tail;
        tail->l = head;
        for (size_t i = 0; i < count; ++i)
            push_back(value);
    }

    Allocator get_allocator() const {
        return allocator;
    }

    size_t size() const {
        return sz;
    }

    T& front() {
        return *begin();
    }

    T& back() {
        return *end();
    }

    void push_back(const T& value) {
        insertBetween(tail->l, tail, value);
    }

    template <typename... Args>
    void emplace_back(Args&& ... args) {
        node* newnode = ATraits::allocate(allocator, 1);
        ATraits::construct(allocator, newnode, std::forward<Args>(args)...);
        insertBetween(tail->l, tail, newnode);
    }


    void push_front(const T& value) {
        insertBetween(head, head->r, value);
    }

    void pop_back() {
        erase(tail->l);
    }

    void pop_front() {
        erase(head->r);
    }

    void clear() {
        while (sz > 0)
            pop_back();
    }

    List& operator=(const List& other) {
        if (&other == this)
            return *this;
        clear();
        if ((allocator != other.allocator) && (ATraits::propagate_on_container_copy_assignment::value)) {
            ATraits::deallocate(allocator, tail, 1);
            ATraits::deallocate(allocator, head, 1);
            allocator = other.allocator;
            tail = ATraits::allocate(allocator, 1);
            head = ATraits::allocate(allocator, 1);
            tail->l = head;
            head->r = tail;
        }
        node* i = other.head->r;
        while (i != other.tail) {
            push_back(i->value);
            i = i->r;
        }
        return *this;
    }
    List& operator=(List&& other) noexcept {
        clear();
        if ((allocator != other.allocator) && (ATraits::propagate_on_container_move_assignment::value)) {
            ATraits::deallocate(allocator, tail, 1);
            ATraits::deallocate(allocator, head, 1);
            allocator = other.allocator;
            tail = ATraits::allocate(allocator, 1);
            head = ATraits::allocate(allocator, 1);
            tail->l = head;
            head->r = tail;
        }
        sz = other.sz;
        if (sz) {
            head -> r = other.head->r;
            tail -> l = other.tail->l;

            other.head->r->l = head;
            other.tail->l->r = tail;
            other.tail->l = other.head;
            other.head->r = other.tail;
            other.sz = 0;
        }
        return *this;
    }

    List(List&& other): allocator(std::move(other.allocator)),
                        head(ATraits::allocate(allocator, 1)),
                        tail(ATraits::allocate(allocator, 1)) {
        sz = other.sz;
        head -> r = other.head->r;
        tail -> l = other.tail->l;

        other.head->r->l = head;
        other.tail->l->r = tail;
        other.tail->l = other.head;
        other.head->r = other.tail;
        other.sz = 0;
    }

    List(const List& other) {
        allocator = ATraits::select_on_container_copy_construction(other.allocator);
        head = ATraits::allocate(allocator, 1);
        sz = 0;
        tail = ATraits::allocate(allocator, 1);
        head->r = tail;
        tail->l = head;
        node* i = other.head->r;
        while (i != other.tail) {
            push_back(i->value);
            i = i->r;
        }
    }


    ~List() {
        clear();
        ATraits::deallocate(allocator, head, 1);
        ATraits::deallocate(allocator, tail, 1);
    }

private:
    struct node {
        node* l = nullptr;
        node* r = nullptr;
        T value;
        template<class... Args>
        node(Args&&... args): value(std::forward<Args>(args)...) {};
    };

    template<typename linkType, typename pointerType, typename valueType>
    struct baseIterator
            : public std::iterator< valueType, int64_t, pointerType, linkType> {



        explicit baseIterator(node* ptr) : ptr(ptr) {}

        operator baseIterator<const valueType&, const valueType*, valueType>() {
            return baseIterator<const valueType&, const valueType*, valueType>(ptr);
        }

        baseIterator& operator++() {
            ptr = ptr->r;
            return *this;
        }

        baseIterator& operator--() {
            ptr = ptr->l;
            return *this;
        }

        baseIterator operator++(int) {
            const auto ans = *this;
            ++*this;
            return ans;
        }

        baseIterator operator--(int) {
            const auto ans = *this;
            --*this;
            return ans;
        }

        baseIterator base() const {
            return *this;
        }

        linkType operator*() {
            return const_cast<linkType> (ptr->value);
        }

        template<class IT>
        bool operator==(const IT & other) const {
            return ptr == other.ptr;
        }

        template<class IT>
        bool operator!=(const IT & other) const {
            return ptr != other.ptr;
        }

        pointerType operator->() {
            return const_cast<pointerType> (&ptr->value);
        }

    private:
        node* ptr;
        friend List;
    };


    using nodeAllocatorT = typename std::allocator_traits<Allocator>::template rebind_alloc<node>;
    using ATraits = typename std::allocator_traits<Allocator>::template rebind_traits<node>;

    nodeAllocatorT allocator;

    node* head;
    size_t sz;
    node* tail;

    void erase(node* pos) {
        pos->l->r = pos->r;
        pos->r->l = pos->l;
        ATraits::deallocate(allocator, pos, 1);
        --sz;
    }

    node* insertBetween(node* l, node* r, const T& value) {
        node* newNode = ATraits::allocate(allocator, 1);
        ATraits::construct(allocator, newNode, value);
        newNode->r = r;
        newNode->l = l;
        l->r = newNode;
        r->l = newNode;
        ++sz;
        return newNode;
    }



    node* insertBetween(node* l, node* r, node* newNode) {
        newNode->r = r;
        newNode->l = l;
        l->r = newNode;
        r->l = newNode;
        ++sz;
        return newNode;
    }

public:
    using iterator = baseIterator<T&, T*, T>;
    using const_iterator = baseIterator<const T&, const T*, T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() { return iterator(head->r); }

    iterator end() { return iterator(tail); }

    const_iterator begin() const { return const_iterator(head->r); }

    const_iterator end() const { return const_iterator(tail); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }

    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    const_iterator cbegin() const { return const_iterator(head->r); }

    const_iterator cend() const { return const_iterator(tail); }

    const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }

    const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

    bool empty() const {
        return sz == 0;
    }

    iterator insert(const_iterator pos, const T& value) {
        auto res = insertBetween(pos.ptr->l, pos.ptr, value);
        return iterator(res);
    }

    iterator insert(const_iterator pos, T* value) {
        node* newnode = ATraits::allocate(allocator, 1);
        memcpy((void*)(&(newnode->value)), (void*)value, sizeof(T));
        auto res = insertBetween(pos.ptr->l, pos.ptr, newnode);
        return iterator(res);
    }

    const_iterator erase(const_iterator pos) {
        auto res = pos.ptr->r;
        erase(pos.ptr);
        return const_iterator(res);
    }

    iterator erase(iterator pos) {
        auto res = pos.ptr->r;
        erase(pos.ptr);
        return iterator(res);
    }

    void splice(iterator pos, List& other, iterator otherPos) {
        --other.sz;
        node* m = otherPos.ptr;
        m->l->r = m->r;
        m->r->l = m->l;
        insertBetween(pos.ptr->l, pos.ptr, m);
    }

    template <typename... Args>
    iterator emplace(iterator pos, Args&& ... args) {
        node* newnode = ATraits::allocate(allocator, 1);
        ATraits::construct(allocator, newnode, args...);
        insertBetween(pos.ptr->l, pos.ptr, newnode);
        return iterator(newnode);
    }

};