function updateWeather() {
	$.ajax("../_odroid_server/bin/Debug/localDisplayData.json").done(function (response) {
		console.log(response);
		$("#localTest").html("");
		for (var i in response)
			$("#localTest").append(i + ": " + response[i]).append("<br>");
	});
}

updateWeather();
setInterval(updateWeather, 1000);
