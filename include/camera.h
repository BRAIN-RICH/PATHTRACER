#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include "sampler.h"
#include "ray.h"

namespace scg
{

class Camera
{
public:
    Vec3f position;//三维位置
    Vec3f rotation;//三维旋转角度

    int width;//屏幕宽度
    int height;//屏幕高度

    bool antialiasing = true;//抗锯齿

    float aperture = 0;//光圈孔径
    float focalLength = 0.5;//焦距

    //Camera() = default;


	//获取光线起始位置
    Ray getRay(int const x, int const y, Sampler &sampler) const
    {
        float dX = 0;
        float dY = 0;

        // Apply jitter for antialiasing
		//在抗锯齿中添加抖动，即在每帧采样时，将采样点进行偏移，偏移0.5
        if (antialiasing)
        {
            dX = sampler.nextFloat() - 0.5f;
            dY = sampler.nextFloat() - 0.5f;
        }

        Vec3f dir{
            ((float)x + dX - width / 2.0f),
            ((float)y + dY - height / 2.0f),
            (width + height) / 2};
        dir = rotate(dir, rotation);

        return Ray{position, dir};//返回光线的位置和方向
    }

	//获取光线实时位置
    Ray getLensRay(int const x, int const y, Sampler &sampler) const
    {
        Ray focalRay = getRay(x, y, sampler);
        Vec3f objective = focalRay(focalLength);//此处（）重定义，表示返回实时光线位置

        Vec3f dPos{
            (sampler.nextFloat() - 0.5f) * aperture,
            (sampler.nextFloat() - 0.5f) * aperture,
            0.0f
        };

        Vec3f pos = position + rotate(dPos, rotation);;
        Vec3f dir = objective - pos;

        return Ray{pos, dir};
    }
};

}

#endif //RAYTRACER_CAMERA_H
