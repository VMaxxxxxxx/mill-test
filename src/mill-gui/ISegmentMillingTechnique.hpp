#pragma once

#include "CuttingToolParams.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace ms
{

class ISegmentMillingTechnique
{
public:
    virtual ~ISegmentMillingTechnique() = default;

    virtual void moveTool(
        std::vector<float> &heightmap,
        glm::ivec2 heightmapResolution,
        glm::mat4 worldHeightmapTransformation,
        const CuttingToolParams &toolParams,
        glm::dvec3 tipStartPosition, 
        glm::dvec3 tipEndPosition
    ) = 0;
};

class BoundariesMillingTechnique:
    public ISegmentMillingTechnique
{
public:
    BoundariesMillingTechnique();
    virtual ~BoundariesMillingTechnique();

    virtual void moveTool(
        std::vector<float> &heightmap,
        glm::ivec2 heightmapResolution,
        glm::mat4 worldHeightmapTransformation,
        const CuttingToolParams &toolParams,
        glm::dvec3 tipStartPosition, 
        glm::dvec3 tipEndPosition
    );

protected:
    CuttingToolParams _cuttingToolParams;
    double _tcRadius;

    glm::dmat4 _heightmapTransformation;

    glm::dvec4 getToolBoundaries(glm::dvec3 position);
};

}
