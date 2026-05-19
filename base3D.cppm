// base3D.cppm
module; // Начало глобального фрагмента
// #define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h> // Все инклуды ДОЛЖНЫ быть здесь
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <functional>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <print>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
export module base3D;

export struct Options{
    std::string name="Test";

    int posx=SDL_WINDOWPOS_CENTERED,
    posy=SDL_WINDOWPOS_CENTERED,
    width=1290,
    height=720,
    index=-1;

    uint32_t window_flags=SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL,

    sdl_init=SDL_INIT_VIDEO|SDL_INIT_AUDIO,
    sdl_image_init=IMG_INIT_PNG;
};
// Теперь экспортируем нужные функции или структуры
export using SDL_Event = ::SDL_Event; // Экспортируем тип из глобального пространства

export using SDL_Window = ::SDL_Window;
export using SDL_GLContext = ::SDL_GLContext;
// Экспортируем функции (теперь они часть модуля)
export using ::SDL_SetRenderDrawColor;
export using ::SDL_CreateTexture;
export using ::SDL_CreateRGBSurface;
export using ::SDL_CreateTextureFromSurface;
export using ::SDL_GetKeyboardState;
export using ::SDL_SetRelativeMouseMode;
export using ::SDL_GetRelativeMouseState;
export using ::SDL_WarpMouseInWindow;
export using ::SDL_GetTicks;
export using ::SDL_GetTicks64;
export using ::SDL_GetPerformanceCounter;
// export using ::SDL_RenderClear;
// export using ::SDL_RenderPresent;
export using ::SDL_PollEvent;
// export using ::SDL_FillRect;
// export using ::SDL_RenderDrawRect;
// export using ::SDL_RenderFillRect;
export using ::SDL_MapRGB;
export using ::SDL_RenderCopy;
// export using ::SDL_HasIntersection;
export using ::SDL_GetMouseState;
export using ::SDL_GetWindowSize;
export using ::SDL_SetWindowSize;
export using ::SDL_SetWindowPosition;
// export using ::SDL_RenderSetVSync;
export using ::SDL_Delay;
// export using ::SDL_RenderDrawPoint;
export using ::SDL_GL_CreateContext;
export using ::SDL_GL_DeleteContext;
export using ::SDL_GL_SwapWindow;
export using ::SDL_GL_SetSwapInterval;
export using ::SDL_GL_SetAttribute;
export using ::SDL_Quit;

//TexturePtr texture{ SDL_CreateTextureFromSurface(app.render.get(), temp_surface) };
// Экспортируем типы, которые эти функции используют
// export using ::SDL_Renderer;
export using ::SDL_Window;
export using ::SDL_Texture;
export using ::SDL_Surface;
// export using ::SDL_Rect;
export using ::SDL_Color;
// export using ::SDL_Point;
export using ::Uint32;
export using ::IMG_Load;
export unsigned int CENTERED =0x2FFF0000u;
// =============================================

// Универсальный хелпер для SDL ресурсов
export template<typename T, auto Deleter>
using SDL_UniquePtr = std::unique_ptr<T, std::integral_constant<decltype(Deleter), Deleter>>;


// Теперь создание новых типов превращается в элегантный список:
export {
    using WindowPtr  =   SDL_UniquePtr<SDL_Window,  SDL_DestroyWindow>;
    using GLcontextPtr = SDL_UniquePtr<void,        &SDL_GL_DeleteContext>;
    using TexturePtr =   SDL_UniquePtr<SDL_Texture, SDL_DestroyTexture>;
    using SurfacePtr =   SDL_UniquePtr<SDL_Surface, SDL_FreeSurface>;
    using SoundPtr =     SDL_UniquePtr<Mix_Chunk,   Mix_FreeChunk>;
}

export struct StateApp{
    Options default_options;
    WindowPtr window;
    GLcontextPtr context;
    // RenderPtr render;
    bool run=false;
};

export struct SoundEfffect {
    std::string name;
    SoundPtr sound;
};

export struct TextureStorage {
    std::string name;
    // Rect rect;
};

export struct StorageTexture{
    std::map<std::string, TextureStorage> storage;
};

