#include <iostream>
#include <vector>

template <typename K, typename V>
class UnorderedMap
{
private:
    struct Node
    {
        K key;
        V value;
        Node *next;

        Node(K key, V value) : key(key), value(value), next(nullptr) {}
    };

    std::vector<Node *> buckets;
    size_t num_elements;
    static constexpr double load_factor = 0.75;

    size_t hash(const K &key)
    {
        return std::hash<K>{}(key) % buckets.size();
    }

    void rehash()
    {
        size_t new_size = buckets.size() * 2;
        std::vector<Node *> new_buckets(new_size, nullptr);

        for (Node *head : buckets)
        {
            while (head)
            {
                auto idx = std::hash<K>{}(head->key) % new_buckets.size();
                auto next = head->next;
                head->next = new_buckets[idx];
                new_buckets[idx] = head;
                head = next;
            }
        }
        buckets = std::move(new_buckets);
    }

public:
    UnorderedMap(size_t init_buckets = 8) : buckets(init_buckets, nullptr), num_elements(0) {}

    ~UnorderedMap()
    {
        for (Node *head : buckets)
        {
            while (head)
            {
                Node *tmp = head;
                head = head->next;
                delete tmp;
            }
        }
    }

    bool insert(const K &key, const V &value)
    {
        if (double(num_elements) / buckets.size() > load_factor)
            rehash();

        size_t idx = hash(key);
        Node *head = buckets[idx];
        while (head)
        {
            if (head->key == key)
                return false;
            head = head->next;
        }

        Node *node = new Node(key, value);
        node->next = buckets[idx];
        buckets[idx] = node;
        num_elements++;
        return true;
    }

    V *find(const K &key)
    {
        size_t idx = hash(key);
        Node *head = buckets[idx];
        while (head)
        {
            if (head->key == key)
                return &(head->value);
            head = head->next;
        }
        return nullptr;
    }

    V &operator[](const K &key)
    {
        auto found = find(key);
        if (found)
            return *found;

        insert(key, V{});
        return *find(key);
    }

    bool erase(const K &key)
    {
        size_t idx = hash(key);
        Node *head = buckets[idx];
        Node *prev = nullptr;

        while (head)
        {
            if (head->key == key)
            {
                if (prev)
                    prev->next = head->next;
                else
                    buckets[idx] = head->next;
                delete head;
                num_elements--;
                return true;
            }
            prev = head;
            head = head->next;
        }
        return false;
    }

    size_t size() const { return num_elements; }

public:
    class Iterator
    {
    public:
        using Node = typename UnorderedMap<K, V>::Node;

        Iterator(std::vector<Node *> &buckets, size_t idx, Node *node)
            : buckets_(buckets), idx_(idx), node_(node)
        {
            advance_to_valid();
        }

        std::pair<const K &, V &> operator*() const
        {
            return {node_->key, node_->value};
        }

        Iterator &operator++()
        {
            if (node_)
                node_ = node_->next;
            advance_to_valid();
            return *this;
        }

        bool operator!=(const Iterator &other) const
        {
            return node_ != other.node_;
        }

    private:
        void advance_to_valid()
        {
            while (!node_ && idx_ + 1 < buckets_.size())
            {
                node_ = buckets_[++idx_];
            }
        }

        std::vector<Node *> buckets_;
        size_t idx_;
        Node *node_;
    };

    Iterator begin()
    {
        for (int i = 0; i < buckets.size(); i++)
        {
            if (buckets[i])
            {
                return Iterator(buckets, i, buckets[i]);
            }
        }
        return end();
    }

    Iterator end()
    {
        return Iterator(buckets, buckets.size(), nullptr);
    }
};

int main()
{
    UnorderedMap<std::string, int> umap;
    umap.insert("apple", 10);
    umap["banana"] = 20;
    umap["cherry"] = 30;

    std::cout << "banana: " << umap["banana"] << "\n";

    umap.erase("apple");

    if (umap.find("apple"))
    {
        std::cout << "Found apple\n";
    }
    else
    {
        std::cout << "Apple not found\n";
    }

    for (auto [k, v] : umap)
    {
        std::cout << k << ": " << v << "\n";
    }
}