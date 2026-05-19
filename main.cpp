import std;
import base3D;

#include <GL/glew.h>
// enum ShaderType{
//     VERTEX,
//     FRAGMENT,
//     GEOMETRY,
//     COMPUTE,
// };
const std::string tvsc = R"(

#version 460 core
#extension GL_ARB_shader_draw_parameters : enable
layout (location = 0) in uint packedData;
layout(std430, binding = 0) buffer PlayerMatrices {
    mat4 models[];
};
layout(std430, binding = 1) buffer ChunkMatrices {
    mat4 models1[];
};


layout (location = 5)uniform mat4 model;
layout (location = 6)uniform mat4 view;
layout (location = 7)uniform mat4 projection;
layout (location = 15)uniform int player;

out float vBrightness;

// Таблица смещений для 4 углов плоскости (0,0) -> (1,0) -> (1,1) -> (0,1)
const vec2 corners[4] = vec2[](
    vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 1.0), vec2(0.0, 1.0)
);
// Создаем массив векторов нормалей
const vec3 normals[6] = vec3[](
    vec3(0, 1, 0),  // 0: TOP
    vec3(0, -1, 0), // 1: BOTTOM
    vec3(0, 0, 1),  // 2: FRONT
    vec3(0, 0, -1), // 3: BACK
    vec3(-1, 0, 0), // 4: LEFT
    vec3(1, 0, 0)   // 5: RIGHT
);

out vec3 vNormal; // Передаем во фрагментный шейдер
out float vAO;
out vec2 uv;
out float vLayer;
out float lightltf;
void main() {
    // 1. Распаковка
    uint x          =  packedData        & 31u;   // 0-4   биты (5 бит)
    uint y          = (packedData >> 5u)  & 31u;   // 5-9   биты (5 бит)
    uint z          = (packedData >> 10u) & 31u;   // 10-14 биты (5 бит)
    uint corner_id  = (packedData >> 15u) & 3u;    // 15-16 биты (2 бита!) -> Маска 3u (11 в битах)
    uint face_id    = (packedData >> 17u) & 7u;    // 17-19 биты (3 бита)
    uint t_id       = (packedData >> 20u) & 63u;   // 20-25 (6 бит) - МАСКА 63u// 6 бит, маска должна быть 63u (2^6 - 1)
    uint l_id       = (packedData >> 26u) & 15u;   // 26-29 (4 бита) - МАСКА 15u// 4 бита, маска должна быть 15u (2^4 - 1)
    uint ao_raw     = (packedData >> 30u) & 3u;    // 30-31 биты (2 бита)
    vec3 base_pos = vec3(x, y, z);
    vec3 final_pos = base_pos;
    uv = corners[corner_id];
    vLayer = float(t_id);
    lightltf = (float(15)) / 15.0;
    vAO = (float(ao_raw)+1.0) / 4.0;//vAO = 0.4 + (float(ao_raw) / 3.0) * 0.6;//vAO = (float(ao_raw)+1.0) / 4.0;
    // 0: TOP, 1: BOTTOM, 2: FRONT, 3: BACK, 4: RIGHT, 5: LEFT
    float faceShadows[6] = float[](
        1.0,   // TOP (Y+) — Самая яркая грань
        0.8,   // BOTTOM (Y-) — Самая темная (но не черная!)
        0.95,  // FRONT (Z+) — Почти полная яркость
        0.95,  // BACK (Z-) — Заметное затенение для объема
        0.9,   // RIGHT (X+) — Среднее значение
        0.89   // LEFT (X-) — Небольшое отличие от RIGHT для четкости углов
    );
    vBrightness = faceShadows[face_id];
    //vAO;
    //vAO = (corner_id == 0u || corner_id == 3u) ? 0.7 : 1.0; // Пример
    if (face_id == 0u)      { final_pos += vec3(uv.x, 1.0, uv.y); }//vBrightness = 1.0; }//TOP
    else if (face_id == 1u) { final_pos += vec3(uv.x, 0.0, uv.y); }//vBrightness = 0.55; }//BOTTOM
    else if (face_id == 2u) { final_pos += vec3(uv.x, uv.y, 1.0); }//vBrightness = 0.85; }//FRONT
    else if (face_id == 3u) { final_pos += vec3(uv.x, uv.y, 0.0); }//vBrightness = 0.75; }//BACK
    else if (face_id == 4u) { final_pos += vec3(1.0, uv.y, uv.x); }//vBrightness = 0.9; }//RIGHT
    else if (face_id == 5u) { final_pos += vec3(0.0, uv.y, uv.x); }//vBrightness = 0.7; }//LEFT
    vNormal = normals[face_id];
    // uint draw_index = uint(gl_InstanceID + gl_BaseInstanceARB);
    //uint draw_index = uint(gl_DrawIDARB);
    if(player == 0)
        //vec3 final_pos1 = vec3(float(final_pos.x) / 2.0, float(final_pos.y) / 2.0, float(final_pos.z) / 2.0);
        gl_Position = projection * view * models1[gl_DrawIDARB] * vec4(final_pos, 1.0);
    else{
        mat4 model1 = models[gl_DrawIDARB];
        gl_Position = projection * view * model1 * vec4(final_pos, 1.0);
    }
}
    )";


