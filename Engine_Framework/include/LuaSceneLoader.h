#ifndef LuaSceneLoader_h__
#define LuaSceneLoader_h__

#include <string>

void LoadScene(class FWorld* World, class FResourceGroup* ResourceGroup, const std::string& Filename, bool bLoadPackages = true);

#endif // LuaSceneLoader_h__
