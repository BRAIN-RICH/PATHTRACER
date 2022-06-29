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
#include <fstream>
#include "json.hpp"
namespace js = nlohmann;

#define RES 650//����ֱ���
#define SCREEN_WIDTH  RES
#define SCREEN_HEIGHT RES

#define FULLSCREEN_MODE false//����ȫ��

#undef main // SDL2 compatibility with Windows

// ��������
bool Update(screen *screen,std::string settingFile);//ʵʱ����
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

int test(std::string configFilePath)
{
    InitialiseBuffer();//��ʼ������
    std::ifstream configFile(configFilePath);
    js::json config;
    configFile >> config;
    screen *screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE);//��ʼ��sdl

    // Initialise scene
    settings = scg::loadSettings();
    scg::loadSettingsFile(settings,config["transferfile"].get<std::string>());//���������ļ�
    scene = scg::loadTestModel(150.0f);//���ؿ��ζ�����
    scg::loadBrain(volume, temp, scene, settings,config["brainpath"].get<std::string>());//���ش�������
    //scg::loadManix(volume, temp, scene, settings);//����
    //scg::loadBunny(volume, temp, scene, settings);

    // ��ʼ��Ҫ��ѭ������
    while (Update(screen,config["transferfile"].get<std::string>()))
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

bool Update(screen *screen,std::string settingFile)
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
                    scg::loadSettingsFile(settings,settingFile);
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


#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
namespace py = pybind11;


PYBIND11_MODULE(raytracer, m)
{

    //��Ļ��
    py::class_<screen>(m, "screen")
            .def(py::init<>())
            .def_readwrite("height", &screen::height)
            .def_readwrite("buffer", &screen::buffer)
            .def_readwrite("width", &screen::width);
    //.def_readwrite("renderer", &screen::renderer)
    //.def_readwrite("texture", &screen::texture)
    //.def_readwrite("window", &screen::window);

    //������ֵ
    m.attr("RES") = 650;
    m.attr("FULLSCREEN_MODE") = false;
    m.attr("SCREEN_WIDTH") = RES;//����Ҫ��ֵ�Ż���python����ʾ
    m.attr("SCREEN_HEIGHT") = RES;
    m.attr("samples") = 0;


    m.def("InitialiseBuffer", &InitialiseBuffer);
    m.def("loadSettings", &scg::loadSettings);
    m.def("loadSettingsFile", &scg::loadSettingsFile);


    //����ģ���࣬����
    py::class_<scg::Vec2f>(m, "Vec2f");
    py::class_<scg::Vec2i>(m, "Vec2i");
    py::class_<scg::Vec3f>(m, "Vec3f");
    py::class_<scg::Vec4f>(m, "Vec4f");

    //������
    py::class_<scg::Camera>(m, "Camera")
            .def(py::init<>())
            .def_readwrite("position", &scg::Camera::position)
            .def_readwrite("rotation", &scg::Camera::rotation)
            .def_readwrite("antialiasing", &scg::Camera::antialiasing)
            .def_readwrite("aperture", &scg::Camera::aperture)
            .def_readwrite("height", &scg::Camera::height)
            .def_readwrite("width", &scg::Camera::width)
            .def_readwrite("focalLength", &scg::Camera::focalLength)
            .def("getRay", &scg::Camera::getRay)
            .def("getLensRay", &scg::Camera::getLensRay);


    //�������
    py::class_<scg::Sampler>(m, "Sampler")
            .def(py::init<>())
            .def("nextFloat", &scg::Sampler::nextFloat)
            .def("nextDiscrete", &scg::Sampler::nextDiscrete);


    ////�������  �����⣡����
    py::class_<scg::Ray>(m, "Ray")
            .def(py::init<>())
            .def(py::init<const scg::Vec3f&, const scg::Vec3f&, float, float>())
            .def_readwrite("direction", &scg::Ray::direction)
            .def_readwrite("maxT", &scg::Ray::maxT)
            .def_readwrite("minT", &scg::Ray::minT)
            .def_readwrite("origin", &scg::Ray::origin);
    //.def_readwrite("operator", &scg::Ray::operator());//���������������ڲ�����
    //.def_readwrite("isInside", &scg::Ray::isInside)//�����������з����ڲ�����



    py::class_<scg::Light>(m, "Light")
            //.def(py::init<const scg::Vec3f&,float>())
            .def("getEmittance", &scg::Light::getEmittance)
            .def("getIntensity", &scg::Light::getIntensity)
            .def("getType", &scg::Light::getType)
            .def("illuminate", &scg::Light::illuminate);


    //�������
    py::class_<scg::Scene>(m, "Scene")
            .def_readwrite("volume", &scg::Scene::volume)
            .def_readwrite("lights", &scg::Scene::lights)
            .def_readwrite("materials", &scg::Scene::materials)
            .def_readwrite("objects", &scg::Scene::objects)
            .def_readwrite("volumePos", &scg::Scene::volumePos);

    ////�������  �����⣡����
    py::class_<scg::Volume>(m, "Volume")
            .def(py::init<int, int, int>())
            .def_readwrite("height", &scg::Volume::height)
            .def_readwrite("width", &scg::Volume::width)
            .def_readwrite("depth", &scg::Volume::depth)
            .def_readwrite("octree", &scg::Volume::octree);
    //.def_readwrite("data", &scg::Volume::data);//����  ����������ָ����������
    //.def("sampleVolume", &scg::Volume::sampleVolume)
    //.def("getGradient", &scg::Volume::getGradient)
    //.def("getGradientNormalised", &scg::Volume::getGradientNormalised);

    ////��������
    py::class_<scg::Settings>(m, "Settings")
            .def(py::init<>())
            .def_readwrite("backgroundLight", &scg::Settings::backgroundLight)
            .def_readwrite("bb", &scg::Settings::bb)
            .def_readwrite("brackets", &scg::Settings::brackets)
            .def_readwrite("densityScale", &scg::Settings::densityScale)
            .def_readwrite("gamma", &scg::Settings::gamma)
            .def_readwrite("gradientFactor", &scg::Settings::gradientFactor)
            .def_readwrite("mask", &scg::Settings::mask)
            .def_readwrite("maxDepth", &scg::Settings::maxDepth)
            .def_readwrite("maxOpacity", &scg::Settings::maxOpacity)
            .def_readwrite("minDepth", &scg::Settings::minDepth)
            .def_readwrite("minStepSize", &scg::Settings::minStepSize)
            .def_readwrite("octreeLevels", &scg::Settings::octreeLevels)
            .def_readwrite("renderType", &scg::Settings::renderType)
            .def_readwrite("stepSize", &scg::Settings::stepSize)
            .def_readwrite("transferFunction", &scg::Settings::transferFunction)
            .def_readwrite("useBox", &scg::Settings::useBox);



    m.def("Draw", &Draw);
    m.def("loadTestModel", &scg::loadTestModel);


    //���ɺ�python�������
    m.def("Update", &Update);//���Լ�д
    m.def("saveScreenshot", &saveScreenshot);
    m.def("SDL_Renderframe", &SDL_Renderframe);
    m.def("KillSDL", &KillSDL);
    m.def("InitializeSDL", &InitializeSDL);



    m.def("loadBrain", &scg::loadBrain);
    m.def("loadBunny", &scg::loadBunny);
    m.def("loadManix", &scg::loadManix);
    m.def("test",&test);
}
