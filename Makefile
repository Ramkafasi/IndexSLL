# My project
# I made this more general purpose so i can use it with something else afterwards.
SRC = project.c

# Compile without flags unless i want to add -Wall or something
CFLAGS ?=

# Build by architecture
CC_WIN32 = i686-w64-mingw32-gcc
CC_WIN64 = x86_64-w64-mingw32-gcc
CC_LINUX32 = gcc -m32
CC_LINUX64 = gcc -m64

# Output
OUT_WIN32 = win32-project.exe
OUT_WIN64 = win64-project.exe
OUT_LINUX32 = linux32-project
OUT_LINUX64 = linux64-project

# Default
all: win32 win64 linux32 linux64

# Platform specific
win32: $(SRC)
	$(CC_WIN32) $(CFLAGS) $(SRC) -o $(OUT_WIN32)

win64: $(SRC)
	$(CC_WIN64) $(CFLAGS) $(SRC) -o $(OUT_WIN64)

linux32: $(SRC)
	$(CC_LINUX32) $(CFLAGS) $(SRC) -o $(OUT_LINUX32)

linux64: $(SRC)
	$(CC_LINUX64) $(CFLAGS) $(SRC) -o $(OUT_LINUX64)

# Remove the binaries
clean:
	rm -rf $(OUT_WIN32) $(OUT_WIN64) $(OUT_LINUX32) $(OUT_LINUX64)

# Tag targets as phony because they are not a file
.PHONY: all win32 win64 linux32 linux64 clean
