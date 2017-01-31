//minimal and maximum percentage values for the color
var minValue = 8;
var maxValue = 50;


//calculate colorintensity according to the time
function color_intensity() {
  var currentdate = new Date();
  var hour = currentdate.getHours();

  var n = 0.0; //initialize the percentage value

  if (hour >= 0 && hour < 12) //if hour between 0:00 and 12:00
  {
    n = hour * (maxValue - minValue) / 12.0 + minValue; //linearize from minValue to maxValue simulating the first daycycle
  }

  if (hour >= 12 && hour <= 24) //if hour between 12:00 and 24:00
  {
    n = (hour - 12) * (minValue - maxValue) / 12 + maxValue; //lineaire from maxValue to minValue simulating the second daycycle
  }

  return "hsl(241, 100%," + n + "%)"; //return the colorvalue
}

//background depended on the current time
function dynamic_background() {
  canvas.style.background = color_intensity();
}





