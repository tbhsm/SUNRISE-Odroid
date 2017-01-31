<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="UTF-8" />
        <title>GitList</title>
        <link rel="stylesheet" type="text/css" href="/gitlist/themes/default/css/style.css">
        <link rel="shortcut icon" type="image/png" href="/gitlist/themes/default/img/favicon.png" />
        <!--[if lt IE 9]>
        <script src="/gitlist/themes/default/js/html5.js"></script>
        <![endif]-->
    </head>

    <body>
            <div class="navbar navbar-scroll-top">
    <div class="navbar-inner">
        <div class="container">
            <a class="btn btn-navbar" data-toggle="collapse" data-target=".nav-collapse">
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
                <span class="icon-bar"></span>
            </a>
            <a class="brand" href="/gitlist/">GitList</a>
            <div class="nav-collapse">
                <ul class="nav pull-right">
                    <li><a href="http://gitlist.org/">About</a></li>
                    <li><a href="/gitlist/refresh">Refresh</a></li>
                    <li><a href="https://github.com/klaussilveira/gitlist/issues/new">Report bug</a></li>
                    <li><a href="https://github.com/klaussilveira/gitlist/wiki">Help</a></li>
                </ul>
            </div>
        </div>
    </div>
</div>

    <div class="container">
        <div class="row">
            <div class="span12">
                                <form class="form-search pull-right" action="/gitlist/SUNRISE-groep-B.git/tree/master/search" method="POST">
                    <input type="text" name="query" class="input-medium search-query" placeholder="Search tree..." value="">
                </form>
                
                                    <div class="btn-group pull-left space-right" id="branchList">
    <button class="btn dropdown-toggle" data-toggle="dropdown">browsing: <strong>master</strong> <span class="caret"></span></button>

    <div class="dropdown-menu">
        <div class="search">
            <input class="search" placeholder="Filter branch/tags" autofocus>
        </div>
    <ul class="unstyled list">
    <li class="dropdown-header">Branches</li>
            <li><a href="/gitlist/SUNRISE-groep-B.git/master/"><span class="item">master</span></a></li>
            </ul>
    </div>
</div>                
                <ul class="nav nav-tabs">
    <li class="active"><a href="/gitlist/SUNRISE-groep-B.git/master/">Files</a></li>
    <li><a href="/gitlist/SUNRISE-groep-B.git/commits/master">Commits</a></li>
    <li><a href="/gitlist/SUNRISE-groep-B.git/stats/master">Stats</a></li>
  	<li><a href="/gitlist/SUNRISE-groep-B.git/network?branch=master">Network</a></li>
</ul>
            </div>
        </div>

            <ul class="breadcrumb">
    <li><a href="/gitlist/SUNRISE-groep-B.git/tree/master/">SUNRISE-groep-B.git</a></li>
            <span class="divider">/</span>
        <li><a href="/gitlist/SUNRISE-groep-B.git/tree/master/newcode/">newcode</a></li>
            <span class="divider">/</span>
        <li class="active">ServerCom.c</li>
    
    </ul>

    <div class="source-view">
        <div class="source-header">
            <div class="meta"></div>

            <div class="btn-group pull-right">
                <a href="/gitlist/SUNRISE-groep-B.git/raw/master/newcode/ServerCom.c" class="btn btn-small"><i class="icon-file"></i> Raw</a>
                <a href="/gitlist/SUNRISE-groep-B.git/blame/master/newcode/ServerCom.c" class="btn btn-small"><i class="icon-bullhorn"></i> Blame</a>
                <a href="/gitlist/SUNRISE-groep-B.git/commits/master/newcode/ServerCom.c" class="btn btn-small"><i class="icon-list-alt"></i> History</a>
            </div>
        </div>
                <pre id="sourcecode" language="clike">/*
 * ServerCom.c
 *
 *  Created on: Oct 13, 2015
 *      Author: odroid
 *      Description: Routines to communicate with the Server.
 */


