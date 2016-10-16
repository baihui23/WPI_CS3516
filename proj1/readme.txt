# Project 1: Implement an HTTP client and server running a simplified version of the HTTP/1.1 protocol using Unix Socket Commands (Multi-threaded for extra credit)

1. First compile the program using make from terminal
    To compile both the client and the server run "make"
    To compile just the client run "make http_client"
    To compile just the server run "make http_server"

2. Run each program with the following USAGE
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
    
    Close:
        To close the server use ctrl + c
        Might need to use a different port number, the next time the server is started again.

Note: If Using cccworks4.wpi.edu:port_number doesn't work
      run ifconfig to find ip address (inet addr)
      then type in ip_address:port_number in the browser.
      or
      run ./http_client ip_address port_number

THE SERVER IS MULTITHREADED.
Testing multi threading:
    1. First start the server on port 9000
    2. Run the test script to send requests quickly to the server
