gcc -Wall -Wextra -g -o ^
./build/main ^
./src/main.c ^
./src/glad.c ^
./dep/stb_image.c ^
./src/engine/primitives.c ^
./src/engine/file.c ^
./src/engine/shader.c ^
./src/engine/texture.c ^
./src/engine/camera.c ^
./src/game/Player.c ^
-Idep -Ldep -lglfw3 -lgdi32 -lopengl32
