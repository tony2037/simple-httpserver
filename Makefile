CC = gcc -std=c99
CCFLAGS = -Wall -O3 -g

SERVER= server
CLIENT= client

GIT_HOOKS := .git/hooks/applied

server: $(SERVER).c
	$(CC) -o $(SERVER) $(CCFLAGS)  $(SERVER).c -lpthread

client: $(CLIENT).c
	$(CC) -o $(CLIENT) $(CCFLAGS)  $(CLIENT).c -lpthread

all: $(GIT_HOOKS)
	$(CC) -o $(SERVER) $(CCFLAGS)  $(SERVER).c -lpthread
	$(CC) -o $(CLIENT) $(CCFLAGS)  $(CLIENT).c -lpthread

$(GIT_HOOKS):
	@.githooks/install-git-hooks
	@echo

debug: $(GIT_HOOKS)
	$(CC) -o $(SERVER) $(CCFLAGS) -g $(SERVER).c -lpthread
	$(CC) -o $(CLIENT) $(CCFLAGS) -g $(CLIENT).c -lpthread


clean:
	rm -rf $(SERVER) $(CLIENT)
