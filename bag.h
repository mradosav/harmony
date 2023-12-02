#include <list>
#include <random>

template <class T>
class Bag {
    private:
        std::list<T> items;

    public:
        Bag() { }

        void insert(T t) {
            items.push_back(t);
        }

        void remove(T t) {
            items.remove(t);
        }

        T pickRandom() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, items.size() - 1);
            auto iter = items.begin();
            std::advance(iter, dist(gen));
            return *iter;
        }

        bool empty() {
            return items.empty();
        }

        typename std::list<T>::iterator begin() {
            return items.begin();
        }

        typename std::list<T>::iterator end() {
            return items.end();
        }
};
