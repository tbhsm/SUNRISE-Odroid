/*
 * Display.c
 *
 *  Created on: Oct 7, 2015
 *      Author: odroid
 */

#include <stdio.h>
#include <stdlib.h>

#include "Weather.h"
#include "ServerCom.h"

void htmlDisplay(){
	FILE  * fp;
	/*READ WEATHER STATION DATA*/
	uint16_t *weather = readInput();
	/*READ SERVER TO GET INFORMATION ABOUT CHARGER STATUS*/
	int i;
	int charger[7];
	for (i=0;i<6;i++){
		char *b=NULL;
		b  = getChargerState(i+1);
		charger[i]= atoi(b);}
	/*CREATE HTML TO HAVE IMPORTANT INFO*/
	fp = fopen("file.html","w+");

	//fprintf(fp, "Content-type: text/html\n\n");
	fprintf(fp, "<html><title>Hello</title><body>\n");
	fprintf(fp,"<head>\n");
	fprintf(fp,"<meta http-equiv='refresh' content='300'>\n");
	fprintf(fp,"<link rel='stylesheet' type='text/css' href='mystyle.css'>\n");
	fprintf(fp,"<script src='go-debug.js'></script>\n");
	fprintf(fp,"</head>\n");
	fprintf(fp,"<body>\n");


	fprintf(fp,"<div id='header'>\n");
	fprintf(fp,"<div class='background'>\n");
	fprintf(fp,"<h1>TU Delft Bicycle shed</h1>\n");
	fprintf(fp,"</div>\n");
	fprintf(fp,"</div>\n");

	fprintf(fp,"<div id='nav'>\n");
	fprintf(fp,"About<br>\n");
	fprintf(fp,"Weather<br>\n");
	fprintf(fp,"Charger<br>\n");
	fprintf(fp,"</div>");

	fprintf(fp,"<div id='section'>\n");
	fprintf(fp,"<h2>About</h2>\n");
	fprintf(fp,"<p>\n");
	fprintf(fp,"Solar eBike charging station is station which is power neutral. It is able to store solar energy and charge up to 4 electric bikes  and a single scooter.\n");
	fprintf(fp,"</p>\n");
	fprintf(fp,"<p>\n");
	fprintf(fp,"This is very much fun so fun.\n");
	fprintf(fp,"</p>\n");
	fprintf(fp,"</div>\n");
	fprintf(fp,"<div id='myDiagramDiv'>\n");
	fprintf(fp,"<script>\n");
	fprintf(fp,"var $ = go.GraphObject.make;\n");
	fprintf(fp,"var myDiagram = \n");
	fprintf(fp,"$(go.Diagram, 'myDiagramDiv', \n");
	fprintf(fp,"{\n");
	fprintf(fp,"initialContentAlignment: go.Spot.Center,\n");
	fprintf(fp,"'undoManager.isEnabled': true,\n");
	fprintf(fp,"layout: $(go.TreeLayout, \n");
	fprintf(fp,"{ angle: 90, layerSpacing: 25})\n");
	fprintf(fp,"});\n");
	fprintf(fp,"myDiagram.nodeTemplate =\n");
	fprintf(fp,"$(go.Node, 'Horizontal',\n");
	fprintf(fp,"$(go.Picture,\n");
	fprintf(fp,"{ margin: 10, width: 40, height: 40, background: 'white' },\n");
	fprintf(fp,"new go.Binding('source')),\n");
	fprintf(fp,"$(go.TextBlock,\n");
	fprintf(fp,"'Default Text',  \n");
	fprintf(fp,"{ margin: 12, stroke: 'black', font: 'bold 10px sans-serif' },\n");
	fprintf(fp,"new go.Binding('text', 'name'))\n");
	fprintf(fp,");\n");
	fprintf(fp,"var model = $(go.TreeModel);\n");
	fprintf(fp,"model.nodeDataArray =\n");
	fprintf(fp,"[ { key: '1',		name: 'Solar', source: 'solar.png' },\n");
	fprintf(fp,"{ key: '3', parent: '2', name: 'Battery', source: 'battery.GIF' },\n");
	fprintf(fp,"{ key: '2', parent: '1', name: 'Converter',  source: 'converter.gif' },\n");
	fprintf(fp,"{ key: '3', parent: '2', name: 'Charger',  source: 'Charger.png' }\n");
	fprintf(fp,"];\n");
	fprintf(fp,"myDiagram.model = model;\n");
	fprintf(fp,"</script>\n");
	fprintf(fp,"</div>\n");
	fprintf(fp,"<div id='section2'>\n");
	fprintf(fp,"<h2>Weather</h2>\n");
	fprintf(fp,"<p>\n");
	fprintf(fp,"Here will be the info from Weather station. Like\n");
	fprintf(fp,"<div id= 'tabloid'> \n");
	fprintf(fp,"<table id='myTable'>\n");
	fprintf(fp,"<tr>\n");
	fprintf(fp,"<th>Name</th>\n");
	fprintf(fp,"<th>Value</th>\n");
	fprintf(fp,"<th>Name</th>\n");
	fprintf(fp,"<th>Value</th>\n");
	fprintf(fp,"</tr>\n");
	fprintf(fp,"<tr>\n");
	fprintf(fp,"<td>Air Temperature</td>\n");
	fprintf(fp,"<td> %d C</td>\n",*(weather)/10);
	fprintf(fp,"<td>Global Irradiation</td>\n");
	fprintf(fp,"<td>%d </td>\n",*(weather+3)/10);
	fprintf(fp,"</tr>\n");
	fprintf(fp,"<tr>\n");
	fprintf(fp,"<td>Relative Humidity</td>\n");
	fprintf(fp,"<td>%d %</td>\n",*(weather+1)/10);
	fprintf(fp,"<td>Air Pressure</td>\n");
	fprintf(fp,"<td>%d kPa</td>\n",*(weather+4)/10);
	fprintf(fp,"</tr>\n");
	fprintf(fp,"<tr>\n");
	fprintf(fp,"<td>Wind Speed</td>\n");
	fprintf(fp,"<td>%d m/s</td>\n",*(weather+2)/10);
	fprintf(fp,"<td>Absolute Precipitation</td>\n");
	fprintf(fp,"<td>%d </td>\n",*(weather+5)/100);
	fprintf(fp,"</tr>\n");
	fprintf(fp,"</table>\n");
	fprintf(fp,"</div> \n");
	fprintf(fp,"</p>\n");
	fprintf(fp,"</div>\n");
	fprintf(fp,"<div id='section3'>\n");
	fprintf(fp,"<h2>Charger</h2>\n");
	fprintf(fp,"<p>\n");
	fprintf(fp,"<div id='myDiagramDiv2'>\n");
	fprintf(fp,"<script>\n");
	fprintf(fp,"var $ = go.GraphObject.make;\n");
	fprintf(fp,"var myDiagram2 =\n");
	fprintf(fp,"$(go.Diagram, 'myDiagramDiv2',\n");
	fprintf(fp,"{\n");
	fprintf(fp,"initialContentAlignment: go.Spot.Center,\n");
	fprintf(fp,"'undoManager.isEnabled': true,\n");
	fprintf(fp,"layout: $(go.TreeLayout,\n");
	fprintf(fp,"{ angle: 90, layerSpacing: 25})\n");
	fprintf(fp,"});\n");
	fprintf(fp,"myDiagram2.nodeTemplate =\n");
	fprintf(fp,"$(go.Node, 'Horizontal',\n");
	fprintf(fp,"$(go.Picture,\n");
	fprintf(fp,"{ margin: 10, width: 40, height: 40, background: 'white' },\n");
	fprintf(fp,"new go.Binding('source')),\n");
	fprintf(fp,"$(go.TextBlock,\n");
	fprintf(fp,"'Default Text',\n");
	fprintf(fp,"{ margin: 12, stroke: 'black', font: 'bold 10px sans-serif' },\n");
	fprintf(fp,"new go.Binding('text', 'name'))\n");
	fprintf(fp,");\n");
	fprintf(fp,"var model2 = $(go.TreeModel);\n");
	fprintf(fp,"model2.nodeDataArray =\n");
	fprintf(fp,"[ { key: '1',		name: 'Converter', source: 'converter.gif' },\n");
	if((charger[0])==0){
		fprintf(fp,"{ key: '2', parent: '1', name: 'Charger', source: 'Wireless.png' },\n");}else{
			fprintf(fp,"{ key: '2', parent: '1', name: 'Charger', source: 'Wirelesson.png' },\n");
		}
	if((charger[1])==0){
		fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Charger.png' },\n");}else{
			fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Chargeron.png' },\n");
		}
	if((charger[2])==0){
		fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Charger.png' },\n");}else{
			fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Chargeron.png' },\n");
		}
	if((charger[3])==0){
		fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Charger.png' },\n");}else{
			fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Chargeron.png' },\n");
		}
	if((charger[4])==0){
		fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Charger.png' },\n");}else{
			fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Chargeron.png' },\n");
		}
	if((charger[5])==0){
		fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Charger.png' },\n");}else{
			fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Chargeron.png' },\n");
		}
	/*if(*(b+6)==0){
		fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Charger.png' },\n");}else{
			fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Chargeron.png' },\n");
		}*/

	fprintf(fp,"];");
	fprintf(fp,"myDiagram2.model = model2;\n");
	fprintf(fp,"</script>\n");
	fprintf(fp,"</div>\n");

	fprintf(fp,"</p>\n");
	fprintf(fp,"</div>\n");
	fprintf(fp,"<div id='footer'>\n");
	fprintf(fp,"Copyright © TU Delft\n");
	fprintf(fp,"</div>\n");

	fprintf(fp,"</body>	</html>\n");
	fclose(fp);

	/*FREE ALLOCATED MEMORY*/
	//free(f);
	free(weather);
}
void WeatherDisplay(){
	FILE  * fp;
	/*READ WEATHER STATION DATA*/
	uint16_t *weather;
	//weather = (uint16_t*)malloc(6*sizeof(uint16_t));
	weather = readInput();
	/*SEND THE WEATHER DATA TO SERVER*/
	int jj = writeWeatherToServer(weather);
	/*READ SERVER TO GET INFORMATION ABOUT CHARGER STATUS*/
	int i;
	int charger[7];
	for (i=0;i<6;i++){
		char *b=NULL;
		b  = getChargerState(i+1);
		charger[i]= atoi(b);}
	/*CREATE HTML TO HAVE IMPORTANT INFO*/
	fp = fopen("file.html","w+");

	//fprintf(fp, "Content-type: text/html\n\n");
	fprintf(fp, "<html><title>Hello</title><body>\n");
	fprintf(fp,"<head>\n");
	fprintf(fp,"<meta http-equiv='refresh' content='300'>\n");
	fprintf(fp,"<link rel='stylesheet' type='text/css' href='mystyle.css'>\n");
	fprintf(fp,"<script src='go-debug.js'></script>\n");
	fprintf(fp,"</head>\n");
	fprintf(fp,"<body>\n");


	fprintf(fp,"<div id='header'>\n");
	fprintf(fp,"<div class='background'>\n");
	fprintf(fp,"<h1>TU Delft Bicycle shed</h1>\n");
	fprintf(fp,"</div>\n");
	fprintf(fp,"</div>\n");

	fprintf(fp,"<div id='nav'>\n");
	fprintf(fp,"About<br>\n");
	fprintf(fp,"Weather<br>\n");
	fprintf(fp,"Charger<br>\n");
	fprintf(fp,"</div>");

	fprintf(fp,"<div id='section'>\n");
	fprintf(fp,"<h2>About</h2>\n");
	fprintf(fp,"<p>\n");
	fprintf(fp,"Solar eBike charging station is station which is power neutral. It is able to store solar energy and charge up to 4 electric bikes  and a single scooter.\n");
	fprintf(fp,"</p>\n");
	fprintf(fp,"<p>\n");
	fprintf(fp,"This is very much fun so fun.\n");
	fprintf(fp,"</p>\n");
	fprintf(fp,"</div>\n");
	fprintf(fp,"<div id='myDiagramDiv'>\n");
	fprintf(fp,"<script>\n");
	fprintf(fp,"var $ = go.GraphObject.make;\n");
	fprintf(fp,"var myDiagram = \n");
	fprintf(fp,"$(go.Diagram, 'myDiagramDiv', \n");
	fprintf(fp,"{\n");
	fprintf(fp,"initialContentAlignment: go.Spot.Center,\n");
	fprintf(fp,"'undoManager.isEnabled': true,\n");
	fprintf(fp,"layout: $(go.TreeLayout, \n");
	fprintf(fp,"{ angle: 90, layerSpacing: 25})\n");
	fprintf(fp,"});\n");
	fprintf(fp,"myDiagram.nodeTemplate =\n");
	fprintf(fp,"$(go.Node, 'Horizontal',\n");
	fprintf(fp,"$(go.Picture,\n");
	fprintf(fp,"{ margin: 10, width: 40, height: 40, background: 'white' },\n");
	fprintf(fp,"new go.Binding('source')),\n");
	fprintf(fp,"$(go.TextBlock,\n");
	fprintf(fp,"'Default Text',  \n");
	fprintf(fp,"{ margin: 12, stroke: 'black', font: 'bold 10px sans-serif' },\n");
	fprintf(fp,"new go.Binding('text', 'name'))\n");
	fprintf(fp,");\n");
	fprintf(fp,"var model = $(go.TreeModel);\n");
	fprintf(fp,"model.nodeDataArray =\n");
	fprintf(fp,"[ { key: '1',		name: 'Solar', source: 'solar.png' },\n");
	fprintf(fp,"{ key: '3', parent: '2', name: 'Battery', source: 'battery.GIF' },\n");
	fprintf(fp,"{ key: '2', parent: '1', name: 'Converter',  source: 'converter.gif' },\n");
	fprintf(fp,"{ key: '3', parent: '2', name: 'Charger',  source: 'Charger.png' }\n");
	fprintf(fp,"];\n");
	fprintf(fp,"myDiagram.model = model;\n");
	fprintf(fp,"</script>\n");
	fprintf(fp,"</div>\n");
	fprintf(fp,"<div id='section2'>\n");
	fprintf(fp,"<h2>Weather</h2>\n");
	fprintf(fp,"<p>\n");
	fprintf(fp,"Here will be the info from Weather station. Like\n");
	fprintf(fp,"<div id= 'tabloid'> \n");
	fprintf(fp,"<table id='myTable'>\n");
	fprintf(fp,"<tr>\n");
	fprintf(fp,"<th>Name</th>\n");
	fprintf(fp,"<th>Value</th>\n");
	fprintf(fp,"<th>Name</th>\n");
	fprintf(fp,"<th>Value</th>\n");
	fprintf(fp,"</tr>\n");
	fprintf(fp,"<tr>\n");
	fprintf(fp,"<td>Air Temperature</td>\n");
	fprintf(fp,"<td> %d C</td>\n",*(weather)/10);
	fprintf(fp,"<td>Global Irradiation</td>\n");
	fprintf(fp,"<td>%d </td>\n",*(weather+3)/10);
	fprintf(fp,"</tr>\n");
	fprintf(fp,"<tr>\n");
	fprintf(fp,"<td>Relative Humidity</td>\n");
	fprintf(fp,"<td>%d %</td>\n",*(weather+1)/10);
	fprintf(fp,"<td>Air Pressure</td>\n");
	fprintf(fp,"<td>%d kPa</td>\n",*(weather+4)/10);
	fprintf(fp,"</tr>\n");
	fprintf(fp,"<tr>\n");
	fprintf(fp,"<td>Wind Speed</td>\n");
	fprintf(fp,"<td>%d m/s</td>\n",*(weather+2)/10);
	fprintf(fp,"<td>Absolute Precipitation</td>\n");
	fprintf(fp,"<td>%d </td>\n",*(weather+5)/100);
	fprintf(fp,"</tr>\n");
	fprintf(fp,"</table>\n");
	fprintf(fp,"</div> \n");
	fprintf(fp,"</p>\n");
	fprintf(fp,"</div>\n");
	fprintf(fp,"<div id='section3'>\n");
	fprintf(fp,"<h2>Charger</h2>\n");
	fprintf(fp,"<p>\n");
	fprintf(fp,"<div id='myDiagramDiv2'>\n");
	fprintf(fp,"<script>\n");
	fprintf(fp,"var $ = go.GraphObject.make;\n");
	fprintf(fp,"var myDiagram2 =\n");
	fprintf(fp,"$(go.Diagram, 'myDiagramDiv2',\n");
	fprintf(fp,"{\n");
	fprintf(fp,"initialContentAlignment: go.Spot.Center,\n");
	fprintf(fp,"'undoManager.isEnabled': true,\n");
	fprintf(fp,"layout: $(go.TreeLayout,\n");
	fprintf(fp,"{ angle: 90, layerSpacing: 25})\n");
	fprintf(fp,"});\n");
	fprintf(fp,"myDiagram2.nodeTemplate =\n");
	fprintf(fp,"$(go.Node, 'Horizontal',\n");
	fprintf(fp,"$(go.Picture,\n");
	fprintf(fp,"{ margin: 10, width: 40, height: 40, background: 'white' },\n");
	fprintf(fp,"new go.Binding('source')),\n");
	fprintf(fp,"$(go.TextBlock,\n");
	fprintf(fp,"'Default Text',\n");
	fprintf(fp,"{ margin: 12, stroke: 'black', font: 'bold 10px sans-serif' },\n");
	fprintf(fp,"new go.Binding('text', 'name'))\n");
	fprintf(fp,");\n");
	fprintf(fp,"var model2 = $(go.TreeModel);\n");
	fprintf(fp,"model2.nodeDataArray =\n");
	fprintf(fp,"[ { key: '1',		name: 'Converter', source: 'converter.gif' },\n");
	if((charger[0])==0){
		fprintf(fp,"{ key: '2', parent: '1', name: 'Charger', source: 'Wireless.png' },\n");}else{
			fprintf(fp,"{ key: '2', parent: '1', name: 'Charger', source: 'Wirelesson.png' },\n");
		}
	if((charger[1])==0){
		fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Charger.png' },\n");}else{
			fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Chargeron.png' },\n");
		}
	if((charger[2])==0){
		fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Charger.png' },\n");}else{
			fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Chargeron.png' },\n");
		}
	if((charger[3])==0){
		fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Charger.png' },\n");}else{
			fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Chargeron.png' },\n");
		}
	if((charger[4])==0){
		fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Charger.png' },\n");}else{
			fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Chargeron.png' },\n");
		}
	if((charger[5])==0){
		fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Charger.png' },\n");}else{
			fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Chargeron.png' },\n");
		}
	/*if(*(b+6)==0){
		fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Charger.png' },\n");}else{
			fprintf(fp,"{ key: '2', parent: '1', name: 'Charger',  source: 'Chargeron.png' },\n");
		}*/

	fprintf(fp,"];");
	fprintf(fp,"myDiagram2.model = model2;\n");
	fprintf(fp,"</script>\n");
	fprintf(fp,"</div>\n");

	fprintf(fp,"</p>\n");
	fprintf(fp,"</div>\n");
	fprintf(fp,"<div id='footer'>\n");
	fprintf(fp,"Copyright © TU Delft\n");
	fprintf(fp,"</div>\n");

	fprintf(fp,"</body>	</html>\n");
	fclose(fp);

	/*FREE ALLOCATED MEMORY*/
	free(weather);
}

