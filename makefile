CC = g++

.PHONY = clean
TEST = 0

server: Src/ErrWrap.cpp Src/Server.cpp
	$(CC) $^ -o $@ -pthread

client: Src/ErrWrap.cpp Src/Client.cpp
	$(CC) $^ -o $@

client_test1: Src/ErrWrap.cpp Src/Client.cpp Src/Client1_test.cpp
	$(CC) $^ -o $@ -pthread
	./client_test1 $(TEST)
#Must be executed first:
client_test2: Src/ErrWrap.cpp Src/Client.cpp Src/Client2_test.cpp
	$(CC) $^ -o $@ -pthread
	./client_test2 $(TEST)
clean:
	rm -rf client server


 