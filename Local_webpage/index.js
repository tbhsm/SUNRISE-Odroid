//Path of weatherdata:
var datapath = "../../_odroid_detection/_odroid_detection/bin/Debug/localDisplayData.json";

//The updateinterval for the weatherinformation:
var updateInterval = 5000; //in ms

//initiate the canvas
var canvas = initCanvas("webgl_canvas");


//variable that holds the current windspeed
var windSpeed = 9;
var angle = 180 + 2 * 360; //standard 2x rotation for animation
var invokeWindAnimation = false;

//variables to hold the modelinformation
var compass = new model();
var NOZW = new model();
var windSpeedIndicator = new text();
var sun = new model();
var temperatureIndicator = new text();
var cloud = new model();
var humidityIndicator = new text();


var thermometerHousing = new model();
var thermometerBall = new model();
var thermometerIndicator = new model();
var invokeThermometerAnimation = false;

//load all models, let the callback hell begin! ;)
loadModel("modelviewer/Models/compass", function (model) {
    compass = model;

    loadModel("modelviewer/Models/NOZW", function (model) {
        NOZW = model;

        loadModel("modelviewer/Models/sun", function (model) {
            sun = model;

            loadModel("modelviewer/Models/cloud", function (model) {
                cloud = model;

                loadModel("modelviewer/Models/thermometer_housing", function (model) {
                    thermometerHousing = model;

                    loadModel("modelviewer/Models/thermometer_ball", function (model) {
                        thermometerBall = model;

                        loadModel("modelviewer/Models/thermometer_indicator", function (model) {
                            thermometerIndicator = model;

                            getWeather(function (response) {
                                //console.log(response);

                                //create a textfield
                                var airTemp = 23;

                                createTextField("windSpeedIndicator", response["wind speed"], "red", transparentColor, 400, "center", "sans", 0, 0, 0, 0, function (text) {
                                    windSpeedIndicator = text;

                                    //create a textfield
                                    createTextField("temperatureIndicator", response["temperature"] + "\xB0" + "C", "red", transparentColor, 300, "center", "sans", 0, 0, 0, 0, function (text) {
                                        temperatureIndicator = text;

                                        createTextField("humidityIndicator", response["humidity"] + "%", "rgb(0,230,255)", transparentColor, 300, "center", "sans", 0, 0, 0, 0, function (text) {
                                            humidityIndicator = text;

                                            //update the angle of the compass
                                            angle = angle + parseFloat(response["wind direction"]);

                                            //initiate the modelviewer
                                            startWebGL();

                                            //rescaleModels(); //rescale all models so they fit the screen  

                                            temperatureIndicator.translate(0.4, 1.2, -2.2);

                                            //add all thermometer elements as a child to the housing
                                            thermometerHousing.addChild(thermometerBall);
                                            thermometerHousing.addChild(thermometerIndicator);
                                            thermometerHousing.addChild(temperatureIndicator);
                                            thermometerHousing.translate(-3.5, -3, 4);

                                            //set thermometer in the middle (0 degrees celcius)
                                            thermometerIndicator.scaleLocal(1.0, 0.51, 1.0);

                                            thermometerIndicator.scaleLocal(1.0, returnThermoScale(response["temperature"]), 1.0); //calculate the thermoscale depended on the temparature and scale the indicator accordingly

                                            //thermometerIndicator.scaleLocal(1.0,thermoScale*response.airTemp,1.0);
                                            //thermoScale = returnThermoScale(response.airTemp); //calculate the thermoscale for the animation

                                            cloud.rotateLocal(0, 90, 0);

                                            NOZW.addChild(compass);//add the compass model as child to the NOZW
                                            NOZW.addChild(windSpeedIndicator)//add the windspeed indicator as child to the NOXW

                                            //translate the humidity indicator so it will be in front of the sun
                                            humidityIndicator.translate(-1.0, -0.3, 0.8);
                                            humidityIndicator.scaleLocal(1.5, 1.5, 1.5);

                                            cloud.addChild(humidityIndicator); //add the humidity indicator as a child to the cloud
                                            cloud.translate(0, 1, 0);

                                            NOZW.translate(-1, 1, 0); //translate the NOZW indicators (and its children)

                                            //NOZW.scale(1, 1, 1); //scale the NOZW indicators (and its children)

                                            //coordinates of webgl go from -1 to 1
                                            NOZW.positionGlobal(4, -3, 0);

                                            //translate the temperature indicator so it will be in front of the sun
                                            temperatureIndicator.translate(0, 0, 3);

                                            temperatureIndicator.translate(0, returnTemperatureIndicatorPosition(response["temperature"]), 0);

                                            //temperatureIndicator.scaleLocal(2,2,1);

                                            //sun.addChild(temperatureIndicator); //add the temperature indicator as a child to the sun
                                            //translate the sun
                                            sun.translate(5, 4, 5);

                                            invokeWindAnimation = true;

                                            setTimeout(updateWeather, 15000); //start the first weather update after 15sec

                                        });
                                    });
                                });
                                // }
                                //else {
                                //  alert("Er is iets fout gegaan met het ophalen van de weerinformatie, probeer het opnieuw");
                                // }
                                // });
                            });
                        });
                    });
                });
            });
        });
    });

})


