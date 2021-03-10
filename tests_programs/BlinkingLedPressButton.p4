const BLINKLEDPIN 4;
const BUTTONLEDPIN 8;
const BUTTONPIN 9;

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
    
task B() {
    while(true){
        bool button_status = BinaryPin(BUTTONPIN);
        
        if(button_status){
            send(ButtonClick);
        } else{
            send(ButtonRelase);
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