all:
	rm fileServer || true
	gcc -o fileServer main.c -lsqlite3

test: all
	SERVER_NAME="she-a.eu" REQUEST_URI="/fileServer/test" REMOTE_ADDR="127.0.0.1" ./fileServer
test-gratis: all
	SERVER_NAME="she-a.eu" REQUEST_URI="/fileServer/gratis" REMOTE_ADDR="127.0.0.1" ./fileServer
test-subdomain: all
	SERVER_NAME="subdomain.example.com" REQUEST_URI="/fileServer/debug" REMOTE_ADDR="127.0.0.1" ./fileServer
