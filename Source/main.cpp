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
#include <cstring>//??????????????std????????????????c++?????????c???????
#include <memory>
#include <omp.h>//openMP???��??
#include <string>


#define RES 650//????????
#define SCREEN_WIDTH  RES
#define SCREEN_HEIGHT RES

#define FULLSCREEN_MODE false//???????

#undef main // SDL2 compatibility with Windows

// ????????
bool Update(screen *screen);//??????
void Draw(screen *screen);//???????
void InitialiseBuffer();//?????????
void saveScreenshot(screen *screen);//??????????


//???????? ???????????????
scg::Sampler sampler[20]; // TODO: !!! find a better solution


//???
scg::Camera camera{
    scg::Vec3f(0, 0, -240),//???��??
    scg::Vec3f(0, 0, 0),//?????????
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    true, // Jitter ????  ?????
    0.2f, // Aperture ??????
    3.0f}; // Focal length  ????

scg::Vec3f rotation{0, 0, 0};

scg::Settings settings;
scg::Scene scene;

scg::Volume volume(VOLUME_SIZE, VOLUME_SIZE, VOLUME_SIZE);//??????????��
scg::Volume temp(VOLUME_SIZE, VOLUME_SIZE, VOLUME_SIZE);//?????????��

int samples;//????????
scg::Vec3f buffer[SCREEN_HEIGHT][SCREEN_WIDTH];//???????

int main(int argc, char *argv[])
{
    InitialiseBuffer();//?????????

    screen *screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE);//?????sdl

    // Initialise scene
	settings = scg::loadSettings();
    scg::loadSettingsFile(settings);//???????????
    scene = scg::loadTestModel(150.0f);//??????��?????
    //scg::loadBrain(volume, temp, scene, settings);//???????????
    //scg::loadManix(volume, temp, scene, settings);//????
    //scg::loadBunny(volume, temp, scene, settings);

    // ???????????????
    while (Update(screen))
    {
        Draw(screen);
        SDL_Renderframe(screen);
    }

    // ?��?????????
    //saveScreenshot(screen);
    KillSDL(screen);

    return 0;
}

void test()
{
    InitialiseBuffer();//初始化缓存

    screen *screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE);//初始化sdl

    // Initialise scene
	settings = scg::loadSettings();
    scg::loadSettingsFile(settings);//加载设置文件
    scene = scg::loadTestModel(150.0f);//加载康奈尔盒子
    scg::loadBrain(volume, temp, scene, settings);//加载大脑数据
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

}

void Draw(screen *screen)
{
    ++samples;//??????��?+1

    // TODO: reseed generator
	//????????for????????for??????????????��???????????????????
    #pragma omp parallel for schedule(dynamic) shared(camera, scene, settings, screen)
    for (int y = 0; y < SCREEN_HEIGHT; ++y)//y???????????
    {
        for (int x = 0; x < SCREEN_WIDTH; ++x)//x???????????
        {
            scg::Ray ray = camera.getRay(x, y, sampler[omp_get_thread_num()]);//??????????
            ray.minT = scg::RAY_EPS;//??????????
            
            ray.origin = scg::rotate(ray.origin, rotation);//???????
            ray.direction = scg::rotate(ray.direction, rotation);//???????

            scg::Vec3f colour = scg::trace(scene, ray, settings, sampler[omp_get_thread_num()]);//???��???????????????
			//???��??????????????????,???1???????
            buffer[y][x] += colour * settings.gamma; // TODO: clamp value

			//???????????????
            PutPixelSDL(screen, x, y, buffer[y][x] / samples);
        }
    }
}

