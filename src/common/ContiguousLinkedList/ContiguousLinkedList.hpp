#include <cassert>
#include <iostream>
#include <iterator>
#include <memory>

template <typename T, typename Allocator = std::allocator<T>> class ContiguousLinkedList
{
  public:
    struct Node
    {
        T data;
        int next; // Index of the next node in the array
        int prev; // Index of the previous node in the array (for bidirectional traversal)
    };

    using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;

    // Constructor to initialize the list with a fixed size
    ContiguousLinkedList(std::size_t max_size, const Allocator &alloc = Allocator())
        : max_size(max_size), head(-1), tail(-1), free_head(0), size_(0), allocator(alloc)
    {

        // Allocate memory for the nodes using the custom allocator
        nodes = allocator.allocate(max_size);

        // Initialize the free list
        for (std::size_t i = 0; i < max_size - 1; ++i)
        {
            nodes[i].next = i + 1;
            nodes[i].prev = -1; // Initialize previous pointers to -1
        }
        nodes[max_size - 1].next = -1; // End of free list
        nodes[max_size - 1].prev = -1;
    }

    // Destructor to deallocate memory using the custom allocator
    ~ContiguousLinkedList()
    {
        allocator.deallocate(nodes, max_size);
    }

    // Insert a new element at the front of the list
    void push_front(const T &value)
    {
        int new_node_index = allocate_node();
        nodes[new_node_index].data = value;
        nodes[new_node_index].next = head;
        nodes[new_node_index].prev = -1;

        if (head != -1)
        {
            nodes[head].prev = new_node_index;
        }
        head = new_node_index;

        if (tail == -1)
        { // List was empty
            tail = new_node_index;
        }

        ++size_;
    }

    // Insert a new element at the back of the list
    void push_back(const T &value)
    {
        int new_node_index = allocate_node();
        nodes[new_node_index].data = value;
        nodes[new_node_index].next = -1;
        nodes[new_node_index].prev = tail;

        if (tail != -1)
        {
            nodes[tail].next = new_node_index;
        }
        tail = new_node_index;

        if (head == -1)
        { // List was empty
            head = new_node_index;
        }

        ++size_;
    }

    // Remove the front element from the list
    void pop_front()
    {
        if (head == -1)
        {
            std::cerr << "List is empty!" << std::endl;
            return;
        }

        int node_to_free = head;
        head = nodes[head].next;

        if (head != -1)
        {
            nodes[head].prev = -1;
        }
        else
        {
            tail = -1; // List is now empty
        }

        free_node(node_to_free);
        --size_;
    }

    // Remove the back element from the list
    void pop_back()
    {
        if (tail == -1)
        {
            std::cerr << "List is empty!" << std::endl;
            return;
        }

        int node_to_free = tail;
        tail = nodes[tail].prev;

        if (tail != -1)
        {
            nodes[tail].next = -1;
        }
        else
        {
            head = -1; // List is now empty
        }

        free_node(node_to_free);
        --size_;
    }

    // Access the front element
    T &front()
    {
        assert(head != -1);
        return nodes[head].data;
    }

    const T &front() const
    {
        assert(head != -1);
        return nodes[head].data;
    }

    // Access the back element
    T &back()
    {
        assert(tail != -1);
        return nodes[tail].data;
    }

    const T &back() const
    {
        assert(tail != -1);
        return nodes[tail].data;
    }

    // Check if the list is empty
    bool empty() const
    {
        return size_ == 0;
    }

    // Return the size of the list
    std::size_t size() const
    {
        return size_;
    }

    // Clear the list
    void clear()
    {
        while (!empty())
        {
            pop_front();
        }
    }

    // Iterator class for the list
    class iterator
    {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;

        iterator(Node *nodes, int index) : nodes(nodes), index(index)
        {
        }

        reference operator*() const
        {
            return nodes[index].data;
        }
        pointer operator->()
        {
            return &nodes[index].data;
        }

        // Pre-increment
        iterator &operator++()
        {
            index = nodes[index].next;
            return *this;
        }

        // Post-increment
        iterator operator++(int)
        {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        // Pre-decrement
        iterator &operator--()
        {
            index = nodes[index].prev;
            return *this;
        }

        // Post-decrement
        iterator operator--(int)
        {
            iterator temp = *this;
            --(*this);
            return temp;
        }

        friend bool operator==(const iterator &a, const iterator &b)
        {
            return a.index == b.index;
        }

        friend bool operator!=(const iterator &a, const iterator &b)
        {
            return a.index != b.index;
        }

        Node *nodes;
        int index;

      private:
    };

    // Begin iterator
    iterator begin()
    {
        return iterator(nodes, head);
    }

    // End iterator
    iterator end()
    {
        return iterator(nodes, -1);
    }

    // Insert an element at a specific position (before the given iterator)
    iterator insert(iterator pos, const T &value)
    {
        if (pos == begin())
        {
            push_front(value);
            return begin();
        }
        else if (pos == end())
        {
            push_back(value);
            iterator it = end();
            return --it;
        }
        else
        {
            int new_node_index = allocate_node();
            int current_index = pos.index;

            nodes[new_node_index].data = value;
            nodes[new_node_index].next = current_index;
            nodes[new_node_index].prev = nodes[current_index].prev;

            nodes[nodes[current_index].prev].next = new_node_index;
            nodes[current_index].prev = new_node_index;

            ++size_;
            return iterator(nodes, new_node_index);
        }
    }

    // Erase an element at a specific position
    iterator erase(iterator pos)
    {
        if (pos.index == head)
        {
            pop_front();
            return begin();
        }
        else if (pos.index == tail)
        {
            pop_back();
            return end();
        }
        else
        {
            int current_index = pos.index;
            int next_index = nodes[current_index].next;

            nodes[nodes[current_index].prev].next = next_index;
            nodes[next_index].prev = nodes[current_index].prev;

            free_node(current_index);
            --size_;
            return iterator(nodes, next_index);
        }
    }

    // Print the list (for debugging)
    void print_list() const
    {
        int current = head;
        while (current != -1)
        {
            std::cout << nodes[current].data << " -> ";
            current = nodes[current].next;
        }
        std::cout << "nullptr" << std::endl;
    }

  private:
    Node *nodes;             // Contiguous block of memory for nodes
    std::size_t max_size;    // Maximum number of nodes in the list
    int head;                // Index of the first node in the list
    int tail;                // Index of the last node in the list
    int free_head;           // Index of the first free node in the free list
    std::size_t size_;       // Current size of the list
    NodeAllocator allocator; // Allocator for managing node memory

    // Allocate a node from the free list
    int allocate_node()
    {
        if (free_head == -1)
        {
            throw std::bad_alloc();
        }
        int new_node_index = free_head;
        free_head = nodes[free_head].next;
        return new_node_index;
    }

    // Free a node and add it back to the free list
    void free_node(int index)
    {
        nodes[index].next = free_head;
        free_head = index;
    }
};
