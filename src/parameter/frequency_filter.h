#pragma once
#include "base/countmin.h"
#include "system/postoffice.h"
#include "base/shared_array_inl.h"

namespace PS {

template<typename K>
class FreqencyFilter {
 public:
  // add unique keys with their key count
  void insertKeys(const SArray<K>& key, const SArray<uint32>& count, int n, int k);
  // filter keys using the threadhold *freqency*
  SArray<K> queryKeys(const SArray<K>& key, int freqency);

  void clear() { map_.clear(); count_.clear(); }
 private:
  CountMin<K, uint8> count_;
  std::unordered_map<K, uint32> map_;
};

// countmin implementation

template<typename K>
SArray<K> FreqencyFilter<K>::queryKeys(const SArray<K>& key, int freqency) {
  CHECK_LT(freqency, kuint8max) << "change to uint16 or uint32...";
  SArray<K> filtered_key;
  for (auto k : key) {
    if (count_.query(k) > freqency) filtered_key.pushBack(k);
  }
  return filtered_key;
}

template<typename K>
void FreqencyFilter<K>::insertKeys(
    const SArray<K>& key, const SArray<uint32>& count, int n, int k) {
  if (count_.empty()) {
    double w = (double)FLAGS_num_workers;
    count_.resize(std::max((int)(w * n / log(w+1)), 64), k);
  }
  CHECK_EQ(key.size(), count.size());
  for (size_t i = 0; i < key.size(); ++i) {
    count_.insert(key[i], count[i]);
  }
}

// hash implementation

// template<typename K>
// SArray<K> FreqencyFilter<K>::queryKeys(const SArray<K>& key, int freqency) {
//   SArray<K> filtered_key;
//   for (K k : key) {
//     if (map_[k] > freqency) filtered_key.pushBack(k);
//   }
//   return filtered_key;
// }

// template<typename K>
// void FreqencyFilter<K>::insertKeys(
//     const SArray<K>& key, const SArray<uint32>& count, int n, int k) {
//   CHECK_EQ(key.size(), count.size());
//   for (size_t i = 0; i < key.size(); ++i) {
//     map_[key[i]] += count[i];
//   }
// }

}
