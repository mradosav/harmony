#include <unordered_map>
#include <random>

template <class T, class S>
class BagSet {
    private:
        std::unordered_map<T, S> items;

    public:
        BagSet() { }

        void insert(T t, S s) {
            items.emplace(t, s);
        }

        bool remove(T t) {
            auto find = items.find(t);
            if (find == items.end()) {
                return false;
            } else {
                items.erase(find);
                return true;
            }
        }

        std::pair<T, S> pickRandom() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, items.size() - 1);
            auto iter = items.begin();
            std::advance(iter, dist(gen));
            return *iter;
        }

        S at(T t) {
            return items.at(t);
        }

        bool empty() {
            return items.empty();
        }

        typename std::unordered_map<T, S>::iterator begin() {
            return items.begin();
        }


        typename std::unordered_map<T, S>::iterator end() {
            return items.end();
        }
};