#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
#include &lt;unistd.h&gt;
#include &lt;string.h&gt;
#include &lt;errno.h&gt;
#include &lt;inttypes.h&gt;
#include &lt;time.h&gt;
//#include &lt;/usr/local/include/modbus/modbus.h&gt;
#include &lt;stdint.h&gt;
//#include &lt;curl/curl.h&gt;
#include &lt;sys/syscall.h&gt;
#include &lt;fcntl.h&gt;
#include &quot;ServerCom.h&quot;
#include &quot;lib-json.h&quot;

/*-----------------WEATHER DATA--------------------------*/
// This function sends information from the weatherstation to the server.
int writeWeatherToServer(double *weather) {
// The number of values that will be sended
// retsize is only used to pass a pointer to int
  int size = 12, retsize;
// writeDataToServer returns a malloced pointer. Since it will not be used, free it.
  free(writeDataToServer(weather, size, WEATHER, &amp;retsize));
  return 1;
}
/*----------------------SOLAR TEMPERATURE------------------*/
// This function sends infomation from the temperature sensors from to the server.
int writeSolarToServer(double *solar) {
  int size = 6, retsize, i;
  free(writeDataToServer(solar, size, SOLARTEMP, &amp;retsize));
  return 1;
}
/*----------------------CHARGER STATES-------------------*/
// this function retrieves the current state of chargers.
uint16_t *getChargerState(void) {
  double tmp = 1;
  int size = 1, retsize;
  // the returned array should be freed by the user
  uint16_t* retArray = writeDataToServer(&amp;tmp, size, GETCHARGE, &amp;retsize);
  return retArray;
}
/*----------------------VICTRON DATA chunk no.1---------------------*/
// this function sends part of the information from the victron.
int writeVictron1ToServer(double *data) {
  int size = 36, retsize;
  free(writeDataToServer(data, size, VICTRON1, &amp;retsize));
  return 1;
}

/*----------------------VICTRON DATA chunk no.1.5-------------------*/
// this function sends part of the information from the victron.
int writeVictron15ToServer(double *data) {
  int size = 36, retsize;
  free(writeDataToServer(data, size, VICTRON15, &amp;retsize));
  return 1;
}

/*----------------------VICTRON DATA chunk no.2---------------------*/
// this function sends part of the information from the victron.
int writeVictron2ToServer(double *data) {
  int size = 45, retsize;
  free(writeDataToServer(data, size, VICTRON2, &amp;retsize));
  return 1;
}
/*----------------------VICTRON DATA chunk no.3---------------------*/
// this function sends part of the information from the victron.
int writeVictron3ToServer(double *data) {
  int size = 12, retsize;
  free(writeDataToServer(data, size,  VICTRON3, &amp;retsize));
  return 1;
}
/*----------------------VICTRON DATA chunk no.4---------------------*/
// this function sends part of the information from the victron.
int writeVictron4ToServer(double *data) {
  int size = 9, retsize;
  free(writeDataToServer(data, size, VICTRON4, &amp;retsize));
  return 1;
}

// this function sends the updates for the chargers, when one of them is plugged out.
int writeChargeToServer(int* offarr){
  int size=6, retsize, i;
  double tmp[6];
// copy the values into a double array, since that is requested by the writeDataToServer function.
  for(i = 0; i &lt; 6; i++) tmp[i] = offarr[i];
  free(writeDataToServer(tmp, size, SETCHARGE, &amp;retsize));
  return 1;
}

// Write to the server when the ODROID is starting.
// The tmp=1 is because a bug in the jsonEncode function. That function requires at least a size of one.
int writeStartToServer(void) {
  int size=1, retsize;
  double tmp = 1;
  free(writeDataToServer(&amp;tmp, size, START, &amp;retsize));
  return 1;
}

/** Write general data to the server. This function combines the whole of lib-json.c
 * @returns the read array from internet.
 * @param data the data to be sent
 * @param size the size of the data array
 * @param type the type of the data. The types are defined in &quot;ServerCom.h&quot;
 * @param retSize the size of the returned array. Will be written in the function
 */
