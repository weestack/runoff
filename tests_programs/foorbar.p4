messages {
    print_foo;
    print_bar;
    print_foobar;
}

task printer() {
    while(true){
        int print_statement;
        receive {
            case print_foo : print_statement = 0;
            case print_bar : print_statement = 1;
            case print_foobar : print_statement = 2;
        }
        
        switch(print_statement){
            case 0:
                print("foo");
            case 1:
                print("bar");
            case 2:
                print("foobar");
            default:
                print("EERRROR");
                # can not happen
        }
    }
}
            

function calculator() -> void {
    print("Give me a number and I'll foo bar it down to 0");
    int f = (int)input("%d");
    
    for(int i = f; i > 0; i--){
        if(i % 7 == 0 && i % 5 == 0){
            send(print_foobar);
        } elseif(i % 7 == 0){
            send(print_foo);
        } elseif(i % 5 == 0){
            send(print_bar);
        }
    }
    print("I hope you are satisfied");
}