#include <cstdlib>

template <typename T>
void quicksort(vector<T>& arr, const size_t left_index = 0,
               const size_t right_index = -1) {
  if (right_index == -1) {
    right_index = arr.size() - 1;
  }

  if (left_index >= right_index) {
    return;
  }

  // select pivot
  const T pivot = arr[right_index];

  // sort pivot
  size_t j = left_index;
  size_t i = j - 1;

  T temp;
  while (true) {
    if (arr[j] >= pivot) {
      j++;
    } else {
      i++;
      temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
      j++;
    }

    if (j == right_index) {
      i++;
      temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
      break;
    }
  }

  // partition
  quicksort(arr, left_index, i - 1);
  quicksort(arr, i + 1, right_index);
}