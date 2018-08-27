/*****************************************************************************************
*                                                                                       *
* OpenSpace                                                                             *
*                                                                                       *
* Copyright (c) 2014-2018                                                               *
*                                                                                       *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
* software and associated documentation files (the "Software"), to deal in the Software *
* without restriction, including without limitation the rights to use, copy, modify,    *
* merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
* permit persons to whom the Software is furnished to do so, subject to the following   *
* conditions:                                                                           *
*                                                                                       *
* The above copyright notice and this permission notice shall be included in all copies *
* or substantial portions of the Software.                                              *
*                                                                                       *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
* PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  *
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  *
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         *
****************************************************************************************/

#include <modules/roverterrainrenderer/roverterrainrenderermodule.h>
#include <modules/roverterrainrenderer/model/modelprovider.h>
#include <modules/roverterrainrenderer/model/multimodelprovider.h>
#include <modules/roverterrainrenderer/model/singlemodelprovider.h>

#include <openspace/rendering/renderable.h>
#include <openspace/util/factorymanager.h>

#include <modules/roverterrainrenderer/renderable/roverterrain.h>

#include <ghoul/logging/logmanager.h>
#include <ghoul/filesystem/filesystem.h>
#include <ghoul/misc/assert.h>
#include <ghoul/misc/dictionary.h>

namespace {
  constexpr const char* RoverTerrainRendererModule = "RoverTerrainRendererModule";
  const char* _loggerCat = "RoverTerrainRendererModule";
}

namespace openspace {

RoverTerrainRendererModule::RoverTerrainRendererModule() : OpenSpaceModule(Name) {}

void RoverTerrainRendererModule::internalInitialize(const ghoul::Dictionary& configuration) {
    auto fRenderable = FactoryManager::ref().factory<Renderable>();
    ghoul_assert(fRenderable, "No renderableFactory existed");

    fRenderable->registerClass<RoverTerrain>("RoverTerrain");

    auto fModelProvider = std::make_unique<ghoul::TemplateFactory<ModelProvider>>();

    fModelProvider->registerClass<MultiModelProvider>("MultiModelProvider");
    fModelProvider->registerClass<SingleModelProvider>("SingleModelProvider");

    FactoryManager::ref().addFactory(std::move(fModelProvider));
}

} // namespace openspace