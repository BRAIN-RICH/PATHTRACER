#include "camera.h"
#include "pathtrace.h"
#include "ray.h"
#include "sampler.h"
#include "scene.h"
#include "settings.h"
#include "SDLauxiliary.h"
#include "utils.h"
#include "vector_type.h"

#include <SDL.h>

#include <iostream>
#include <cstdint>
#include <cstring>//声明的名称是在std空间中命名的，可在c++代码中使用c风格的函数
#include <memory>
#include <omp.h>//openMP并行编程
#include <string>

#define RES 650//定义分辨率
#define SCREEN_WIDTH  RES
#define SCREEN_HEIGHT RES

#define FULLSCREEN_MODE false//定义全屏

#undef main // SDL2 compatibility with Windows

// 函数声明
bool Update(screen *screen);//实时更新
void Draw(screen *screen);//绘制程序
void InitialiseBuffer();//初始化缓存
void saveScreenshot(screen *screen);//屏幕截图保存


//采样方法 生成一个对象数组
scg::Sampler sampler[20]; // TODO: !!! find a better solution


//相机
scg::Camera camera{
    scg::Vec3f(0, 0, -240),//相机位置
    scg::Vec3f(0, 0, 0),//相机旋转角度
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    true, // Jitter 抖动  抗锯齿
    0.2f, // Aperture 光圈孔径
    3.0f}; // Focal length  焦距

scg::Vec3f rotation{0, 0, 0};

scg::Settings settings;
scg::Scene scene;

scg::Volume volume(VOLUME_SIZE, VOLUME_SIZE, VOLUME_SIZE);//创建体数据存储
scg::Volume temp(VOLUME_SIZE, VOLUME_SIZE, VOLUME_SIZE);//临时体数据存储

int samples;//采样次数
scg::Vec3f buffer[SCREEN_HEIGHT][SCREEN_WIDTH];//三维向量

int main(int argc, char *argv[])
{
    InitialiseBuffer();//初始化缓存

    screen *screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE);//初始化sdl

    // Initialise scene
	settings = scg::loadSettings();
    scg::loadSettingsFile(settings);//加载设置文件
    scene = scg::loadTestModel(150.0f);//加载康奈尔盒子
    //scg::loadBrain(volume, temp, scene, settings);//加载大脑数据
    //scg::loadManix(volume, temp, scene, settings);//加载
    //scg::loadBunny(volume, temp, scene, settings);

    // 开始主要的循环绘制
    while (Update(screen))
    {
        Draw(screen);
        SDL_Renderframe(screen);
    }

    // 存储和结束进程
    //saveScreenshot(screen);
    KillSDL(screen);

    return 0;
}

void Draw(screen *screen)
{
    ++samples;//采样一次就+1

    // TODO: reseed generator
	//创建线程，for表示后面的for循环将被多线程执行，另每次循环之间不能有联系
    #pragma omp parallel for schedule(dynamic) shared(camera, scene, settings, screen)
    for (int y = 0; y < SCREEN_HEIGHT; ++y)//y方向上的像素
    {
        for (int x = 0; x < SCREEN_WIDTH; ++x)//x方向上的像素
        {
            scg::Ray ray = camera.getRay(x, y, sampler[omp_get_thread_num()]);//获取起始光线
            ray.minT = scg::RAY_EPS;//光线起始时间
            
            ray.origin = scg::rotate(ray.origin, rotation);//旋转光线
            ray.direction = scg::rotate(ray.direction, rotation);//光线方向

            scg::Vec3f colour = scg::trace(scene, ray, settings, sampler[omp_get_thread_num()]);//通过路径追踪求取最终颜色
			//缓存经过伽马矫正的像素颜色,目前为1，待修改
            buffer[y][x] += colour * settings.gamma; // TODO: clamp value

			//将颜色显示到屏幕上
            PutPixelSDL(screen, x, y, buffer[y][x] / samples);
        }
    }
}

bool Update(screen *screen)
{
    static int t = SDL_GetTicks();//获取从SDL库初始化来所经历过的时间，单位为微秒。返回unsigned 32-bit类型
    /* 计算渲染一帧时间 */
    int t2 = SDL_GetTicks();//渲染完一帧的时间
    float dt = float(t2 - t);
    t = t2;//起始时间设置为下一帧
    /*Good idea to remove this*/
    std::cout << "Iteration: " << samples << ". Render time: " << dt << " ms." << std::endl;

    SDL_Event e;//声明一个SDLevent变量
    while (SDL_PollEvent(&e))//读取事件
    {
       if (e.type == SDL_QUIT)//查看事件类型，如果用户点击了关闭。SDL_QUIT用来清空所有SDL占用资源
        {
            return false;
        } else if (e.type == SDL_KEYDOWN)//事件类型若为键盘事件，如果用户按下键盘
        {
            int key_code = e.key.keysym.sym;//根据相应的按键选择surface
            switch (key_code)
            {
                case SDLK_0:
                    settings.renderType = 0;
                    InitialiseBuffer();
                    break;
                case SDLK_1:
                    settings.renderType = 1;
                    InitialiseBuffer();
                    break;
                case SDLK_2:
                    settings.renderType = 2;
                    InitialiseBuffer();
                    break;
                case SDLK_ESCAPE://ESC退出
                    /* Move camera quit */
                    return false;
                case SDLK_w://相机前移
                    /* Move camera forward */
                    camera.position.z += 3.0f;
                    InitialiseBuffer();
                    break;
                case SDLK_s://相机后移
                    /* Move camera backwards */
                    camera.position.z -= 3.0f;
                    InitialiseBuffer();
                    break;
                case SDLK_a://相机左移
                    /* Move camera left */
                    camera.position.x -= 3.0f;
                    InitialiseBuffer();
                    break;
                case SDLK_d://相机右移
                    /* Move camera right */
                    camera.position.x += 3.0f;
                    InitialiseBuffer();
                    break;
                case SDLK_r://加载设置文件
                    InitialiseBuffer();
                    scg::loadSettingsFile(settings);
                    break;
                case SDLK_p://截图
                    saveScreenshot(screen);
                    break;
                case SDLK_UP://向上旋转
                    rotation.x -= 5;
                    if (rotation.x < 0)
                        rotation.x += 360;
                    InitialiseBuffer();
                    break;
                case SDLK_DOWN:
                    rotation.x += 5;
                    if (rotation.x > 360)
                        rotation.x -= 360;
                    InitialiseBuffer();
                    break;
                case SDLK_LEFT:
                    rotation.y += 5;
                    if (rotation.y > 360)
                        rotation.y -= 360;
                    InitialiseBuffer();
                    break;
                case SDLK_RIGHT:
                    rotation.y -= 5;
                    if (rotation.y < 0)
                        rotation.y += 360;
                    InitialiseBuffer();
                    break;
            }
        }
    }
    return true;
}

void InitialiseBuffer()
{
    samples = 0;
    memset(buffer, 0, sizeof(buffer));//初始化缓存区，设置为0
}

void saveScreenshot(screen *screen)
{
    std::string fileName = "screenshot" + std::to_string(samples) + ".bmp";
    SDL_SaveImage(screen, fileName.c_str());//c_str把string转为char，保存
}