pinid BLINKLEDPIN = 4P;
pinid BUTTONLEDPIN = 8P;
pinid BUTTONPIN = 9P;

messages {
    ButtonClick;
    ButtonRelase;
}

task A() {
    while(true){
        receive {
            case ButtonClick : turnOn(BUTTONLEDPIN);
            case ButtonRelase : turnOff(BUTTONLEDPIN);
        }
    }
}

task B(taskid other) {
    while(true){
        bool button_status = readInput(BUTTONPIN);

        if(button_status){
            send ButtonClick{} to other; # FIXME who to send to??
        } else{
            send ButtonRelase{} to other; # FIXME who to send to??
        }
    }
}

task C(){
    while(true){
        turnOn(BUTTONPIN);
        delay(1000);
        turnOff(BUTTONPIN);
        delay(500);
    }
}

function setup() -> void {
    inputPin(BUTTONPIN);
    outputPin(BUTTONLEDPIN);
    outputPin(BLINKLEDPIN);
}