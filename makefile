all:
	rm fileServer || true
	gcc -o fileServer main.c

test: all
	SERVER_NAME="she-a.eu" REQUEST_URI="/path/subpath" REMOTE_ADDR="127.0.0.1" ./fileServer
test-path: all
	SERVER_NAME="she-a.eu" REQUEST_URI="/path/subpath" REMOTE_ADDR="127.0.0.1" ./fileServer
test-subdomain: all
	SERVER_NAME="subdomain.example.com" REQUEST_URI="/debug" REMOTE_ADDR="127.0.0.1" ./fileServer
