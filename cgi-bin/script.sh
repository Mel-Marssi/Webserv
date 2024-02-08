#!/bin/bash

while(true) do "" ; done ;
echo -e "HTTP/1.1 200 ok\r\nContent-Length: 146\r\nContent-Type: text/html\r\n\r\n"
echo "<!DOCTYPE html>" ;
echo "<html>";
echo "<head>";
echo 	"<title>My Shell Page</title>";
echo "</head>";
echo "<body>";
echo 	"<h1>Welcome to my shell page!</h1>";
echo 	"<p>This is a simple HTML page.</p>";
echo "</body>";
echo "</html>";