function updateWeather() {
    console.log("Updating the weather...");
    getWeather(function(response){ //get the new weather information
        //update all textfields with the new information
        windSpeedIndicator.updateTextField(response["wind speed"]);
        temperatureIndicator.updateTextField(response["temperature"] + "\xB0" + "C");
        humidityIndicator.updateTextField(response["humidity"] + "%");
        
        //update the compass direction:
        compass.angleLocal(0,0,180+parseFloat(response["wind direction"]));
        
        setTimeout(updateWeather, updateInterval); //update every updateInterval ms
    })
}

//get the weather information
function getWeather(callback) {
    $.ajax(datapath + "?cachebreaker:" + Math.random()).done(function (response) { //breaking the cache like a boss 
        callback(response);
    });
}


function returnThermoScale(temperature) {
    //thermometer scale (implementing a range from -40 to 50 degrees celcius): 2.00 is the scale when the thermometer displays 50 (max), 0.00 is the scale when the thermometer displays -40 (min).
    return temperature * (2.0 / 90) + 40 * (2.0 / 90);
}

//returns the right y position of the thermometer text so it moves with the indicator
function returnTemperatureIndicatorPosition(temperature) {
    //0 -> bottom 7-> top
    return temperature * (7.0 / 90) + 40 * (7.0 / 90);
}


var compassAnimationTime = 0;
var z = 0;
var count = 0;

//function gets called every renderloopiteration
function fixedUpdate(deltaTime) {
    if (invokeWindAnimation && z < angle) //only execute the animation when the angle has not reached the final point yet
    {
        z = easeOutBounce(compassAnimationTime, 0, angle, 1); //animation for the compass
        compass.angleLocal(0, 0, z); //adjust the local angle
        compassAnimationTime = compassAnimationTime + deltaTime / 10; //keep track of the time
    }
}


//EasOutBounce function. t=time,b=begin value,c=destination-b,d=duration
function easeOutBounce(t, b, c, d) { //source: http://stackoverflow.com/questions/2949636/how-to-use-this-easeoutbounce-method
    if ((t /= d) < (1 / 2.75)) {
        return c * (7.5625 * t * t) + b;
    } else if (t < (2 / 2.75)) {
        return c * (7.5625 * (t -= (1.5 / 2.75)) * t + .75) + b;
    } else if (t < (2.5 / 2.75)) {
        return c * (7.5625 * (t -= (2.25 / 2.75)) * t + .9375) + b;
    } else {
        return c + b; //edited line
    }
};
