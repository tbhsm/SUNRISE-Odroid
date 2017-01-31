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
        <li class="active">lib-json.c</li>
    
    </ul>

    <div class="source-view">
        <div class="source-header">
            <div class="meta"></div>

            <div class="btn-group pull-right">
                <a href="/gitlist/SUNRISE-groep-B.git/raw/master/newcode/lib-json.c" class="btn btn-small"><i class="icon-file"></i> Raw</a>
                <a href="/gitlist/SUNRISE-groep-B.git/blame/master/newcode/lib-json.c" class="btn btn-small"><i class="icon-bullhorn"></i> Blame</a>
                <a href="/gitlist/SUNRISE-groep-B.git/commits/master/newcode/lib-json.c" class="btn btn-small"><i class="icon-list-alt"></i> History</a>
            </div>
        </div>
                <pre id="sourcecode" language="clike">#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
#include &lt;unistd.h&gt;
#include &lt;string.h&gt;
#include &lt;sys/types.h&gt;
#include &lt;sys/socket.h&gt;
#include &lt;netinet/in.h&gt;
#include &lt;netdb.h&gt;
#include &quot;lib-json.h&quot;

// The libJson constructor
// Fill all the pointers with NULL, since that is free-save
struct libJson* libJson_construct(void) {
  // Malloc the actual struct
  struct libJson* lj = (struct libJson*)malloc(sizeof(struct libJson));
  int i;
  lj-&gt;jsonEncoded = NULL;
  lj-&gt;msg = NULL;
  // The answer buffer is an array, initialize it with zeros
  for(i = 0; i &lt; sizeof(lj-&gt;bufferAnswer) / sizeof(lj-&gt;bufferAnswer[0]); i++)
    lj-&gt;bufferAnswer[i] = '\0';
  lj-&gt;jsonArr = NULL;
  lj-&gt;jsonArrLength = 0;
  lj-&gt;msgBack = NULL;
  // 0 is an existing file descriptor, so initialize in with -1
  lj-&gt;sockfd = -1;
  return lj;
}

// The libJson destructor
// Free all the pointers, close the file descriptor and free the structure
void libJson_destruct(struct libJson* lj) {
  free(lj-&gt;jsonEncoded);
  free(lj-&gt;msg);
  free(lj-&gt;jsonArr);
  free(lj-&gt;msgBack);
  if(lj-&gt;sockfd &gt;= 0)
    close(lj-&gt;sockfd);
  free(lj);
}

/** add the HTTP headers to the message
 * @returns void
 * @param lj the libJson struct
 * @param hostname the site to connect to, e.g. solarpoweredbikes.tudelft.nl
 * @param pagename the page on the site to connect to, e.g. ccon/updatedb.php The whole address is solarpoweredbikes.tudelft.nl/ccon/updatedb.php
 * @param msg the message to prepend the header to
 */
void addHeaders(struct libJson* lj, const char* hostname, const char* pagename, const char* msg) {
  char blueprint[] =
    &quot;POST /%s HTTP/1.1\n&quot;
    &quot;Host: %s\n&quot;
    &quot;Connection: Close\n&quot;
    &quot;Content-type: application/json\n&quot;
    &quot;Content-length: %d\n&quot;
    &quot;\n&quot;
    &quot;%s\n&quot;;
    // determine the length of the final message, and malloc the message. Fill the blueprint afterwards. The filled in blueprint will be set in the msg member of the struct
    int length = strlen(blueprint) + strlen(msg) + strlen(hostname) + strlen(pagename) + 2;
    lj-&gt;msg = (char*)malloc(length * sizeof(char));
    sprintf(lj-&gt;msg, blueprint, pagename, hostname, strlen(msg), msg);
}

/** try to connect to the server, and send the message
 * @returns -1 if error, &gt;= 0 if succes
 * @param lj the libJson struct
 * @param hostname the side to connect to
 * @param portno the port number to connect to
 */
