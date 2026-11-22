#pragma once

#include "CADacomb/Volume/SVO/SVO.h"

#include <cstdint>
#include <memory>
#include <string>

struct lua_State;

// Evaluates a user-provided Lua script (sdf or eval + optional classifyBox) in parallel to generate a boolean SVO.
class LuaSVOField
{
  public:
    LuaSVOField();
    virtual ~LuaSVOField();

    static std::unique_ptr<CADacomb::SVO<bool>> generateSVO(uint32_t dimension, const std::string& script, std::string& outError);

  private:
    static void registerNoiseHelpers(lua_State* state);
    static int  luaPerlin3D(lua_State* state);
    static int  luaFbm3D(lua_State* state);
};
