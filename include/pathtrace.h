#ifndef RAYTRACER_PATHTRACE_H
#define RAYTRACER_PATHTRACE_H

#include "ray.h"
#include "raytrace.h"
#include "sampler.h"
#include "scene.h"
#include "scatterevent.h"
#include "settings.h"
#include "vector_type.h"

namespace scg
{

// From PBRT-v3
inline float powerHeuristic(int nf, float fPdf, int ng, float gPdf)
{
    float f = nf * fPdf;
    float g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}

Vec3f SampleOneLight(ScatterEvent& interaction, Scene const& scene, std::shared_ptr<Material> const& material,
                     std::shared_ptr<Light> const& hitLight, Settings const& settings, Sampler& sampler)
{
    // Cannot light mirror
    if ((material->getSupportedLobes(interaction.uv) & BSDFLobe::Specular) != 0)
    {
        return Vec3f(0, 0, 0);
    }

    // Cannot sample another light
    if (scene.lights.size() == 0 || (scene.lights.size() <= 1 && hitLight != nullptr))
    {
        return Vec3f(0, 0, 0);
    }

    // Find another light
    std::shared_ptr<Light> light;
    do
    {
        size_t index = (size_t)sampler.nextDiscrete(scene.lights.size());
        light = scene.lights[index];
    } while (light == hitLight);

    // Calculate light
    Vec3f directLight;

    LightType lightType = light->getType();
    LightHit lightHit = light->illuminate(interaction, sampler);

    switch (lightType)
    {
        case LightType_Abstract:
        {
            if (lightHit.pdf != 0)
            {
                interaction.inputDir = lightHit.direction;
                float pdf = material->pdf(interaction);
                if (pdf != 0)
                {
                    directLight += material->evaluate(interaction) * lightHit.colour / lightHit.pdf; // TODO: fix formula
                }
            }

            break;
        }
        case LightType_Point:
        case LightType_Directional:
        case LightType_Object:
        {
            Ray lightRay{interaction.position, lightHit.direction, RAY_EPS};
            Intersection lightIntersection{};

            if (std::isnormal(lightHit.pdf)) // Real number, not 0
            {
                // Check for objects blocking the path
                if (!getClosestIntersection(scene, lightRay, lightIntersection, settings, sampler) ||
                    lightIntersection.distance + EPS >= lightHit.distance)
                {
                    getClosestIntersection(scene, lightRay, lightIntersection, settings, sampler);
                    interaction.inputDir = lightHit.direction;
                    float pdf = material->pdf(interaction);
                    if (pdf != 0)
                    {
                        float weight = powerHeuristic(1, lightHit.pdf, 1, pdf);
                        directLight += material->evaluate(interaction) * lightHit.colour * weight / lightHit.pdf;
                    }
                }
            }

            break;
        }
    }

    return directLight * scene.lights.size();
}

//跟随光线
Vec3f trace(
    Scene const& scene,//场景
    Ray ray,
    Settings const& settings,
    Sampler &sampler)
{
    Vec3f colour;
    Vec3f throughput(1.0f, 1.0f, 1.0f);

    ScatterEvent interaction;
    interaction.iorI = 1.0f; // Air

    int bounces = 0;
    int minBounces = settings.minDepth;//最小弹射次数
    int maxBounces = settings.maxDepth;//最大弹射次数

    for (bounces = 0; bounces < maxBounces; ++bounces)
    {
        // Intersect the scene
        Intersection intersection;


		//如果没有打到物体，则与返回一个背景色
        if (!getClosestIntersection(scene, ray, intersection, settings, sampler))
        {
            colour += throughput * settings.backgroundLight;//初值（1，1，1）*背景色
            break;
        }

        // Initialise interaction
        interaction.position = intersection.position;//相交位置
        interaction.normal = intersection.normal;//相交位置发现
        interaction.uv = interaction.uv;
        interaction.outputDir = -ray.direction;//光线出射方向
        interaction.iorO = 0.0f;

        std::shared_ptr<Material> material;

        if (intersection.surfaceType == SurfaceType::Volume)
        {
            Vec3f localPos = intersection.position - scene.volumePos;
            Vec3f normal = scene.volume->getGradient(localPos, 0.5f); // TODO: Maybe use TransferFunction
            float magnitude = normal.length();//量级
            float intensity = scene.volume->sampleVolume(localPos);
            Vec4f out = settings.transferFunction.evaluate(intensity);

            interaction.normal = normal / magnitude;

            // T. Kroes
            //float probBRDF = (1.0f - std::exp(-settings.gradientFactor * (magnitude * scene.invMaxGradient)));
            float probBRDF = (1.0f - std::exp(-settings.gradientFactor * (magnitude / intensity)));

            // Surface
			//体积表面
            if (sampler.nextFloat() < probBRDF)
            {
                interaction.position += interaction.normal * settings.stepSize;

                throughput *= Vec3f{out.r, out.g, out.b};
                material = settings.transferFunction.getMaterial(intensity, sampler);
            }
            // Isotropic
            else
            {
                throughput *= 2.0f; // TODO: probably wrong, but looks better
                material = std::make_shared<Isotropic>(Isotropic{std::make_shared<ColourTexture>(ColourTexture{{out.x, out.y,  out.z}})});
            }
        }
        else
        {
            material = scene.materials[intersection.materialID];
        }

        // Add light
        auto const& hitLight = material->getLight(interaction.uv);
        if (hitLight != nullptr && (bounces == 0 || interaction.sampledLobe & BSDFLobe::Specular))
        {
            colour += throughput * hitLight->getEmittance(interaction);
        }

        // Calculate direct light
		//计算直接光照
        colour += throughput * SampleOneLight(interaction, scene, material, hitLight, settings, sampler);

        if (bounces == maxBounces - 1)
            break;

        // Sample next direction
        float pdf;
        do
        {
            material->sample(interaction, sampler);
            pdf = material->pdf(interaction);
        } while(!std::isnormal(pdf)); // Sampler may return an impossible(parallel) direction

        // Accumulate
        if (!std::isnormal(pdf)) std::cout << "WTF";
        throughput *= material->evaluate(interaction) / pdf;

        if (interaction.sampledLobe == BSDFLobe::SpecularTransmission)
        {
            interaction.iorI = interaction.iorO;
        }

        // Create new ray
        ray.origin = interaction.position;
        ray.direction = interaction.inputDir;
        ray.minT = RAY_EPS;

        // Russian Roulette
		//俄罗斯轮盘赌
        if (bounces >= minBounces - 1)
        {
            float p = std::max(throughput.x, std::max(throughput.y, throughput.z));
            if (sampler.nextFloat() > p) {
                break;
            }

            throughput /= p;
        }
    }

    return colour / (1 + bounces);
}

}

#endif //RAYTRACER_PATHTRACE_H
