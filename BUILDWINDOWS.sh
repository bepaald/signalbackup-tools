# This command once was able to cross compile a working windows binary. But never again. Just keeping it around for future reference.

x86_64-w64-mingw32-g++ -I/usr/x86_64-w64-mingw32/include/ -I/usr/x86_64-w64-mingw32/include/cryptopp/ -Wall -Wextra -static-libgcc -static-libstdc++ -static -std=c++2a -pedantic -fomit-frame-pointer -O3 -s -L/usr/x86_64-w64-mingw32/lib/ -L/usr/x86_64-w64-mingw32/ -o signalbackup-tools.exe */*.cc *.cc -lcryptopp -lsqlite3