export struct StorageSoundEffect{
    std::map<std::string, SoundEfffect> storage;
};
export void sound_load(std::map<std::string, SoundEfffect>& storage,const std::string& key, const std::string& path) {
    Mix_Chunk* raw = Mix_LoadWAV(path.c_str());
    if (raw) {
        storage[key] = {key, SoundPtr(raw)};
    }
}

export void sound_play(std::map<std::string, SoundEfffect>& storage,const std::string& key) {
    if (storage.contains(key)) { // C++20 фича .contains()
        Mix_PlayChannel(-1, storage[key].sound.get(), 0);
    }
}

export class EventManager {
public:
    using Handler = std::function<void(const SDL_Event&)>;

    // Подписка на событие конкретного типа
    void subscribe(Uint32 eventType, Handler handler) {
        handlers[eventType].push_back(handler);
    }

    // Обработка всех накопившихся событий SDL
    void update() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (handlers.count(event.type)) {
                for (auto& handler : handlers[event.type]) {
                    handler(event);
                }
            }
        }
    }

private:
    std::map<Uint32, std::vector<Handler>> handlers;
};



// export struct Camera{
//     // Переменные состояния камеры
//     glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 0.0f);
//     glm::vec3 cameraFront = glm::vec3(0.0f, -1.0f, -0.5f);
//     glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

//     float yaw = -90.0f, pitch = -0.0f; // Углы поворота
//     float cameraSpeed = 1.0f; // Скорость полета
// };

// export glm::mat4 getView(Camera& camera){
//     return glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);
// }

// export void cameraUpdate(Camera& camera){
//     const uint8_t* state = SDL_GetKeyboardState(NULL);
//     float speed = camera.cameraSpeed;

//     if (state[SDL_SCANCODE_W]) camera.cameraPos += speed * camera.cameraFront;
//     if (state[SDL_SCANCODE_S]) camera.cameraPos -= speed * camera.cameraFront;

//     glm::vec3 right = glm::normalize(glm::cross(camera.cameraFront, camera.cameraUp));
//     if (state[SDL_SCANCODE_A]) camera.cameraPos -= right * speed;
//     if (state[SDL_SCANCODE_D]) camera.cameraPos += right * speed;

//     // Вертикальное движение теперь по Y
//     if (state[SDL_SCANCODE_SPACE]) camera.cameraPos.y += speed;
//     if (state[SDL_SCANCODE_LSHIFT]) camera.cameraPos.y -= speed;
// }






export void initApp(StateApp &app){
    if (SDL_Init(app.default_options.sdl_init) < 0) return;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    if (IMG_Init(app.default_options.sdl_image_init)< 0) return;
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    // reset() сам удалит старое окно, если оно было
    app.window.reset(SDL_CreateWindow(
        app.default_options.name.data(),
        app.default_options.posx, app.default_options.posy,
        app.default_options.width, app.default_options.height,
        app.default_options.window_flags
    ));

    app.context.reset(SDL_GL_CreateContext(app.window.get()));

    // 2. Инициализация GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return;

    if (app.window) app.run = true;
    SDL_GL_SetSwapInterval(1);
}

export void closeApp(){
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}







// =========================================================================================================
//
//  SHADER SYSTEMSHADER SYSTEMSHADER SYSTEMSHADER SYSTEMSHADER SYSTEMSHADER SYSTEMSHADER SYSTEMSHADER SYSTEM
//
// =========================================================================================================
//
export struct Shader {
    std::uint32_t id = 0;
    std::unordered_map<std::string, int32_t> locs; // В OpenGL location — это int32_t, а не uint32_t

    // Запрещаем копирование, чтобы избежать случайного удаления ID в деструкторе
    Shader() = default;
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Разрешаем перемещение (move)
    Shader(Shader&& other) noexcept : id(other.id), locs(std::move(other.locs)) {
        other.id = 0;
    }
    Shader& operator=(Shader&& other) noexcept {
        if (this != &other) {
            if (id) glDeleteProgram(id);
            id = other.id;
            locs = std::move(other.locs);
            other.id = 0;
        }
        return *this;
    }

