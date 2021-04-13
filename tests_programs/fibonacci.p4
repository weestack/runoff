function fibonacci_sequence(int n) -> int {
    if(n == 0){
        return 0;
    } elseif(n == 1){
        return 1;
    } else{
        return (fibonacci_sequence(n - 1) + fibonacci_sequence(n - 2));
    }
}

function setup() -> void {
}
