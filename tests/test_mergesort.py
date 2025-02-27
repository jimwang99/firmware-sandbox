def mergesort(arr, left_index=0, right_index=-1):
    if right_index == -1:
        right_index = len(arr) - 1

    print(f"{left_index=}, {right_index=}")

    if left_index >= right_index:
        return

    if left_index == right_index - 1:
        if arr[left_index] > arr[right_index]:
            arr[left_index], arr[right_index] = arr[right_index], arr[left_index]
        return

    # partition
    mid_index = (left_index + right_index) // 2
    mergesort(arr, left_index, mid_index)
    mergesort(arr, mid_index + 1, right_index)

    # merge
    left_arr = arr[left_index : mid_index + 1]
    right_arr = arr[mid_index + 1 : right_index + 1]
    i = j = 0
    k = left_index

    while i < len(left_arr) and j < len(right_arr):
        if left_arr[i] < right_arr[j]:
            arr[k] = left_arr[i]
            i += 1
            k += 1
        else:
            arr[k] = right_arr[j]
            j += 1
            k += 1

    if i < len(left_arr):
        arr[k : k + len(left_arr) - i] = left_arr[i:]
    if j < len(right_arr):
        arr[k : k + len(right_arr) - j] = right_arr[j:]

    print(f"{arr=}")


def test_mergesort():
    arr = [10, 5, 2, 3, 1, 4, 6, 7, 8, 9]
    mergesort(arr)
    assert arr == [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]


if __name__ == "__main__":
    import sys
    import pytest

    # Exit with the pytest's exit code
    sys.exit(pytest.main([__file__, "-s"]))
