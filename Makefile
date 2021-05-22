CFLAGS = -std=c++17 -IJumaEngine/source/
CFLAGS_DEBUG =
CFLAGS_RELEASE = -O2 -DNDEBUG
LIBFLAGS = -lGL -lglfw -lGLEW -lIL -lassimp

SOURCE=$(shell find ./JumaEngine/ -name '*.cpp')

JumaEngineDebug: $(SOURCE)
	clang++ $(CFLAGS) $(CFLAGS_DEBUG) -o JumaEngine/bin/JumaEngineDebug $(SOURCE) $(LIBFLAGS)

JumaEngineRelease: $(SOURCE)
	clang++ $(CFLAGS) $(CFLAGS_RELEASE) -o JumaEngine/bin/JumaEngineRelease $(SOURCE) $(LIBFLAGS)

.PHONY: clean

clean:
	rm -f JumaEngine/bin/JumaEngineDebug JumaEngine/bin/JumaEngineRelease