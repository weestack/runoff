struct placement {
    int row;
    int height;
    bool exist;
}

messages {
    detected_plactic(struct placement value);
}

task detect_plastic() {
    while(true){
        struct placment new_plastic = analyse_factory_mill();
        if(new_plastic.exist){
            send detected_plactic(new_plastic) to 1234; # This makes no sense :)
        }
    }
}

task blow_plastic_away() {
    while(true){
        struct placement target;
        receive {
            case detected_plactic : target = value;
        }
        blow_away_plastic_at_id(detected_plactic);
    }
}

function blow_away_plastic_at_id(struct placement f) -> void {
    # Should blow a piece of plastic away on a specific placement on the factory mill
}

function analyse_factory_mill() -> struct placement {
    # Should return a placement, with aprpriate values.
}