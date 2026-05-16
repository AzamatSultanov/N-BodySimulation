/*
--------------------------  asd
Program main part
--------------------------
*/
#include "body.hpp"
#include "quad.hpp"
#include "bhtree.hpp"

#include <raylib.h>
#include <vector>
#include <cmath>
#include <thread>

using namespace std;

#define P_NUMBER 60000 //number of particles simulated
#define FPS 45 

int main(int argc, char* argv[])
{

    // Initialization
    //---------------------------------------------------------
    const int screenWidth = 1400;
    const int screenHeight = 800;
    const int max_radius = 200;
    double center_x = screenWidth / 2.0;
    double center_y = screenHeight / 2.0;
    double G = 200.0;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "N-body gravitational simulation");
    SetTargetFPS(FPS);      

    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    Shader blurShader = LoadShader(0, "assets/shaders/blur.fs");


    vector<Body> particles;

    double left_center_x = center_x - 300; 
    double left_center_y = center_y + 150; 
    for (int i = 0; i < P_NUMBER / 2; i++){ 
        double angle = (double)GetRandomValue(0, 360) / 180 * PI;
        double radius = max_radius * sqrt(GetRandomValue(0, 100000) / 100000.0);
        double y = left_center_y + radius * sin(angle);
        double x = left_center_x + radius * cos(angle);  

        particles.push_back(Body(x, y, 0.0, 0.0, 1.0));
    }

    double right_center_x = center_x + 300; 
    double right_center_y = center_y - 150; 
    for (int i = 0; i < P_NUMBER / 2; i++){ 
        double angle = (double)GetRandomValue(0, 360) / 180 * PI;
        double radius = max_radius * sqrt(GetRandomValue(0, 100000) / 100000.0);
        double y = right_center_y + radius * sin(angle);
        double x = right_center_x + radius * cos(angle);  

        particles.push_back(Body(x, y, 0.0, 0.0, 1.0));
    }

    // for (int i = 0; i < P_NUMBER - 1; i++){
    //     double px, py, dx, dy, dist;
    //     do {
    //         px = (double)GetRandomValue(0, screenWidth);
    //         py = (double)GetRandomValue(0, screenHeight);
    //         dx = px - center_x;
    //         dy = py - center_y;
    //         dist = sqrt(dx*dx + dy*dy);
    //     } while (dist < 100.0);

    //     double orbital_speed = sqrt((G * 10000.0) / dist);

    //     double vx = (-dy / dist) * orbital_speed;
    //     double vy = (dx / dist) * orbital_speed;

    //     particles.push_back(Body(px, py, vx, vy, 1.0));
    // }

    // particles.push_back(Body(center_x - 200, center_y, 0.0, 0.0, 100000.0));

    // for (int i = 0; i < P_NUMBER/4; i++) {
    //     particles.push_back(Body(
    //         double(GetRandomValue(center_x, center_x)),
    //         double(GetRandomValue(center_y, center_y)),
    //         0.0,
    //         0.0,
    //         1.0
    //     ));
    // }
    // for (int i = 0; i < 3 * P_NUMBER/4; i++) {
    //     particles.push_back(Body(
    //         double(GetRandomValue(400, 1000)),
    //         double(GetRandomValue(200, 600)),
    //         0.0,
    //         0.0,
    //         1.0
    //     ));
    // }


    // for (int i = 0; i < P_NUMBER; i++) {
    //     particles.push_back(Body(
    //         double(GetRandomValue(0, screenWidth)),
    //         double(GetRandomValue(0, screenHeight)),
    //         0.0,
    //         0.0,
    //         2.0
    //     ));
    // }
    //----------------------------------------------------------

    // Main Simulation Loop
    while (!WindowShouldClose()) {

        Quad quad(0.0, 0.0, double(screenWidth)); 
        BHTree * root = new BHTree(quad);

        for (int i = 0; i < P_NUMBER; i++) {
            root->insert(particles[i]);
        }

// --- МНОГОПОТОЧНОСТЬ НАЧИНАЕТСЯ ЗДЕСЬ ---
        
        // Узнаем, сколько потоков поддерживает ваш процессор
        unsigned int num_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        int chunk_size = particles.size() / num_threads;

        // 2. Распределяем вычисление сил по потокам
        for (unsigned int t = 0; t < num_threads; ++t) {
            int start = t * chunk_size;
            // Последний поток забирает все оставшиеся частицы
            int end = (t == num_threads - 1) ? particles.size() : (t + 1) * chunk_size;

            threads.push_back(std::thread([&, start, end]() {
                for (int i = start; i < end; ++i) {
                    root->updateForce(particles[i]);
                }
            }));
        }

        // Ждем, пока все потоки закончат вычисления
        for (auto& t : threads) {
            t.join();
        }
        threads.clear(); // Очищаем вектор потоков для следующего шага

        // 3. Распределяем обновление позиций по потокам
        float dt = 0.01;
        for (unsigned int t = 0; t < num_threads; ++t) {
            int start = t * chunk_size;
            int end = (t == num_threads - 1) ? particles.size() : (t + 1) * chunk_size;

            threads.push_back(std::thread([&, start, end, dt]() {
                for (int i = start; i < end; ++i) {
                    particles[i].updatePos(dt);
                }
            }));
        }

        // Ждем завершения обновления позиций
        for (auto& t : threads) {
            t.join();
        }
        
        root->~BHTree();


        BeginTextureMode(target);
            ClearBackground(BLACK);
            
            BeginBlendMode(BLEND_ADDITIVE);
            for (const Body & p : particles) {
                if (p.mass > 100.0) {
                    DrawCircle(
                        (int)p.position_x, 
                        (int)p.position_y, 
                        8.0f,
                        Fade(RAYWHITE, 1.0f)
                    );
                } 
                else {
                    DrawRectangleV(
                        {(float)p.position_x, (float)p.position_y}, 
                        {2.0f, 2.0f},
                        Fade(SKYBLUE, 0.15f)
                    );
                }
            }
            EndBlendMode();
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);

            BeginShaderMode(blurShader);
                DrawTextureRec(
                    target.texture, 
                    { 0, 0, (float)target.texture.width, (float)-target.texture.height }, 
                    { 0, 0 }, 
                    WHITE
                );
            EndShaderMode();

            DrawFPS(10, 10);
        EndDrawing();
    }

    // Cleanup
    UnloadShader(blurShader);
    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}