const std::string tfsc = R"(
    #version 460 core
    #extension GL_ARB_shader_draw_parameters : enable
    in vec3 vNormal;
    in float vBrightness; // та самая яркость, которую мы считали в VS
    in vec2 uv;
    out vec4 FragColor;
    in float vAO;
    in float vLayer;
    in float lightltf;
    uniform sampler2DArray mySampler;
    uniform vec4 u_Color;
    void main() {
        vec3 baseColor = vec4(0.2, 0.7, 0.3, 1.0).rgb; // Зеленый блок

        // 1. Создаем "минимум выживания" для света
        // Даже если lightltf == 0, мы увидим 0.1 (глубокие сумерки)
        float lightLevel = max(lightltf, 0.1);
        //float lightFactor = pow(lightltf, 1.2);

        // 1. Добавляем "солнце" (направленный свет сверху-сбоку)
        vec3 sunDir = normalize(vec3(0.4, 1.0, 0.3));
        float diff = max(dot(vNormal, sunDir), 0.0)*2;

        // 2. Смешиваем глобальное освещение (Ambient) и солнечное
        float ambient = 0.9;
        float light = ambient + diff * 0.5;

        // 3. Умножаем базовый цвет из текстуры на освещение и яркость грани
        // texture() возвращает vec4, поэтому vec4(color, 1.0) здесь уместен.

        //vec4 texColor = texture(mySampler, vec3(1-uv, vLayer));//vLayer
        vec4 texColor = vec4(0.2, 0.7, 0.3, 1.0);
        vec3 baseRGB;
        if (abs(vLayer - 1.0) < 0.1) { // Безопасное сравнение float
            baseRGB = texColor.ggg * vec3(0.4, 0.8, 0.3);
        } else {
            // Вместо сырого rgb, возьмем яркость, чтобы убрать цветовой шум
            // 0.33 - это усреднение каналов
            float luminance = dot(texColor.rgb, vec3(0.33, 0.33, 0.33));
            baseRGB = vec3(luminance) * texColor.rgb; // Сохранит родной цвет, но "причешет" его
            // Или просто оставь baseRGB = texColor.rgb, если уверен в flat
        }

        vec3 finalRGB = baseRGB * light * vBrightness * vAO * lightLevel;

        // Итоговый цвет с сохранением прозрачности из текстуры
        FragColor = vec4(finalRGB, texColor.a);//
        //FragColor = vec4(vec3(lightltf* vAO), 1.0);
        // FragColor = (lightltf > 0.0) ? vec4(1.0, 0.0, 0.0, 1.0) : vec4(0.0, 0.0, 1.0, 1.0);
    }
)";


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
