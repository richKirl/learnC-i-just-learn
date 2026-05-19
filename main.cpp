import std;
import base3D;

#include <GL/glew.h>
// enum ShaderType{
//     VERTEX,
//     FRAGMENT,
//     GEOMETRY,
//     COMPUTE,
// };

int main(){
    StateApp a;
    initApp(a);
    EventManager eventManagerGame;

    eventManagerGame.subscribe(SDL_QUIT, [&](const SDL_Event&) {
        a.run = false;
    });

    std::vector<std::string> tempv{tvsc,tfsc};
    // Shader shader;

    // shader.compile_shader(tempv);
    ShaderInc shaders;
    shaders.add("main", tempv);
    shaders.use("main");
    shaders.set_int("main","player",1);
    while(a.run){
        eventManagerGame.update();

        SDL_GL_SwapWindow(a.window.get());
    }


    closeApp();
    return 0;
}
