def quicksort(arr, left_index=0, right_index=-1):
    if right_index == -1:
        right_index = len(arr) - 1

    if left_index >= right_index:
        return

    # select pivot
    pivot = arr[right_index]

    # sort pivot
    j = left_index
    i = j - 1

    while True:
        if arr[j] >= pivot:
            j += 1
        else:
            i += 1
            arr[i], arr[j] = arr[j], arr[i]
            j += 1

        if j == right_index:
            i += 1
            arr[i], arr[j] = arr[j], arr[i]
            break

    # partition
    quicksort(arr, left_index, i - 1)
    quicksort(arr, i + 1, right_index)


def test_quicksort():
    arr = [10, 5, 2, 3, 1, 4, 6, 7, 8, 9]
    quicksort(arr)
    assert arr == [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]


if __name__ == "__main__":
    import sys
    import pytest

    # Exit with the pytest's exit code
    sys.exit(pytest.main([__file__, "-s"]))
