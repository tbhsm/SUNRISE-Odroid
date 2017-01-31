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
        <li class="active">ServerCom.h</li>
    
    </ul>

    <div class="source-view">
        <div class="source-header">
            <div class="meta"></div>

            <div class="btn-group pull-right">
                <a href="/gitlist/SUNRISE-groep-B.git/raw/master/newcode/ServerCom.h" class="btn btn-small"><i class="icon-file"></i> Raw</a>
                <a href="/gitlist/SUNRISE-groep-B.git/blame/master/newcode/ServerCom.h" class="btn btn-small"><i class="icon-bullhorn"></i> Blame</a>
                <a href="/gitlist/SUNRISE-groep-B.git/commits/master/newcode/ServerCom.h" class="btn btn-small"><i class="icon-list-alt"></i> History</a>
            </div>
        </div>
                <pre id="sourcecode" language="clike">/*
 * ServerCom.h
 *
 *  Created on: Oct 13, 2015
 *      Author: odroid
 *      Desription: Routines to communicate with Server.
 */

#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
#include &lt;unistd.h&gt;
#include &lt;string.h&gt;
#include &lt;errno.h&gt;
#include &lt;inttypes.h&gt;
//#include &lt;/usr/local/include/modbus/modbus.h&gt;
#include &lt;stdint.h&gt;
//#include &lt;curl/curl.h&gt;
#include &lt;fcntl.h&gt;

#ifndef SERVERCOM_H_
#define SERVERCOM_H_

// De typenumbers for the writeDataToServer function
// numbers &lt; 10 mean an insert for the database
// 10 &lt;= numbers &lt; 20 mean a retrieve
// 20 &lt;= numbers &lt; 30 mean an update
// numbers &gt;= 100 are special cases. Test and a startup when an error occurs
#define TEST      100
#define START     200
#define WEATHER   0
#define SOLARTEMP 1
#define VICTRON1  2
#define VICTRON15 25
#define VICTRON2  3
#define VICTRON3  4
#define VICTRON4  5
#define GETCHARGE 10
#define SETCHARGE 20

// wrapper functions for the writeDataToServer function
int writeWeatherToServer(double*);
int writeSolarToServer(double*);
int writeVictron1ToServer(double*);
int writeVictron15ToServer(double*);
int writeVictron2ToServer(double*);
int writeVictron3ToServer(double*);
int writeVictron4ToServer(double*);
int writeChargeToServer(int*);
int writeStartToServer(void);

uint16_t* writeDataToServer(double*, int, int,int*);

uint16_t* getChargerState(void);

#endif /* SERVERCOM_H_ */
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
