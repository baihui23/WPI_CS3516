How to run the programs

From terminal:
To compile just the client run "make http_client"
To compile just the server run "make http_server"
To compile both run "make"

USAGE: ./http_client [-options] server_address port_number
Example: ./http_client www.cnn.com 80
or
Example: ./ http_client –p www.cnn.com 80
port_number should be between 0 and 65536, inclusive
Use 80 for http.
If port number is invalid, 0 is used
Each option should be separated by a space character.
Invalid options are ignored.
Avalaible options:
-p: Prints the RTT for accessing the address before the server's response.