#ifndef COMS30115_UTILITY_H
#define COMS30115_UTILITY_H

#include "settings.h"
#include "scene.h"
#include "volume.h"

#include <vector>
#include <filesystem>
namespace scg
{

Settings loadSettings();

void loadSettingsFile(Settings &settings,std::string filename);

void loadBrain(Volume& volume, Volume& temp, Scene &scene, Settings &settings,std::string filepath);

void loadManix(Volume& volume, Volume& temp, Scene &scene, Settings &settings);

void loadBunny(Volume& volume, Volume& temp, Scene &scene, Settings &settings);

// Loads the Cornell Box. It is scaled to fill the volume:
// -1 <= x <= +1
// -1 <= y <= +1
// -1 <= z <= +1
Scene loadTestModel(float size);

}

#endif //COMS30115_UTILITY_H
