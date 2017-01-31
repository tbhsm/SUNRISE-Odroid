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
        <li class="active">lib-json.h</li>
    
    </ul>

    <div class="source-view">
        <div class="source-header">
            <div class="meta"></div>

            <div class="btn-group pull-right">
                <a href="/gitlist/SUNRISE-groep-B.git/raw/master/newcode/lib-json.h" class="btn btn-small"><i class="icon-file"></i> Raw</a>
                <a href="/gitlist/SUNRISE-groep-B.git/blame/master/newcode/lib-json.h" class="btn btn-small"><i class="icon-bullhorn"></i> Blame</a>
                <a href="/gitlist/SUNRISE-groep-B.git/commits/master/newcode/lib-json.h" class="btn btn-small"><i class="icon-list-alt"></i> History</a>
            </div>
        </div>
                <pre id="sourcecode" language="clike">#ifndef LIB_JSON_H
#define LIB_JSON_H
#include &lt;stdint.h&gt;
#define BUFFER_LENGTH 200

struct libJson {
  char* jsonEncoded; // set by the jsonEncodeArr function, read by the writeDataToServer function
  char* msg;         // set by the addHeaders function or by the emptyBuffer function, read by the connectTUDelft function
  char bufferAnswer[256]; // set by the readChunk function, read by the getContentLength, readAns and the findMessage function
  uint16_t* jsonArr;      // set by the jsonDecodeArr function, read by the writeDataToServer function
  int jsonArrLength;      // ^
  char* msgBack;          // set by the readAns function, read by the jsonDecodeArr function
  int sockfd;             // set by the connectTUDelft function, read by the readChunk function
};


struct libJson* libJson_construct(void);
void libJson_destruct(struct libJson*);
void addHeaders(struct libJson*, const char*, const char*, const char*);
int connectTUDelft(struct libJson*, const char*,int);
int readChunk(struct libJson*); 
char *findMessage(struct libJson*);
int getContentLength(struct libJson*);
void jsonDecodeArr(struct libJson*);
void jsonEncodeArr(struct libJson*, double*, int);
int readAns(struct libJson*);
void addStringToBuffer(struct libJson*);
void emptyBuffer(void);
int bufferHasContent(void);
void cleanupBuffer(void);
int statusHasContent(void);
void addStatusToBuffer(int*);
void emptyStatus(void);
int getStatusCode(struct libJson*);
#endif // LIB_JSON_H
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
