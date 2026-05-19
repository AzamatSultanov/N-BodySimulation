/*
-------------------------- 
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
#include <fstream>
#include <iostream>

using namespace std;

#define P_NUMBER 40000 //number of particles simulated
#define FPS 45 

void updateForcesThread(BHTree * root, vector<Body> & particles, int start, int end) { // helper function to pass inside thread
    for (int i = start; i < end; i++) {
        root->updateForce(particles[i]);
    } 
}

int main(int argc, char* argv[])
{

    //Reading user config

    if (argc < 2) {
        cout << "Please, enter the filename in the terminal" << endl;
        return 1;
    }

    ifstream input;
    string user_file_name = argv[1];
    string option_num;

    input.open(user_file_name);
    if (!input.is_open()) {
        cout << "Could not open file" << user_file_name << endl;
        return 1;
    }

    getline(input, option_num);

    input.close();


    // Initialization
    //---------------------------------------------------------
    const int screenWidth = 1400;
    const int screenHeight = 800;
    double center_x = screenWidth / 2.0;
    double center_y = screenHeight / 2.0;
    const double G = 200.0;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "N-body gravitational simulation");
    SetTargetFPS(FPS);      

    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    Shader blurShader = LoadShader(0, "assets/shaders/blur.fs");


    vector<Body> particles;

    if (option_num == "galactic-collision") {
        int max_radius = 240;
        double left_center_x = center_x - 350; 
        double left_center_y = center_y + 150; 
        for (int i = 0; i < P_NUMBER / 2; i++){ 
            double angle, radius, x, y, dx, dy, dist;
            do {
                angle = (double)GetRandomValue(0, 359) / 180 * PI;
                radius = max_radius * sqrt(GetRandomValue(0, 100000) / 100000.0);
                y = left_center_y + radius * sin(angle);
                x = left_center_x + radius * cos(angle);

                dx = x - left_center_x;
                dy = y - left_center_y;
                dist = sqrt(dx*dx + dy*dy);
            } while (dist < 10.0);
            particles.push_back(Body(x, y, 0.0, 0.0, 1.0, 'b'));
        }

        double right_center_x = center_x + 350; 
        double right_center_y = center_y - 150; 
        for (int i = 0; i < P_NUMBER / 2; i++){ 
            double angle, radius, x, y, dx, dy, dist;
            do {
                angle = (double)GetRandomValue(0, 359) / 180 * PI;
                radius = max_radius * sqrt(GetRandomValue(0, 100000) / 100000.0);
                y = right_center_y + radius * sin(angle);
                x = right_center_x + radius * cos(angle);

                dx = x - right_center_x;
                dy = y - right_center_y;
                dist = sqrt(dx*dx + dy*dy);
            } while (dist < 10.0);
            particles.push_back(Body(x, y, 0.0, 0.0, 1.0, 'o'));
        }
    } else if(option_num == "galaxy") {
        const int max_radius = 400;
        for (int i = 0; i < P_NUMBER; i++){ 
            double angle, radius, x, y, dx, dy, dist;
            do {
                angle = (double)GetRandomValue(0, 359) / 180 * PI;
                radius = max_radius * sqrt(GetRandomValue(0, 100000) / 100000.0);
                y = center_y + radius * sin(angle);
                x = center_x + radius * cos(angle);

                dx = x - center_x;
                dy = y - center_y;
                dist = sqrt(dx*dx + dy*dy);
            } while (dist < 10.0);
            particles.push_back(Body(x, y, 0.0, 0.0, 1.0, 'b'));
        }
    } else if(option_num == "solar-system") {
        const int max_radius = 500;
        double solar_center_x = center_x; 
        double solar_center_y = center_y; 
        for (int i = 0; i < P_NUMBER - 1; i++){ 
            double angle, radius, x, y, dx, dy, dist;
            do {
                angle = (double)GetRandomValue(0, 359) / 180 * PI;
                radius = max_radius * sqrt(GetRandomValue(0, 100000) / 100000.0);
                y = solar_center_y + radius * sin(angle);
                x = solar_center_x + radius * cos(angle);

                dx = x - solar_center_x;
                dy = y - solar_center_y;
                dist = sqrt(dx*dx + dy*dy);
            } while (dist < 150.0);

            double orbital_speed = sqrt((G * 10000.0) / dist);
            double vx = (-dy / dist) * orbital_speed;
            double vy = (dx / dist) * orbital_speed;

            particles.push_back(Body(x, y, vx, vy, 0.1, 'b'));
        }
        particles.push_back(Body(center_x, center_y, 0.0, 0.0, 10000.0, 'o'));
    } 


    

    //----------------------------------------------------------

    // Main Simulation Loop
    while (!WindowShouldClose()) {

        Quad quad(0.0, 0.0, double(screenWidth)); 
        BHTree * root = new BHTree(quad);

        for (int i = 0; i < P_NUMBER; i++) {
            if (quad.contains(particles[i].position_x, particles[i].position_y)) {
                root->insert(particles[i]);
            }

        }

        //Force update
        int num_threads = thread::hardware_concurrency();
        vector<thread> threads;
        int thread_work = particles.size() / num_threads;

        for (int i = 0; i < num_threads; i++) {
            int start = i * thread_work;
            int end = (i + 1) * thread_work;

            if (i == num_threads - 1) {
                end = particles.size(); //so that the last thread takes all the remaining work
            } 

            threads.push_back(thread(updateForcesThread, root, ref(particles), start, end));
            // threads.push_back(thread([&, start, end]() {
            //     for (int i = start; i < end; ++i) {
            //         root->updateForce(particles[i]);
            //     }
            // }));
        }

        // Wait when they will finish
        for (auto & t : threads) {
            t.join();
        }
        threads.clear(); 


        // for (int i = 0; i < P_NUMBER; i++) {
        //     root->updateForce(particles[i]);
        // }

        // Position update
        float dt = 0.01;
        for (int i = 0; i < P_NUMBER; i++) {
            particles[i].updatePos(dt);
        }
        
        delete root;


        BeginTextureMode(target);
            ClearBackground(BLACK);
            
            BeginBlendMode(BLEND_ADDITIVE);
            for (const Body & p : particles) {
                if (p.mass > 100.0) {
                    DrawCircle(
                        (int)p.position_x, 
                        (int)p.position_y, 
                        4.0f,
                        Fade(WHITE, 0.9f)
                    );
                } else if (p.color == 'b'){
                    DrawPixelV(
                        {(float)p.position_x, (float)p.position_y}, 
                        Fade(SKYBLUE, 0.45f)
                    );

                } else {
                    DrawPixelV(
                        {(float)p.position_x, (float)p.position_y}, 
                        Fade(ORANGE, 0.45f)
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