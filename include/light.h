﻿#ifndef RAYTRACER_LIGHT_H
#define RAYTRACER_LIGHT_H

#include "object.h"
#include "sampler.h"
#include "scatterevent.h"
#include "vector_type.h"

#include <memory>

namespace scg
{

// Information regarding the illuminance of a light at a certain position
//特定方向上光的照明信息,找到光线最近的一个打击点，把当前点的信息（位置，法向量，t）保存下来
class LightHit
{
public:
    Vec3f colour;
    float pdf;//概率密度函数

    Vec3f direction; // Normalised vector pointing to the source. Equal to the normal in case of AbstractLight.
    float distance;
};

// Light types
//枚举光线类型
enum LightType {
    LightType_Abstract,
    LightType_Point,
    LightType_Directional,
    LightType_Object
};

//灯光虚基类
class Light
{
protected:
    Vec3f colour;//颜色
    float intensity;//强度

public:
    Light(Vec3f const& colour, float intensity):
        colour(colour), intensity(intensity) {};

	//照明纯虚函数
    virtual LightHit illuminate(ScatterEvent const&, Sampler &) const = 0;

	//获取发射光
    virtual Vec3f getEmittance(ScatterEvent const& interaction) const
    {
        // Do not illuminate on the back side
        return colour * intensity * std::max(0.0f, dot(interaction.normal, interaction.outputDir));
    }

    virtual float getIntensity() const
    {
        return intensity;
    }

    virtual LightType getType() const = 0;
};

// An abstract light, assume it is located at the position where it is sampled
// Always returns maximum intensity
//抽象灯光类，假设位于采样点，总是返回最大光强。继承自灯光父类
class AbstractLight : public Light
{
private:
    Vec3f colour;
    float intensity;

public:
    AbstractLight(Vec3f const& colour, float intensity):
        Light(colour, intensity) {};

    LightHit illuminate(ScatterEvent const& interaction, Sampler&) const override
    {
        LightHit lightHit;

        lightHit.colour = colour * intensity;
        lightHit.pdf = 1;//概率密度函数为1
        lightHit.direction = interaction.normal;//返回击打处的正交方向

        return lightHit;
    }

    LightType getType() const override
    {
        return LightType_Abstract;
    }
};

// Background light, comes from any direction
// Should be used on the scene member variable, not the light list
//背景光
class BackgroundLight : public Light
{
private:
    Vec3f colour;
    float intensity;

public:
    BackgroundLight(Vec3f const& colour, float intensity):
        Light(colour, intensity) {};

    LightHit illuminate(ScatterEvent const&, Sampler&) const override
    {
        LightHit lightHit;

        lightHit.colour = colour * intensity;
        lightHit.pdf = 1;

        return lightHit;
    }

    LightType getType() const override
    {
        return LightType_Abstract;
    }
};

// A source of light at a particular position with no shape/body
//点光源
class PointLight : public Light
{
private:
    Vec3f position;

public:
    PointLight(Vec3f const& colour, float intensity, Vec3f const& position):
        Light(colour, intensity), position(position) {};

    LightHit illuminate(ScatterEvent const& interaction, Sampler&) const override
    {
        LightHit lightHit;

        lightHit.colour = colour * intensity * (float)M_1_PI;

        lightHit.direction = this->position - interaction.position;
        lightHit.distance = lightHit.direction.length();
        lightHit.direction /= lightHit.distance;

        lightHit.pdf = (float)(4.0f * M_1_PI * lightHit.distance * lightHit.distance);

        return lightHit;
    }

    LightType getType() const override
    {
        return LightType_Point;
    }
};

// A source of light in a particular direction with no shape/body and position
//方向光源
class DirectionalLight : public Light
{
private:
    Vec3f direction;

public:
    DirectionalLight(Vec3f const& colour, float intensity, Vec3f const& direction):
        Light(colour, intensity), direction(normalise(direction)) {};

    LightHit illuminate(ScatterEvent const&, Sampler&) const override
    {
        LightHit lightHit;

        lightHit.colour = colour * intensity;

        lightHit.direction = -direction; // TODO: generate slightly random direction
        lightHit.distance = INF;

        lightHit.pdf = 1.0f;

        return lightHit;
    }

    LightType getType() const override
    {
        return LightType_Directional;
    }
};

//物体反射光
class ObjectLight : public Light // TODO: Add m_area for scaling luminosity with size
{
private:
    std::shared_ptr<Object> object;

public:
    ObjectLight(Vec3f const& colour, float intensity, std::shared_ptr<Object> object):
        Light(colour, intensity), object(object) {};

    LightHit illuminate(ScatterEvent const& interaction, Sampler &sampler) const override
    {
        LightHit lightHit;

        ScatterEvent source = object->sampleSurface(sampler);

        lightHit.colour = colour * intensity * (float)M_1_PI;

        lightHit.direction = source.position - interaction.position;
        lightHit.distance = lightHit.direction.length();
        lightHit.direction /= lightHit.distance;

        lightHit.pdf = (float)M_1_PI * lightHit.distance * lightHit.distance;
        lightHit.pdf /= std::max(0.0f, dot(source.normal, -lightHit.direction));

        return lightHit;
    }

    LightType getType() const override
    {
        return LightType_Object;
    }
};

}

#endif //RAYTRACER_LIGHT_H
