#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H

#include "light.h"
#include "material.h"
#include "object.h"
#include "volume.h"

#include <vector>

namespace scg
{

class Scene
{
public:
    std::vector<std::shared_ptr<Light>> lights;//存放光线智能指针的容器
    std::vector<std::shared_ptr<Material>> materials;//存放材质智能指针的容器
    std::vector<std::shared_ptr<Object>> objects;//存放物体智能指针的容器

    // Supports single volume
    Vec3f volumePos;
    std::shared_ptr<Volume> volume;//智能指针，指向类型为Volume的对象
};

}

#endif //RAYTRACER_SCENE_H
