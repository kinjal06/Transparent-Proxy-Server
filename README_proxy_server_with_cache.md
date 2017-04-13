This program is written for HTTP webproxy server.
proxy server is run using command " webproxy <portnumber>&"
Proxy server handles many client connections using pthread. The thread controller of proxy server connects to the webserver 
as requested by the client. After establishing connection it proxy server recives the data from web server and passes the data to the 
client.
md5 check sum is taken from "http://stackoverflow.com/questions/7627723/how-to-create-a-md5-hash-of-a-string-in-c"