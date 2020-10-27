#ifndef CHAIN_MAP_LIB
#define CHAIN_MAP_LIB

#include <vector>
#include <map>
#include <iostream>

template<typename K, typename V>
class ChainMap{
    private:
        std::vector< std::map<K, V> > stack;

    public:
        ChainMap(){
            stack.push_back(std::map<K,V> ());
        }

        void insert(K key, V value){
            std::map<K,V> &var_map = this->stack[this->stack.size() - 1];
            var_map.insert(std::pair<K, V>(key, value));
        }
        void remove(K key){
            std::map<K,V> &var_map = this->stack[this->stack.size() - 1];
            var_map.erase(key);
        }
        void new_map(){
            stack.push_back(std::map<K,V>());
        }
        int remove_map(){
            int len = this->stack[this->stack.size()-1].size();
            this->stack.pop_back();
            return len;
        }
        void clear_all(){
            this->stack.clear();
        }
        bool exist(K key){
            for(int i=this->stack.size()-1;i>=0;i--){
                if (this->stack[i].count(key) > 0){
                    return true;
                }
            }
            return false;
        }
        V get(K key, V default_value){
            for(int i=this->stack.size()-1;i>=0;i--){
                if (this->stack[i].count(key) > 0){
                    return this->stack[i][key];
                }
            }
            return default_value;
        }
        bool exist_in_last_map(K key){
            if (this->stack[this->stack.size()-1].count(key) > 0){
                return true;
            }
            return false;
        }
        std::map<K,V> local_map(){
            return this->stack[this->stack.size()-1];
        }
};

#endif // CHAIN_MAP_LIB