int connectTUDelft(struct libJson* lj, const char* hostname,
                   int portno) {
  int n;
  struct sockaddr_in servAddr;
  struct hostent* server;
  // create the socket for internet
  lj-&gt;sockfd = socket(AF_INET, SOCK_STREAM, 0);
  // return -1 if socket creation failed
  if(lj-&gt;sockfd &lt; 0) {
    perror(&quot;ERROR opening socket&quot;);
    return -1;
  }
  // try to find the host
  server = gethostbyname(hostname);
  // if the host is not found, print an error message, close the socket to prevent communication and return -1.
  // The socket file descriptor is also set to -1 to prevent closing it twice.
  if(server == NULL) {
    fprintf(stderr, &quot;ERROR, cannot find host: %s\n&quot;, hostname);
    close(lj-&gt;sockfd);
    return lj-&gt;sockfd = -1;
  }
  // Initialize the servAddr with zeros
  bzero((char*)&amp;servAddr, sizeof(servAddr));
  // Configure the internet address struct to tell it is actually internet
  servAddr.sin_family = AF_INET;
  // copy the address taken from gethostbyname to the address struct
  bcopy((char*)server-&gt;h_addr,
    (char*)&amp;servAddr.sin_addr.s_addr,
    server-&gt;h_length);
  // the port to connect to (it must be converted to network byte order first with htons
  servAddr.sin_port = htons(portno);
  // Try to connect to the server. If it fails, close the socket and return -1.
  if(connect(lj-&gt;sockfd, (struct sockaddr*)&amp;servAddr, sizeof(servAddr)) &lt; 0) {
    perror(&quot;ERROR connecting&quot;);
    close(lj-&gt;sockfd);
    return lj-&gt;sockfd = -1;
  }
  // Write the message to the server. n will be set to the number of characters sent.
  n = write(lj-&gt;sockfd, lj-&gt;msg, strlen(lj-&gt;msg));
  // If not all characters of the message are sent, close the socket and return -1.
  if(n &lt; strlen(lj-&gt;msg)) {
    perror(&quot;ERROR writing&quot;);
    close(lj-&gt;sockfd);
    return lj-&gt;sockfd = -1;
  }
  return lj-&gt;sockfd; // Status code; sockfd is stored in the struct;
}

// Read 255 characters from internet and store it in the bufferAnswer array in the libJson struct
int readChunk(struct libJson* lj) {
  int i, n, length = sizeof(lj-&gt;bufferAnswer) / sizeof(*lj-&gt;bufferAnswer);
  // initialize the buffer with zeros, to prevent having half the previous message.
  // Note that the buffer is 256 bytes, but 255 will actually be used to enforce the closing '\0'
  for(i = 0; i &lt; length; i++)
    lj-&gt;bufferAnswer[i] = '\0';
  // read from the socket specified by the file descriptor
  n = read(lj-&gt;sockfd, lj-&gt;bufferAnswer, length-1);
  // if nothing read because of an error, return -1 as status
  if(n &lt; 0) {
    perror(&quot;ERROR reading from socket&quot;);
    return -1;
  }
  // else return the number of characters as status.
  return n;
}

// Find the end of the header, which is always two newline characters or two '\r\n' combos if read from a windows computer.
char* findMessage(struct libJson* lj) {
  int i, length = sizeof(lj-&gt;bufferAnswer) / sizeof(*lj-&gt;bufferAnswer)-1;
  for(i = 0; i &lt; length; i++) {
    // If a character is a newline character, look if the following character is either a '\r' or a '\n'. If it is a '\n', return a pointer to the character after it.
    // If it is a '\r', return a pointer to the character after the following '\n'
    if(lj-&gt;bufferAnswer[i] == '\n' &amp;&amp;
        (lj-&gt;bufferAnswer[i+1] == '\n' || lj-&gt;bufferAnswer[i+1] == '\r')) {
      if(lj-&gt;bufferAnswer[i+1] == '\r') return &amp;lj-&gt;bufferAnswer[i+3];
      else return &amp;lj-&gt;bufferAnswer[i+2];
    }
  }
  // End of header is not in this chunk, read the next one
  // If there is an error, or when there is nothing more to read, return NULL.
  if(readChunk(lj) &lt;= 0) return NULL;
  return findMessage(lj);
}

// Get the length of the actual message, given the bufferAnswer with in it the header.
int getContentLength(struct libJson* lj) {
  int i, ret, length = sizeof(lj-&gt;bufferAnswer) / sizeof(*lj-&gt;bufferAnswer)-1;
  for(i = 0; i &lt; length; i++)
    // Check in each line if it starts with &quot;Content-Length&quot;. If it does, scan the number behind it and return it.
    if(lj-&gt;bufferAnswer[i] == '\n') {
      if(strncmp(lj-&gt;bufferAnswer + i+1, &quot;Content-Length: &quot;, strlen(&quot;Content-Length: &quot;)) == 0) {
        sscanf(lj-&gt;bufferAnswer + i+1, &quot;Content-Length: %d&quot;, &amp;ret);
        return ret;
      }
    }
  // No Content-Length found, return -1
  return -1;
}