uint16_t* writeDataToServer(double *data, int size, int type, int* retSize) {
  // The struct used for the communication
  struct libJson* lj;
  int lengthTime, length, stat, i;
  time_t curTime = time(NULL);
  time_t tmpTime;
  uint16_t* arrBack = NULL;
  // A way to sneak the type number together with the current time and the actual data.
  char* tmpString, *blueprint =
    &quot;{\&quot;type\&quot;: %d,&quot;
    &quot;\&quot;time\&quot;: %d,&quot;
    &quot;\&quot;array\&quot;: %s}&quot;, *hostName =
  &quot;solarpoweredbikes.tudelft.nl&quot;;
  // Determine the length (in characters) of the current unix epoch time.
  for(lengthTime = 0, tmpTime = curTime;
      tmpTime != 0;
      tmpTime /= 10, lengthTime++)
    ;

  // initialize the libJson structure
  lj = libJson_construct();

  // encode the given array in JSON format
  jsonEncodeArr(lj, data, size);
  // determine the length of the string to be sended, for the malloc below
  length = strlen(blueprint) + strlen(lj-&gt;jsonEncoded) + lengthTime;
  // allocate the string for the message.
  tmpString = (char*)malloc(sizeof(char)* length);
  // Fill in the blueprint.
  sprintf(tmpString, blueprint, type, curTime, lj-&gt;jsonEncoded);
  // Add the headers with the hostname, the curret page, and the filled in blueprint
  addHeaders(lj, hostName, &quot;ccon/updatedb.php&quot;, tmpString);
  // tmpString is not necessary anymore, the total message is now stored in the libJson struct
  free(tmpString);
  // Try to connect to the server, and send the string
  stat = connectTUDelft(lj, hostName, 80);
  if(stat &lt; 0) {
    // connection failed
    if(type != GETCHARGE &amp;&amp; type != SETCHARGE) {
      // Add the string to the buffer if it is data from the weather station or from the victron.
      addStringToBuffer(lj);
    } else if(type == SETCHARGE) {
      // If it is an update about the chargers, store it in the special status buffer.
      // The tmp variable in necessary for the cast from double to int.
      int tmp[6];
      for(i=0;i&lt;6;i++) tmp[i] = data[i];
      addStatusToBuffer(tmp);
    }
  } else {
    // Connection succeeded. Read the answer from the function
    stat = readAns(lj);
    if(stat &lt; 0) {
      // If something went wrong with reading the answer, buffer the data with the same code used above.
      if(type != GETCHARGE &amp;&amp; type != SETCHARGE) {
        addStringToBuffer(lj);
      } else if(type == SETCHARGE) {
        int tmp[6];
        for(i=0;i&lt;6;i++) tmp[i] = data[i];
        addStatusToBuffer(tmp);
      }
    } else {
      // Answer read succesfully, decode it from JSON to an actual array.
      jsonDecodeArr(lj);
      // set the retSize, the value passed by pointer.
      *retSize = lj-&gt;jsonArrLength;
      // to be able to cleanup the libJson construct, the array has to be copied over into a new one
      // malloc it here
      arrBack = (uint16_t*)malloc(sizeof(uint16_t)*lj-&gt;jsonArrLength);
      // copy the array over.
      for(i = 0; i &lt; lj-&gt;jsonArrLength; i++) {
        arrBack[i] = lj-&gt;jsonArr[i];
      }
      // Everything is done. Since internet works, empty both the buffers if they have content.
      if(bufferHasContent()) {
        emptyBuffer();
      }
      if(statusHasContent()) {
        emptyStatus();
      }
    }
  }
  // cleanup the libJson struct
  libJson_destruct(lj);
  // return the decoded array, or NULL if something went wrong.
  return arrBack;
}
</pre>
            </div>

    <hr />

        <footer>
    <p>Powered by <a href="https://github.com/klaussilveira/gitlist">GitList 0.4.0</a></p>
</footer>
    </div>
        <script src="/gitlist/themes/default/js/jquery.js"></script>
        <script src="/gitlist/themes/default/js/raphael.js"></script>
        <script src="/gitlist/themes/default/js/bootstrap.js"></script>
        <script src="/gitlist/themes/default/js/codemirror.js"></script>
        <script src="/gitlist/themes/default/js/showdown.js"></script>
        <script src="/gitlist/themes/default/js/table.js"></script>
        <script src="/gitlist/themes/default/js/list.min.js"></script>
        <script src="/gitlist/themes/default/js/main.js"></script>
        <script src="/gitlist/themes/default/js/networkGraph.js"></script>
    </body>
</html>
