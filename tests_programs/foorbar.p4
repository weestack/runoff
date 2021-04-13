messages {
    print_foo;
    print_bar;
    print_foobar;
}

function print(int wack) -> void {
}

task printer() {
    while(true){
        int print_statement;
        receive {
            case print_foo{} : print_statement = 0;
            case print_bar{} : print_statement = 1;
            case print_foobar{} : print_statement = 2;
        }

		int foo;
		int bar;
		int foobar;
		int EERRROR;
        switch(print_statement){
            case 0:
                print(foo);
            case 1:
                print(bar);
            case 2:
                print(foobar);
            default:
                print(EERRROR);
                # can not happen
        }
    }
}


function calculator(int f, taskid who) -> void {
    for(int i = f; i > 0; i--){
        if(i % 7 == 0 && i % 5 == 0){
            send print_foobar{} to who;
        } elseif(i % 7 == 0){
            send print_foo{} to who;
        } elseif(i % 5 == 0){
            send print_bar{} to who;
        }
    }

}

function setup() -> void {
}