// Decode the message to find the string containing it.
void jsonDecodeArr(struct libJson* lj) {
  int i, j, k;
  int size = 1;
  char buffer[10];
  // The number of commas in the array determine the length of the array. Since the commas are inbetween the numbers, the size is one bigger.
  for(i = 0; i &lt; strlen(lj-&gt;msgBack); i++) {
    if(lj-&gt;msgBack[i] == ',') size++;
  }
  // Malloc the array in which everything will be decoded.
  lj-&gt;jsonArr = (uint16_t*)malloc(sizeof(uint16_t) * size);
  // If the string only contains the characters '[]', return with a size of 0
  if(strlen(lj-&gt;msgBack) &lt;= 2) {
    lj-&gt;jsonArrLength = 0;
    return;
  }
  i = j = 0;
  // The main decode loop
  while(j &lt; strlen(lj-&gt;msgBack)) {
    // Empty the buffer in which the numbers will be decoded
    for(k = 0; k &lt; 10; k++) buffer[k] = '\0';
    k = 0;
    // If there is a character that is not a number, skip it
    while(j &lt; strlen(lj-&gt;msgBack) &amp;&amp; (lj-&gt;msgBack[j] &lt; '0' || lj-&gt;msgBack[j] &gt; '9')) {
      j++;
    }
    // If there is a character that is a number, copy it over in the buffer
    do {
      buffer[k++] = lj-&gt;msgBack[j++];
    } while(j &lt; strlen(lj-&gt;msgBack) &amp;&amp; lj-&gt;msgBack[j] &gt;= '0' &amp;&amp; lj-&gt;msgBack[j] &lt;= '9');
    // If there is something in the buffer, decode it and put it in the array.
    if(buffer[0] != '\0')
      lj-&gt;jsonArr[i++] = atoi(buffer);
  }
  // Set the size, then return.
  lj-&gt;jsonArrLength = size;
}

// Encode an array of the given length to JSON format. This function has one limitation: The array must contain at least one digit, otherwise a memory error will occur
// To be done in the future.
void jsonEncodeArr(struct libJson* lj, double* arr, int size) {
  int length = 14*size + 1; // Biggest number seen has a size of 12, plus the ', ' makes 14
  int i;
  double cur;
  // Malloc the to be encoded string...
  lj-&gt;jsonEncoded = (char*)malloc(length*sizeof(char));
  // ... and initialize it with zeros
  for(i = 1; i &lt; length; i++) lj-&gt;jsonEncoded[i] = '\0';
  // The array starts with a [, so put it in.
  lj-&gt;jsonEncoded[0] = '[';
  for(i = 0; i &lt; size; i++) {
    // Append ', ' to the string if the first number was put in
    if(i) sprintf(lj-&gt;jsonEncoded, &quot;%s, &quot;, lj-&gt;jsonEncoded);
    if(arr[i] == 0) cur = 0; else cur = arr[i]; // To fix '-0';
    // append the number to the string
    sprintf(lj-&gt;jsonEncoded, &quot;%s%6g&quot;, lj-&gt;jsonEncoded, cur);
  }
  // Append the final ]
  sprintf(lj-&gt;jsonEncoded, &quot;%s]&quot;, lj-&gt;jsonEncoded);
}

// A combination of readChunk, getContentLength and findMessage to fetch the actual message
int readAns(struct libJson* lj) {
  int i, j, length = sizeof(lj-&gt;bufferAnswer) / sizeof(*lj-&gt;bufferAnswer) - 1;
  int size;
  char* tmp;
  // retrieve the first chunk
  if(readChunk(lj) &lt;= 0) return -1;
  // If the server doesn't return 200 (OK), discard the message and buffer it if necessary
  if(getStatusCode(lj) != 200) return -1;
  // Try to get the length of the message. On fail, return with an error code
  if(getContentLength(lj) &lt;= 0) return -1;
  size = sizeof(char) * getContentLength(lj) + 1;
  // malloc the message and initialize it
  lj-&gt;msgBack = (char*)malloc(size);
  for(i=0; i &lt; size; i++) lj-&gt;msgBack[i] = '\0';
  // Find the bottom of the header
  tmp = findMessage(lj);
  if(tmp == NULL) return -1; // Something went wrong, return with nothing
  // copy the rest of the message over in the malloc'ed string
  for(i = 0; i &lt; strlen(tmp); i++) {
    lj-&gt;msgBack[i] = tmp[i];
  }
  // copy the rest over if there more to read
  while(readChunk(lj) &gt; 0) {
    for(j = 0; j &lt; strlen(lj-&gt;bufferAnswer); j++) {
      lj-&gt;msgBack[i++] = lj-&gt;bufferAnswer[j];
    }
  }
  // return succes
  return 1;
}

