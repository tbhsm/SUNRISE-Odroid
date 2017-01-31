//user defined variables / options. This is cut out of the modelviewer because not every page uses it

//<USER DEFINED VARIABLES>//

//set the value for the debug toggle based on the user preferences
$.post("http://solarpoweredbikes.tudelft.nl/eud/global_php_scripts/read_debug.php", function (response) {
	var toggleDebug = false;
	//console.log(response)
	
	if(!(response == " null" || response == "null") && parseInt(JSON.parse(response).debugmode)) //parse the response
		toggleDebug = true;

	debugMode = toggleDebug;
});

//</USER DEFINED VARIABLES>//