bool Update(screen *screen)
{
    static int t = SDL_GetTicks();//?????SDL???????????????????????��????????unsigned 32-bit????
    /* ???????????? */
    int t2 = SDL_GetTicks();//????????????
    float dt = float(t2 - t);
    t = t2;//???????????????
    /*Good idea to remove this*/
    std::cout << "Iteration: " << samples << ". Render time: " << dt << " ms." << std::endl;

    SDL_Event e;//???????SDLevent????
    while (SDL_PollEvent(&e))//??????
    {
       if (e.type == SDL_QUIT)//????????????????????????SDL_QUIT???????????SDL??????
        {
            return false;
        } else if (e.type == SDL_KEYDOWN)//????????????????????????????????
        {
            int key_code = e.key.keysym.sym;//???????????????surface
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
                case SDLK_ESCAPE://ESC???
                    /* Move camera quit */
                    return false;
                case SDLK_w://??????
                    /* Move camera forward */
                    camera.position.z += 3.0f;
                    InitialiseBuffer();
                    break;
                case SDLK_s://???????
                    /* Move camera backwards */
                    camera.position.z -= 3.0f;
                    InitialiseBuffer();
                    break;
                case SDLK_a://???????
                    /* Move camera left */
                    camera.position.x -= 3.0f;
                    InitialiseBuffer();
                    break;
                case SDLK_d://???????
                    /* Move camera right */
                    camera.position.x += 3.0f;
                    InitialiseBuffer();
                    break;
                case SDLK_r://???????????
                    InitialiseBuffer();
                    scg::loadSettingsFile(settings);
                    break;
                case SDLK_p://???
                    saveScreenshot(screen);
                    break;
                case SDLK_UP://???????
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
    memset(buffer, 0, sizeof(buffer));//??????????????????0
}

void saveScreenshot(screen *screen)
{
    std::string fileName = "screenshot" + std::to_string(samples) + ".bmp";
    SDL_SaveImage(screen, fileName.c_str());//c_str??string??char??????
}


#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
namespace py = pybind11;


PYBIND11_MODULE(raytracer, m) 
{	
	
	//?????
	py::class_<screen>(m, "screen")
		.def(py::init<>())
		.def_readwrite("height", &screen::height)
		.def_readwrite("buffer", &screen::buffer)
		.def_readwrite("width", &screen::width);
		//.def_readwrite("renderer", &screen::renderer)
		//.def_readwrite("texture", &screen::texture)
		//.def_readwrite("window", &screen::window);

	//???????
	m.attr("RES") = 650;
	m.attr("FULLSCREEN_MODE") = false;
	m.attr("SCREEN_WIDTH") = RES;//?????????????python?????
	m.attr("SCREEN_HEIGHT") = RES;
	m.attr("samples") = 0;


	m.def("InitialiseBuffer", &InitialiseBuffer);
	m.def("loadSettings", &scg::loadSettings);
	m.def("loadSettingsFile", &scg::loadSettingsFile);


	//?????????????
	py::class_<scg::Vec2f>(m, "Vec2f");
	py::class_<scg::Vec2i>(m, "Vec2i");
	py::class_<scg::Vec3f>(m, "Vec3f");
	py::class_<scg::Vec4f>(m, "Vec4f");

	//??????
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


	//???????
	py::class_<scg::Sampler>(m, "Sampler")
		.def(py::init<>())
		.def("nextFloat", &scg::Sampler::nextFloat)
		.def("nextDiscrete", &scg::Sampler::nextDiscrete);


	////???????  ??????????
	py::class_<scg::Ray>(m, "Ray")
		.def(py::init<>())
		.def(py::init<const scg::Vec3f&, const scg::Vec3f&, float, float>())
		.def_readwrite("direction", &scg::Ray::direction)
		.def_readwrite("maxT", &scg::Ray::maxT)
		.def_readwrite("minT", &scg::Ray::minT)
		.def_readwrite("origin", &scg::Ray::origin);
		//.def_readwrite("operator", &scg::Ray::operator());//???????????????????????
		//.def_readwrite("isInside", &scg::Ray::isInside)//?????????????��??????????



	py::class_<scg::Light>(m, "Light")
		//.def(py::init<const scg::Vec3f&,float>())
		.def("getEmittance", &scg::Light::getEmittance)
		.def("getIntensity", &scg::Light::getIntensity)
		.def("getType", &scg::Light::getType)
		.def("illuminate", &scg::Light::illuminate);

	
	//??????? 
	py::class_<scg::Scene>(m, "Scene")
		.def_readwrite("volume", &scg::Scene::volume)
		.def_readwrite("lights", &scg::Scene::lights)
		.def_readwrite("materials", &scg::Scene::materials)
		.def_readwrite("objects", &scg::Scene::objects)
		.def_readwrite("volumePos", &scg::Scene::volumePos);

	////???????  ??????????
	py::class_<scg::Volume>(m, "Volume")
		.def(py::init<int, int, int>())
		.def_readwrite("height", &scg::Volume::height)
		.def_readwrite("width", &scg::Volume::width)
		.def_readwrite("depth", &scg::Volume::depth)
		.def_readwrite("octree", &scg::Volume::octree);
		//.def_readwrite("data", &scg::Volume::data);//????  ?????????????????????
		//.def("sampleVolume", &scg::Volume::sampleVolume)
		//.def("getGradient", &scg::Volume::getGradient)
		//.def("getGradientNormalised", &scg::Volume::getGradientNormalised);

	////????????
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


	//?????python???????
	m.def("Update", &Update);//?????��
	m.def("saveScreenshot", &saveScreenshot);
	m.def("SDL_Renderframe", &SDL_Renderframe);
	m.def("KillSDL", &KillSDL);
	m.def("InitializeSDL", &InitializeSDL);



	m.def("loadBrain", &scg::loadBrain);
	m.def("loadBunny", &scg::loadBunny);
	m.def("loadManix", &scg::loadManix);
    m.def("test",&test);

}
