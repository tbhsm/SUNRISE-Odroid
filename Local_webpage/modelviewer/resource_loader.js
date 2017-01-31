var breakCache = false; //turn the cachebreaker of or on
var cachebreaker = "";
if(breakCache){
cachebreaker = '?cachebreaker=' + Math.random();
}

//load a text file
var loadText = function (url, callback) {

    var request = new XMLHttpRequest();
    request.open('GET', url + cachebreaker, true); //breaks the cache
    //request callback
    request.onload = function () {
        //check for errors
        if (request.status > 299 || request.status < 200) {
            callback("Error. HTTP Status: " + request.status);
        } else {
            callback(null, request.responseText);
        }
    };
    request.send();
};

var loadImage = function (url, callback) {
    var img = new Image();
    img.onload - function () {
        callback(null, image);
    }
    img.src = url;
};

var loadJSON = function (url, callback) {
    loadText(url, function (err, result) {
        if (err) {
            //pass the errorcode
            callback(err);
        } else {
            callback(null, JSON.parse(result));
        }

    });

};