// The buffer of messages when internet in gone
static char* bufferInternetOutage[BUFFER_LENGTH];
// The pointer past the last message in the buffer
static char** curBuf = bufferInternetOutage;
// The pointer past the array, to check if curBuf is in the array.
static char** endBuf = bufferInternetOutage + BUFFER_LENGTH;

// The buffer for status changes, In the beginning, there are no statuses to send, hence it is initialized with zeros
static int chargerStatusOutage[6] = {0,0,0,0,0,0};

// Add a string to the buffer, and remove the first one if the buffer is full
void addStringToBuffer(struct libJson* lj) {
  // If there is still room left, set the message in the buffer and increment curBuf to point past it.
  if(curBuf &lt; endBuf) { 
    *curBuf = lj-&gt;msg;
    curBuf++;
  } else {
    // Buffer is full, delete the first ones by freeing it and shifting the rest over.
    char** bufIdx = bufferInternetOutage;
    free(bufferInternetOutage[0]);
    // shifting occurs by incrementing the pointer and moving *(bufIdx + 1) to *bufIdx
    while(bufIdx &lt; endBuf - 1) {
      bufIdx[0] = bufIdx[1];
      bufIdx++;
    }
    // The last message, the one prior to endBuf, since this pointer point past the end of the array, is the place where the message should go
    endBuf[-1] = lj-&gt;msg;
  }
  // Prevent freeing (the message has to be sent yet)
  lj-&gt;msg = NULL;
  
}

// Empty the buffer and send every message to the server
void emptyBuffer(void) {
  char hostName[100]; int i, stat;
  // While there actually are messages in the buffer, send them.
  while(bufferHasContent()) {
    // Create a new libJson struct to prevent overwriting messages that still have to be freed
    struct libJson* send = libJson_construct();
    // Curbuf pointed past the last message, decrement it to let it point to the message to be sended.
    curBuf--;
    // The headers were already added, so fill send-&gt;msg manually.
    send-&gt;msg = *curBuf;
    // Fetch the hostname by scanning it out of the message to be send
    for(i = 0; i &lt; 100; i++) hostName[i] = '\0';
    sscanf(send-&gt;msg, &quot;POST /%*s HTTP/1.1 Host: %s Connection %*s&quot;, hostName);
    // Try to connect to the server
    stat = connectTUDelft(send, hostName, 80);
    // on fail, let curbuf point past the message once again
    if(stat &lt; 0) {
      curBuf++;
// TODO: send-&gt;msg = NULL // prevent the message to be freed
      libJson_destruct(send);
      return;
    }
    readAns(send); // Flushing the internal buffer by trying to read from the socket
    // Destruct the structure
    libJson_destruct(send);
  }
}

// curBuf points past the last message, so it can only point to the start of the array if there is no message in it
int bufferHasContent(void) {
  return curBuf &gt; bufferInternetOutage;
}

// A function to free all messages in the buffer.
void cleanupBuffer(void) {
  while(bufferHasContent()) {
    curBuf--;
    free(*curBuf);
  }
}

// returns true if there are status updates to send
int statusHasContent(void) {
  int i;
  for(i = 0; i &lt; 6; i++) {
    // return true if one of the array elements is 1
    if(chargerStatusOutage[i] == 1) return 1;
  }
  return 0;
}

// Copy all 1s in the status buffer. This way, multiple status notifications can be combined into one to send it once.
void addStatusToBuffer(int* status) {
  int i;
  for(i = 0; i &lt; 6; i++) {
    if(status[i] == 1) chargerStatusOutage[i] = 1;
  }
}

// The emptyStatus function set the buffer back to {0,0,0,0,0,0} and sends the status updates in the buffer via the writeChargeToServer
void emptyStatus(void) {
  extern int writeChargeToServer(int*);
  int tmp[6], i;
  // copy the buffer over in a temporary array
  for(i = 0; i &lt; 6; i++)
    tmp[i] = chargerStatusOutage[i];
  // Reset the buffer
  for(i = 0; i &lt; 6; i++)
    chargerStatusOutage[i] = 0;
  // write the array to the server. If something fails, the message will be set back in chargerStatusOutage automatically
  writeChargeToServer(tmp);
}

// Get the status code of the HTTP response
int getStatusCode(struct libJson* lj) {
  int statuscode;
  // The status code always follows &quot;HTTP/1.1 &quot;, so scan it in and return it.
  sscanf(lj-&gt;bufferAnswer, &quot;HTTP/1.1 %d&quot;, &amp;statuscode);
  return statuscode;
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
