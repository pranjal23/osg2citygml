#ifndef CITYGMLHELPER_H__
#define CITYGMLHELPER_H__

#include <QString>

class CityGMLNamespace {
    /**
     * @brief CityGML Members
     *
     */

public:
    static QString colon = ":";

    static QString namespace_citygml = "http://www.opengis.net/citygml/2.0";

    static QString namespace_appearance[2] = {"app", "http://www.opengis.net/citygml/appearance/2.0"};

    static QString namespace_bridge[2] = {"brdg", "http://www.opengis.net/citygml/bridge/2.0"};

    static QString namespace_building[2] = {"bldg", "http://www.opengis.net/citygml/building/2.0"};

    static QString namespace_citygmlbase[2] = {"core", "http://www.opengis.net/citygml/citygmlbase/2.0"};

    static QString namespace_cityfurniture[2] = {"frn", "http://www.opengis.net/citygml/cityfurniture/2.0"};

    static QString namespace_cityobjectGroup[2] ={"grp", "http://www.opengis.net/citygml/cityobjectgroup/2.0"};

    static QString namespace_generics[2] = {"gen", "http://www.opengis.net/citygml/generics/2.0"};

    static QString namespace_landuse[2] = {"luse", "http://www.opengis.net/citygml/landuse/2.0"};

    static QString namespace_relief[2] = {"dem", "http://www.opengis.net/citygml/relief/2.0"};

    static QString namespace_transportation[2] = {"tran" , "http://www.opengis.net/citygml/transportation/2.0"};

    static QString namespace_tunnel[2] = {"tun", "http://www.opengis.net/citygml/tunnel/2.0"};

    static QString namespace_vegetation[2] = {"veg", "http://www.opengis.net/citygml/vegetation/2.0"};

    static QString namespace_waterBody[2] = {"wtr", "http://www.opengis.net/citygml/waterbody/2.0"};

    static QString namespace_texturedsurface[2] = {"tex", "http://www.opengis.net/citygml/texturedsurface/2.0"};


};

class CityGMLNameSpaceObject{
public:
    QString* nameSpaceId;

    CityGMLBaseTypedObject(QString* namespaceid)
    {
        nameSpaceId = namespaceid;
    }
};

static bool isCityGMLNameSpaceObjectEqual(const CityGMLNameSpaceObject& s1, const CityGMLNameSpaceObject& s2)
{
    /**
     * @brief
     * Returns 0 if the string Ids are equal
     * */
    return QString::compare(s1,s2,Qt::CaseSensitivity::CaseSensitive)==0;
}

class AppearanceNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/appearance.xsd"
      * Class representing the CityGML app namespace
      */
};

class BridgeNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/bridge.xsd"
      * Class representing the CityGML brdg namespace
      */

};

class BuildingNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/building.xsd"
      * Class representing the CityGML bldg namespace
      */


};

class CityGMLBaseNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/cityGMLBase.xsd";
      * Class representing the CityGML core namespace
      */



    static QString PropertyName_FEATURE_CityModel = "CityModel";
    static QString PropertyName_FEATURE_CityObjectMember = "cityObjectMember";
    static QString PropertyName_FEATURE_Address = "Address";
    static QString PropertyName_OBJECT_ImplicitGeometry = "ImplicitGeometry";

    QString ns_core_CityModel = CityGMLNamespace::namespace_citygmlbase[0]
            + CityGMLNamespace::colon
            + PropertyName_FEATURE_CityModel;

    QString ns_core_CityModel = CityGMLNamespace::namespace_citygmlbase[0]
            + CityGMLNamespace::colon
            + PropertyName_FEATURE_CityObjectMember;

};

class CityFurnitureNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/cityFurniture.xsd"
      * Class representing the CityGML frn namespace
      */

};

class CityObjectGroupNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/cityObjectGroup.xsd"
      * Class representing the CityGML grp namespace
      */

};

class GenericCityObjectNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/generics.xsd"
      * Class representing the CityGML gen namespace
      */

};

class LandUseNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/landUse.xsd"
      * Class representing the CityGML luse namespace
      */

};

class ReliefNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/relief.xsd"
      * Class representing the CityGML dem namespace
      */

};

class TransportationNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/transportation.xsd"
      * Class representing the CityGML tran namespace
      */

};

class TunnelNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/tunnel.xsd"
      * Class representing the CityGML tun namespace
      */

};

class VegetationNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/vegetation.xsd"
      * Class representing the CityGML veg namespace
      */

};

class WaterObjectNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/waterBody.xsd"
      * Class representing the CityGML wtr namespace
      */

};

class TexturedSurfaceNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/texturedSurface.xsd"
      * Class representing the CityGML tex namespace
      */

};


#endif //CITYGMLHELPER_H__
