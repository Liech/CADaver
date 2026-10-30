#pragma once

#include <string>

namespace Library
{
    int         String2TopAbs(const std::string&);
    std::string TopAbs2String(int);
    int         String2TopAbsOrientation(const std::string&);
    std::string TopAbsOrientation2String(int);
}