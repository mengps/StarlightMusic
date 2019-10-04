#include "skinmanager.h"

SkinManager* SkinManager::instance()
{
    static SkinManager setting;
    return &setting;
}

SkinManager::~SkinManager()
{
}

SkinManager::SkinManager()
{
}