    bool checkstatus(uint32_t object_id, uint32_t type) {
        int32_t success = 0;
        char info_log[512];

        if (type == GL_COMPILE_STATUS) {
            glGetShaderiv(object_id, type, &success);
            if (!success) {
                glGetShaderInfoLog(object_id, 512, nullptr, info_log);
                std::println("Ошибка компиляции шейдера: {}", info_log);
            }
        }
        else if (type == GL_LINK_STATUS) {
            // Для программы используются другие функции!
            glGetProgramiv(object_id, type, &success);
            if (!success) {
                glGetProgramInfoLog(object_id, 512, nullptr, info_log);
                std::println("Ошибка линковки программы: {}", info_log);
            }
        }
        return static_cast<bool>(success);
    }

    uint32_t compget(const std::string& vss, uint32_t type) {
        const char* src_ptr = vss.c_str();
        uint32_t vs = glCreateShader(type);

        // nullptr вместо &len указывает OpenGL, что строка заканчивается на '\0'
        glShaderSource(vs, 1, &src_ptr, nullptr);
        glCompileShader(vs);

        if (!checkstatus(vs, GL_COMPILE_STATUS)) {
            glDeleteShader(vs);
            return 0;
        }
        return vs;
    }
    void addLocs(const std::vector<std::string>& vecs){
        for (auto a: vecs){
            std::istringstream str(a);
            std::string line;
            std::string uname="";
            while(std::getline(str,line)){
                if(line.contains("uniform")){
                    auto start = line.find('=')+1;
                    auto end = line.find(')');
                    auto str = line.substr(start,end-start);

                    auto startName = line.find_last_of(" ")+1;
                    auto endName = line.find_last_of(";");
                    auto strName = line.substr(startName,endName-startName);

                    //std::println("{}",strName);

                    int32_t loc = glGetUniformLocation(id,strName.c_str());
                    uname = strName;
                    if (loc != -1) {
                        locs.insert_or_assign(uname,loc);
                        // std::println("Зарегистрирован юниформ: {} -> location: {}", uname, loc);
                    }

                }


            }
        }
    }
    void compile_shader(const std::vector<std::string>& vecs) {

        if (vecs.size() < 2) return;

        uint32_t vs = compget(vecs[0], GL_VERTEX_SHADER);
        uint32_t fs = compget(vecs[1], GL_FRAGMENT_SHADER);

        if (vs == 0 || fs == 0) return; // Если хоть один не скомпилировался — выходим

        id = glCreateProgram();
        glAttachShader(id, vs);
        glAttachShader(id, fs);
        glLinkProgram(id);

        // Проверяем линковку у PROGRAM (id), а не у шейдера!
        if (!checkstatus(id, GL_LINK_STATUS)) {
            glDeleteProgram(id);
            id = 0;

            // Чистим за собой промежуточные объекты шейдеров
            glDetachShader(id, vs);
            glDetachShader(id, fs);
            glDeleteShader(vs);
            glDeleteShader(fs);
            return;
        }
        addLocs(vecs);
        // Чистим за собой промежуточные объекты шейдеров
        glDetachShader(id, vs);
        glDetachShader(id, fs);
        glDeleteShader(vs);
        glDeleteShader(fs);
    }
    int32_t get_uniform(const std::string& name){
        auto it = locs.find(name);
        if (it != locs.end()) {
            return it->second;
        }
        return -1;
    }
    // slot — это индекс слота (0, 1, 2...).
    // texture_id — это ID самой текстуры, полученный от glGenTextures
    void set_texture(const std::string& name, int32_t slot, uint32_t texture_id) {
        int32_t loc = get_uniform(name);
        if (loc != -1) {
            // 1. Активируем нужный текстурный слот (GL_TEXTURE0 + 0, GL_TEXTURE0 + 1 и т.д.)
            glActiveTexture(GL_TEXTURE0 + slot);

            // 2. Привязываем текстуру к этому активному слоту
            glBindTexture(GL_TEXTURE_2D, texture_id);

            // 3. Передаем номер слота в юниформ шейдера
            glUniform1i(loc, slot);
        }
    }
    // slot — это индекс слота (0, 1, 2...).
    // texture_id — это ID самой текстуры, полученный от glGenTextures
    void set_texture_array(const std::string& name, int32_t slot, uint32_t texture_id) {
        int32_t loc = get_uniform(name);
        if (loc != -1) {
            // 1. Активируем нужный текстурный слот (GL_TEXTURE0 + 0, GL_TEXTURE0 + 1 и т.д.)
            glActiveTexture(GL_TEXTURE0 + slot);

            // 2. Привязываем текстуру к этому активному слоту
            glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);

            // 3. Передаем номер слота в юниформ шейдера
            glUniform1i(loc, slot);
        }
    }
    void set_int(const std::string& name,int32_t i){
        glUniform1i(get_uniform(name),i);
    }
    // matrix_ptr — указатель на первый элемент массива из 16 float (float[16])
    // transpose — нужно ли транспонировать матрицу.
    // Если ваша либа хранит матрицы по строкам (row-major), передайте true.
    // Если по столбцам (column-major, как GLM), передайте false.
    void set_mat4(const std::string& name, const float* matrix_ptr, bool transpose = false) {
        int32_t loc = get_uniform(name);
        if (loc != -1) {
            // 1 — количество матриц
            // GL_FALSE/GL_TRUE — флаг транспонирования
            glUniformMatrix4fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, matrix_ptr);
        }
    }
    void use(){
        glUseProgram(id);
    }

    ~Shader() {
        if (id) {
            glDeleteProgram(id);
        }
    }
};


