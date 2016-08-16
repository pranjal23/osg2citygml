#include "UIHelper.h"

static UIHelperSingleton* theUIHelper = nullptr;

UIHelperSingleton::UIHelperSingleton()
{
    initialize();
}

UIHelperSingleton* UIHelperSingleton::getInstance()
{
    if(!theUIHelper)
    {
        theUIHelper = new UIHelperSingleton();
    }

    return theUIHelper;
}

void UIHelperSingleton::initialize(void)
{
    feature_vec = new std::vector<QString>();
    name_space_map = new std::map<QString,QString>();

    //Add all relevant building elements for UI
    QString building_namespace = CityGMLNamespace::namespace_building();

    //QString Building_feature = BuildingNamespace::FEATURE_Building();
    //feature_vec->push_back(Building_feature);
    //name_space_map->insert(std::make_pair(Building_feature,building_namespace));

    //QString BuildingPart_feature = BuildingNamespace::FEATURE_BuildingPart();
    //feature_vec->push_back(BuildingPart_feature);
    //name_space_map->insert(std::make_pair(BuildingPart_feature,building_namespace));

    QString RoofSurface_feature = BuildingNamespace::FEATURE_RoofSurface();
    feature_vec->push_back(RoofSurface_feature);
    name_space_map->insert(std::make_pair(RoofSurface_feature,building_namespace));

    QString WallSurface_feature = BuildingNamespace::FEATURE_WallSurface();
    feature_vec->push_back(WallSurface_feature);
    name_space_map->insert(std::make_pair(WallSurface_feature,building_namespace));

    QString CeilingSurface_feature = BuildingNamespace::FEATURE_CeilingSurface();
    feature_vec->push_back(CeilingSurface_feature);
    name_space_map->insert(std::make_pair(CeilingSurface_feature,building_namespace));

    QString ClosureSurface_feature = BuildingNamespace::FEATURE_ClosureSurface();
    feature_vec->push_back(ClosureSurface_feature);
    name_space_map->insert(std::make_pair(ClosureSurface_feature,building_namespace));

    QString FloorSurface_feature = BuildingNamespace::FEATURE_FloorSurface();
    feature_vec->push_back(FloorSurface_feature);
    name_space_map->insert(std::make_pair(FloorSurface_feature,building_namespace));

    QString GroundSurface_feature = BuildingNamespace::FEATURE_GroundSurface();
    feature_vec->push_back(GroundSurface_feature);
    name_space_map->insert(std::make_pair(GroundSurface_feature,building_namespace));

    QString InteriorWallSurface = BuildingNamespace::FEATURE_InteriorWallSurface();
    feature_vec->push_back(InteriorWallSurface);
    name_space_map->insert(std::make_pair(InteriorWallSurface,building_namespace));

    QString OuterCeilingSurface_feature = BuildingNamespace::FEATURE_OuterCeilingSurface();
    feature_vec->push_back(OuterCeilingSurface_feature);
    name_space_map->insert(std::make_pair(OuterCeilingSurface_feature,building_namespace));

    QString OuterFloorSurface_feature = BuildingNamespace::FEATURE_OuterFloorSurface();
    feature_vec->push_back(OuterFloorSurface_feature);
    name_space_map->insert(std::make_pair(OuterFloorSurface_feature,building_namespace));

    //Add the default namespace and element
    //feature_vec->push_back(OSGHELPERS::DEFAULT_STR());
    //name_space_map->insert(std::make_pair(OSGHELPERS::DEFAULT_STR(),OSGHELPERS::DEFAULT_STR()));

}

std::vector<QString>* UIHelperSingleton::getFeatrueVector()
{
    return feature_vec;
}

std::map<QString,QString>* UIHelperSingleton::getNameSpaceMap()
{
    return name_space_map;
}

std::vector<QString>& UIHelperSingleton::getLabels()
{
    std::vector<QString>* vect = new std::vector<QString>();
    for(int i=0; i<feature_vec->size();i++)
    {
        vect->push_back((*name_space_map)[feature_vec->at(i)] + ":" + feature_vec->at(i));
    }
    return *vect;
}
