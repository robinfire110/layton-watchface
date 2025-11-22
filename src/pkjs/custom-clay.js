module.exports = function(minified) {
    var clayConfig = this;
    var _ = minified._;
    var $ = minified.$;
    var HTML = minified.HTML;
  
    function toggleSpoiler() {
        if (this.get())
        {
            //Hide normal
            clayConfig.getItemById('cs1').hide();
            clayConfig.getItemById('cs2').hide();
            clayConfig.getItemById('cs3').hide();

            //Show Spoiler
            clayConfig.getItemById('cs1_spoiler').show();
            clayConfig.getItemById('cs2_spoiler').show();
            clayConfig.getItemById('cs3_spoiler').show();
        }
        else 
        {
            //Show normal
            clayConfig.getItemById('cs1').show();
            clayConfig.getItemById('cs2').show();
            clayConfig.getItemById('cs3').show();

            //Hide Spoiler
            clayConfig.getItemById('cs1_spoiler').hide();
            clayConfig.getItemById('cs2_spoiler').hide();
            clayConfig.getItemById('cs3_spoiler').hide();
        }
    }

    function toggleShuffle() {
        //Hide RandomInterval Input (the one that actually gets set in the code)
        var randomInterval = clayConfig.getItemById('random_interval');
        randomInterval.hide();

        //Hide and show based on custom interval status
        var customInterval = clayConfig.getItemById('custom_interval');
        var customIntervalUnit = clayConfig.getItemById('random_interval_unit');
        if (this.get() == 1)
        {
            customInterval.enable();
            customIntervalUnit.enable();
            //Random interval update is done in the onchange event
        }
        else
        {
            customInterval.disable();
            customIntervalUnit.disable();
            randomInterval.set(this.get());
        }

        //Update Interval
        updateRandomInterval();
    }

    function clampTime() {
        //Clamp Time
        var hour = clayConfig.getItemById('random_interval_unit').get();
        var interval = clayConfig.getItemById('custom_interval');
        var value = interval.get();

        //Max Out
        if (hour == 1)
        {
            if (value > 24)
            {
                interval.set(24);
            } 
        }
        else
        {
            if (value > 59)
            {
                interval.set(59);
            } 
        }

        //Min
        if (value < 1)
        {
            interval.set(1);
        }
    }

    function updateRandomInterval() {
        //Clamp amounts
        clampTime();

        //Check for type
        var toggle = clayConfig.getItemById('random_options');
        var customIntervalInput = clayConfig.getItemById('custom_interval');
        var value = -1;
        if (toggle.get() == 0) value = 0;
        else value = customIntervalInput.get();

        //Set
        clayConfig.getItemById('random_interval').set(value);
    }
  
    clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
        //Spoiler Mode
        var spoilerModeToggle = clayConfig.getItemById('spoiler');
        if (spoilerModeToggle)
        {
            toggleSpoiler.call(spoilerModeToggle);
            spoilerModeToggle.on('change', toggleSpoiler);
        }   

        //Character Shuffle
        var shuffleModeToggle = clayConfig.getItemById('random_options');
        toggleShuffle.call(shuffleModeToggle);
        shuffleModeToggle.on('change', toggleShuffle);

        //Update random interval
        var customIntervalInput = clayConfig.getItemById('custom_interval');
        customIntervalInput.on('change', updateRandomInterval);

        //Update random interval on unit change
        var customTimeUnit = clayConfig.getItemById('random_interval_unit');
        customTimeUnit.on('change', clampTime);
    });
    
};