export class ShaderInc{
    public:
        // name shader
        std::unordered_map<std::string,Shader> shaders;
        void add(const std::string& name,const std::vector<std::string>& vecs){
            Shader temp;
            temp.compile_shader(vecs);

            // Проверяем, что шейдер вообще скомпилировался, прежде чем портить мапу
            if (temp.id == 0) {
                std::println("Не удалось добавить шейдер '{}': ошибка компиляции.", name);
                return;
            }

            // Перемещаем напрямую, избегая лишнего дефолтного конструирования
            shaders.insert_or_assign(name, std::move(temp));
        }
        // Быстрый метод для активации шейдера по имени
        void use(const std::string& name) {
            auto it = shaders.find(name);
            if (it != shaders.end()) {
                it->second.use(); // Вызывает glUseProgram
            } else {
                std::println("Шейдер с именем '{}' не найден!", name);
            }
        }
        // Быстрый метод для активации шейдера по имени
        void set_int(const std::string& sname,const std::string& uname,int32_t i) {
            auto it = shaders.find(sname);
            if (it != shaders.end()) {
                it->second.set_int(uname,i); // Вызывает glUseProgram
            } else {
                std::println("Шейдер с именем '{}' не найден!", sname);
            }
        }
        // shader_name — имя шейдера в мапе (например, "chunk_shader")
        // uniform_name — имя сэмплера в коде GLSL (например, "u_TexturePack")
        void set_texture(const std::string& shader_name, const std::string& uniform_name, int32_t slot, uint32_t texture_id) {
            auto it = shaders.find(shader_name);
            if (it != shaders.end()) {
                it->second.set_texture(uniform_name, slot, texture_id);
            } else {
                std::println("Шейдер '{}' не найден для установки текстуры!", shader_name);
            }
        }
        // shader_name — имя шейдера в мапе (например, "chunk_shader")
        // uniform_name — имя сэмплера в коде GLSL (например, "u_TexturePack")
        void set_texture_array(const std::string& shader_name, const std::string& uniform_name, int32_t slot, uint32_t texture_id) {
            auto it = shaders.find(shader_name);
            if (it != shaders.end()) {
                it->second.set_texture_array(uniform_name, slot, texture_id);
            } else {
                std::println("Шейдер '{}' не найден для установки текстуры!", shader_name);
            }
        }
        void set_mat4(const std::string& shader_name, const std::string& uniform_name, const float* matrix_ptr, bool transpose = false) {
            auto it = shaders.find(shader_name);
            if (it != shaders.end()) {
                it->second.set_mat4(uniform_name, matrix_ptr, transpose);
            } else {
                std::println("Шейдер '{}' не найден для установки матрицы!", shader_name);
            }
        }
};
// else if(!line.contains("layout")&&!line.contains("buffer")&&line.contains("in")&&line.contains("uniform")){
//     std::println("{}",line);
// }
//
// ==========================================================================================================
//
//  SHADER SYSTEMSHADER SYSTEMSHADER SYSTEMSHADER SYSTEMSHADER SYSTEMSHADER SYSTEMSHADER SYSTEMSHADER SYSTEM
//
// ==========================================================================================================
