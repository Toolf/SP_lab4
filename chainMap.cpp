#include "chainMap.h"
#include <vector>
#include <map>

// template<typename K, typename V>
// ChainMap<K,V>::ChainMap(){
//     this->stack = std::vector<std::map<K,V> > ();
//     this->stack.push_back(std::map<K,V> ());
// }

// template<typename K, typename V>
// void ChainMap<K,V>::insert(K key, V value){
//     std::map<K,V> &var_map = this->stack[this->stack.size() - 1];
//     var_map.insert(std::pair<K, V>(key, value));
// }

// template<typename K, typename V>
// void ChainMap<K,V>::remove(K key){
//     std::map<K,V> &var_map = this->stack[this->stack.size() - 1];
//     var_map.erase(key);
// }


// template<typename K, typename V>
// void ChainMap<K,V>::new_map(){
//     this->stack.push_back(std::map<K,V>());
// }

// template<typename K, typename V>
// int ChainMap<K,V>::remove_map(){
//     int len = this->stack[this->stack.size()-1].size();
//     this->stack.pop_back();
//     return len;
// }

// template<typename K, typename V>
// void ChainMap<K,V>::clear_all(){
//     this->stack.clear();
// }

// template<typename K, typename V>
// bool ChainMap<K,V>::exist(K key){
//     for(int i=this->stack.size()-1;i>=0;i--){
//         if (this->stack[i].count(key) > 0){
//             return true;
//         }
//     }
//     return false;
// }

// template<typename K, typename V>
// V ChainMap<K,V>::get(K key, V default_value){
//     for(int i=this->stack.size()-1;i>=0;i--){
//         if (this->stack[i].count(key) > 0){
//             return this->stack[i][key];
//         }
//     }
//     return default_value;
// }
