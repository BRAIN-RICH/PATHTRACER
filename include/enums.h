#ifndef RAYTRACER_ENUMS_H
#define RAYTRACER_ENUMS_H

//枚举表面类型
enum SurfaceType
{
    Surface = 0,
    Volume = (1 << 0)//把1按2进制左移一位
};

enum BSDFLobe
{
    Null = 0,
    Diffuse = (1 << 0),//漫反射
    SpecularReflection = (1 << 1),//镜面反射
    SpecularTransmission = (1 << 2),//镜面传输
    Specular = SpecularReflection | SpecularTransmission//二选一
};

#endif //RAYTRACER_ENUMS_H
