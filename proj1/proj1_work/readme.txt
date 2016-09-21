How to run the programs

From terminal:
To compile both the clien and the server run "make"
To compile just the client run "make http_client"
To compile just the server run "make http_server"

CLIENT:
USAGE: ./http_client [-options] server_address port_number
where port_number should be between 1 and 65536, inclusive
Use 80 for http.
Each option should be separated by a space character.
Invalid options are ignored.
Avalaible options:
-p: Prints the RTT for accessing the address before the server's response.
Example: ./http_client www.cnn.com 80
or
Example: ./ http_client –p www.cnn.com 80

SERVER:
USAGE: ./http_server port_number
where port_number should be between 1 and 65536, inclusive.
The higher the better.
Example: ./http_server 5008

If Using cccworks4.wpi.edu:port_number doesn't work
run ifconfig to find ip address (inet addr)
then type in ip_address:port_number in the browser.
or
run ./http_client ip_address port_number

The server is multithreaded.
