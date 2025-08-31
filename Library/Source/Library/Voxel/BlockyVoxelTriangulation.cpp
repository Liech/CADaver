#include "BlockyVoxelTriangulation.h"

#include <map>

#include "Library/Triangle/Triangulation.h"
#include "Library/Voxel/BinaryVolume.h"

namespace Library
{
    BlockyVoxelTriangulation::BlockyVoxelTriangulation(const BinaryVolume& _input)
      : input(_input)
    {
    }

    std::unique_ptr<Triangulation> BlockyVoxelTriangulation::triangulate()
    {
        auto result = std::make_unique<Triangulation>();

        glm::ivec3 dim = input.dimension;

        for (size_t i = 0; i < 3; i++)
        {
            scan(i, false);
            scan(i, true);
        }

        result->indices.resize(indexBuffer.size());
        for (size_t i = 0; i < indexBuffer.size(); i++)
            result->indices[i] = indexBuffer[i];

        glm::dvec3 voxelLength = glm::dvec3(input.size.x / input.dimension.x, input.size.y / input.dimension.y, input.size.z / input.dimension.z);

        result->vertices.resize(indexMap.size());
        for (const auto& x : indexMap)
        {
            result->vertices[x.second] = input.origin + glm::dvec3(x.first.x * voxelLength.x, x.first.y * voxelLength.y, x.first.y * voxelLength.y);
        }

        return std::move(result);
    }

    void BlockyVoxelTriangulation::scan(size_t scanDimension, bool reverse)
    {
        auto get = [this](const glm::ivec3& x) { return input.data[x.x + x.y * input.dimension.x + x.y * input.dimension.x * input.dimension.y]; };

        size_t dimA        = (scanDimension + 1) % 3;
        size_t dimB        = (scanDimension + 2) % 3;
        size_t scanDimSize = input.dimension[scanDimension];
        size_t dimASize    = input.dimension[dimA];
        size_t dimBSize    = input.dimension[dimB];

        // GATHER STREAKS

        std::map<std::pair<size_t, size_t>, bool> streakMap;
        std::vector<std::pair<size_t, size_t>>    streakList;
        std::pair<size_t, size_t>                 currentStreak  = std::make_pair(0, 0); // len0 = no active streak
        auto                                      isStreakActive = [&currentStreak]() { return currentStreak.second != 0; };
        auto                                      endStreak      = [&currentStreak, &streakList, &streakMap]()
        {
            streakMap[currentStreak] = false;
            streakList.push_back(currentStreak);
            currentStreak.first  = 0;
            currentStreak.second = 0;
        };

        for (size_t scanPos = 0; scanPos < scanDimSize; scanPos++)
        {
            for (int A = 0; A < dimASize; A++)
            {
                for (int B = 0; B < dimBSize; B++)
                {
                    glm::ivec3 pos;
                    pos[scanDimension] = scanPos;
                    pos[dimA]          = A;
                    pos[dimB]          = B;
                    glm::ivec3 pos2    = pos;
                    pos2[scanDimension] += reverse ? -1 : 1;

                    bool val2 = false;
                    if (!((scanPos == 0 && reverse) || (scanPos == scanDimSize - 1 && !reverse)))
                        val2 = get(pos2);

                    bool val  = get(pos);
                    bool edge = (!val2) && val;

                    bool active = isStreakActive();
                    if (!edge && active)
                        endStreak;
                    else if (edge && active)
                        currentStreak.second++;
                    else if (edge && !active)
                    {
                        currentStreak.first  = B + A * dimBSize;
                        currentStreak.second = 1;
                    }
                    // else if (!val && !active) doNothing();
                }
                if (isStreakActive())
                    endStreak();
            }

            // PROCESS STREAKS to SQUARES

            std::vector<std::pair<glm::ivec2, glm::ivec2>> squares; // pos, size
            for (size_t i = 0; i < streakList.size(); i++)
            {
                const auto& streak = streakList[i];
                if (streakMap[streak])
                    continue;
                std::pair<size_t, size_t> streakBelow     = { streak.first + dimBSize, streak.second };
                size_t                    streakOfStreaks = 1;

                while (streakMap.contains(streakBelow))
                {
                    streakOfStreaks++;
                    streakMap[streakBelow] = true;
                    streakBelow.first += dimBSize;
                }
                squares.push_back(std::make_pair(glm::ivec2(streak.first / dimBSize, streak.first % dimBSize), glm::ivec2(streak.second, streakOfStreaks)));
            }

            auto getIndex = [this](const glm::ivec3& pos)
            {
                if (indexMap.contains(pos))
                    return indexMap[pos];
                else
                {
                    auto result = nextIndex;
                    nextIndex++;
                    indexMap[pos] = result;
                    return result;
                }
            };

            // Process squares
            double vlScanDim = input.size[scanDimension] / (double)input.dimension[scanDimension]; // voxel length
            double vlDimA    = input.size[dimA] / (double)input.dimension[dimA];
            double vlDimB    = input.size[dimB] / (double)input.dimension[dimB];
            for (size_t i = 0; i < squares.size(); i++)
            {
                const auto& square = squares[i];
                glm::ivec3  MM{ 0, 0, 0 }; // Minus Minus Square corner
                MM[scanDimension] = scanPos;
                MM[dimA] += square.first[0];
                MM[dimB] += square.first[1];
                glm::ivec3 MP = MM; // Minus Plus Square corner
                glm::ivec3 PM = MM;
                PM[dimA] += square.second[0];
                MP[dimB] += square.second[1];
                glm::ivec3 PP = PM;
                PP[dimB] += square.second[1];

                size_t              iMM            = getIndex(MM);
                size_t              iMP            = getIndex(MP);
                size_t              iPM            = getIndex(PM);
                size_t              iPP            = getIndex(PP);
                std::vector<size_t> subIndexBuffer = { iMM, iPM, iPP, iMM, iPP, iMP };
                if (reverse)
                    std::reverse(subIndexBuffer.begin(), subIndexBuffer.end());
                for (const auto& x : subIndexBuffer)
                    indexBuffer.push_back(x);
            }
        }
    }
}