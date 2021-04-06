function CopyPart(int[5] a, int[10] b, int start, int end) -> void {

}

function merge(int[10] arr, int left, int mid, int right) -> void {
    int i; int j; int k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int [5] LeftHalf;
    int [5] RightHalf;

    # Synes det kunne være nice med funktioner til at koppier Arrays, tænker Copy, koppier det hele og
    # CopyPart(arr1, arr2, index1, index2), koppier arr2[index..index2] over i arr1.

    CopyPart(LeftHalf, arr, 0, n1);

    # Synes helt sikkert vi skal have Array.length.
    CopyPart(RightHalf, arr, n1 + 1, 10);
    i = j = 0;
    k = left;
    while(i < n1 && j < n2){
        if(LeftHalf[i] <= RightHalf[j]){
            arr[k] = LeftHalf[i];
            i++;
        }else{
            arr[k] = RightHalf[j];
            j++;
        }
        k++;
    }

    while(i < n1){
        arr[k] = LeftHalf[i];
        i++;
        k++;
    }

    while(j < n2){
        arr[k] = RightHalf[j];
        j++;
        k++;
    }
}

function mergeSort(int[10] arr, int left, int right) -> void {
	int l;
	int r;

    if(left < right){
        int mid = l + (r - 1) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}