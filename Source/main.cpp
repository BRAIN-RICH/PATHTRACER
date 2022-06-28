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
#include <cstring>//��������������std�ռ��������ģ�����c++������ʹ��c���ĺ���
#include <memory>
#include <omp.h>//openMP���б��
#include <string>

#define RES 650//����ֱ���
#define SCREEN_WIDTH  RES
#define SCREEN_HEIGHT RES

#define FULLSCREEN_MODE false//����ȫ��

#undef main // SDL2 compatibility with Windows

// ��������
bool Update(screen *screen);//ʵʱ����
void Draw(screen *screen);//���Ƴ���
void InitialiseBuffer();//��ʼ������
void saveScreenshot(screen *screen);//��Ļ��ͼ����


//�������� ����һ����������
scg::Sampler sampler[20]; // TODO: !!! find a better solution


//���
scg::Camera camera{
    scg::Vec3f(0, 0, -240),//���λ��
    scg::Vec3f(0, 0, 0),//�����ת�Ƕ�
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    true, // Jitter ����  �����
    0.2f, // Aperture ��Ȧ�׾�
    3.0f}; // Focal length  ����

scg::Vec3f rotation{0, 0, 0};

scg::Settings settings;
scg::Scene scene;

scg::Volume volume(VOLUME_SIZE, VOLUME_SIZE, VOLUME_SIZE);//���������ݴ洢
scg::Volume temp(VOLUME_SIZE, VOLUME_SIZE, VOLUME_SIZE);//��ʱ�����ݴ洢

int samples;//��������
scg::Vec3f buffer[SCREEN_HEIGHT][SCREEN_WIDTH];//��ά����

int main(int argc, char *argv[])
{
    InitialiseBuffer();//��ʼ������

    screen *screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE);//��ʼ��sdl

    // Initialise scene
	settings = scg::loadSettings();
    scg::loadSettingsFile(settings);//���������ļ�
    scene = scg::loadTestModel(150.0f);//���ؿ��ζ�����
    //scg::loadBrain(volume, temp, scene, settings);//���ش�������
    //scg::loadManix(volume, temp, scene, settings);//����
    //scg::loadBunny(volume, temp, scene, settings);

    // ��ʼ��Ҫ��ѭ������
    while (Update(screen))
    {
        Draw(screen);
        SDL_Renderframe(screen);
    }

    // �洢�ͽ�������
    //saveScreenshot(screen);
    KillSDL(screen);

    return 0;
}

void Draw(screen *screen)
{
    ++samples;//����һ�ξ�+1

    // TODO: reseed generator
	//�����̣߳�for��ʾ�����forѭ���������߳�ִ�У���ÿ��ѭ��֮�䲻������ϵ
    #pragma omp parallel for schedule(dynamic) shared(camera, scene, settings, screen)
    for (int y = 0; y < SCREEN_HEIGHT; ++y)//y�����ϵ�����
    {
        for (int x = 0; x < SCREEN_WIDTH; ++x)//x�����ϵ�����
        {
            scg::Ray ray = camera.getRay(x, y, sampler[omp_get_thread_num()]);//��ȡ��ʼ����
            ray.minT = scg::RAY_EPS;//������ʼʱ��
            
            ray.origin = scg::rotate(ray.origin, rotation);//��ת����
            ray.direction = scg::rotate(ray.direction, rotation);//���߷���

            scg::Vec3f colour = scg::trace(scene, ray, settings, sampler[omp_get_thread_num()]);//ͨ��·��׷����ȡ������ɫ
			//���澭��٤�������������ɫ,ĿǰΪ1�����޸�
            buffer[y][x] += colour * settings.gamma; // TODO: clamp value

			//����ɫ��ʾ����Ļ��
            PutPixelSDL(screen, x, y, buffer[y][x] / samples);
        }
    }
}

bool Update(screen *screen)
{
    static int t = SDL_GetTicks();//��ȡ��SDL���ʼ��������������ʱ�䣬��λΪ΢�롣����unsigned 32-bit����
    /* ������Ⱦһ֡ʱ�� */
    int t2 = SDL_GetTicks();//��Ⱦ��һ֡��ʱ��
    float dt = float(t2 - t);
    t = t2;//��ʼʱ������Ϊ��һ֡
    /*Good idea to remove this*/
    std::cout << "Iteration: " << samples << ". Render time: " << dt << " ms." << std::endl;

    SDL_Event e;//����һ��SDLevent����
    while (SDL_PollEvent(&e))//��ȡ�¼�
    {
       if (e.type == SDL_QUIT)//�鿴�¼����ͣ�����û�����˹رա�SDL_QUIT�����������SDLռ����Դ
        {
            return false;
        } else if (e.type == SDL_KEYDOWN)//�¼�������Ϊ�����¼�������û����¼���
        {
            int key_code = e.key.keysym.sym;//������Ӧ�İ���ѡ��surface
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
                case SDLK_ESCAPE://ESC�˳�
                    /* Move camera quit */
                    return false;
                case SDLK_w://���ǰ��
                    /* Move camera forward */
                    camera.position.z += 3.0f;
                    InitialiseBuffer();
                    break;
                case SDLK_s://�������
                    /* Move camera backwards */
                    camera.position.z -= 3.0f;
                    InitialiseBuffer();
                    break;
                case SDLK_a://�������
                    /* Move camera left */
                    camera.position.x -= 3.0f;
                    InitialiseBuffer();
                    break;
                case SDLK_d://�������
                    /* Move camera right */
                    camera.position.x += 3.0f;
                    InitialiseBuffer();
                    break;
                case SDLK_r://���������ļ�
                    InitialiseBuffer();
                    scg::loadSettingsFile(settings);
                    break;
                case SDLK_p://��ͼ
                    saveScreenshot(screen);
                    break;
                case SDLK_UP://������ת
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
    memset(buffer, 0, sizeof(buffer));//��ʼ��������������Ϊ0
}

void saveScreenshot(screen *screen)
{
    std::string fileName = "screenshot" + std::to_string(samples) + ".bmp";
    SDL_SaveImage(screen, fileName.c_str());//c_str��stringתΪchar������
}