#pragma once
#ifndef UIHELPER_H__
#define UIHELPER_H__

#include <QString>
#include <vector>
#include <map>

#include "CityGMLHelper.h"
#include "GeometryHandler.h"

class UIHelperSingleton
{
private:
    void initialize(void);
    UIHelperSingleton();
    std::vector<QString>* feature_vec;
    std::map<QString,QString>* name_space_map;

public:
    static UIHelperSingleton* getInstance();
    std::vector<QString>* getFeatrueVector();
    std::map<QString,QString>* getNameSpaceMap();
    std::vector<QString>& getLabels();
};


#endif //UIHELPER_H__
