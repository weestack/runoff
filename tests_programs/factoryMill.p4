struct placement {
    int row;
    int height;
    bool exist;
}

messages {
    detected_plactic{struct placement value};
}

function blow_away_plastic_at_id(struct placement f) -> void {
    # Should blow a piece of plastic away on a specific placement on the factory mill
}

function analyse_factory_mill() -> struct placement {
    # Should return a placement, with aprpriate values.
}

task detect_plastic(taskid blower) {
    while(true){
        struct placement new_plastic = analyse_factory_mill();
        if(new_plastic.exist){
            send detected_plactic{new_plastic} to blower;
        }
    }
}

task blow_plastic_away() {
    while(true){
        struct placement target;
        receive {
            case detected_plactic{value} : target = value;
        }
        blow_away_plastic_at_id(target);
    }
}
