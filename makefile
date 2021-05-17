CC = g++

.PHONY = clean

server: Src/ErrWrap.cpp Src/Server.cpp
	$(CC) $^ -o $@ -pthread

client: Src/ErrWrap.cpp Src/Client.cpp
	$(CC) $^ -o $@

clean:
	rm -rf client server


 