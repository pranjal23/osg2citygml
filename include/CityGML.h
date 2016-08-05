#ifndef CITYGML_H__
#define CITYGML_H__

class CityGML {
    /**
     * @brief CityGML Members
     *
     */

    QString namespace_citygml = "http://www.opengis.net/citygml/2.0";

    QString namespace_appearance[2] = {"app", "http://www.opengis.net/citygml/appearance/2.0"};

    QString namespace_bridge[2] = {"brdg", "http://www.opengis.net/citygml/bridge/2.0"};

    QString namespace_building[2] = {"bldg", "http://www.opengis.net/citygml/building/2.0"};

    QString namespace_citygmlbase[2] = {"core", "http://www.opengis.net/citygml/citygmlbase/2.0"};

    QString namespace_cityfurniture[2] = {"frn", "http://www.opengis.net/citygml/cityfurniture/2.0"};

    QString namespace_cityobjectGroup[2] ={"grp", "http://www.opengis.net/citygml/cityobjectgroup/2.0"};

    QString namespace_generics[2] = {"gen", "http://www.opengis.net/citygml/generics/2.0"};

    QString namespace_landuse[2] = {"luse", "http://www.opengis.net/citygml/landuse/2.0"};

    QString namespace_relief[2] = {"dem", "http://www.opengis.net/citygml/relief/2.0"};

    QString namespace_transportation[2] = {"tran" , "http://www.opengis.net/citygml/transportation/2.0"};

    QString namespace_tunnel[2] = {"tun", "http://www.opengis.net/citygml/tunnel/2.0"};

    QString namespace_vegetation[2] = {"veg", "http://www.opengis.net/citygml/vegetation/2.0"};

    QString namespace_waterBody[2] = {"wtr", "http://www.opengis.net/citygml/waterbody/2.0"};

    QString namespace_texturedsurface[2] = {"tex", "http://www.opengis.net/citygml/texturedsurface/2.0"};

};

class Appearance {
    /**
      * @brief
      * Schema Location = "CityGML/appearance.xsd"
      * Class representing the CityGML app namespace
      */

};

class Bridge {
    /**
      * @brief
      * Schema Location = "CityGML/bridge.xsd"
      * Class representing the CityGML brdg namespace
      */

};

class Building {
    /**
      * @brief
      * Schema Location = "CityGML/building.xsd"
      * Class representing the CityGML bldg namespace
      */

};

class CityGMLBase {
    /**
      * @brief
      * Schema Location = "CityGML/cityGMLBase.xsd";
      * Class representing the CityGML core namespace
      */

    QString PropertyName_FEATURE_CityModel = "CityModel";
    QString PropertyName_FEATURE_CityObjectMember = "cityObjectMember";
    QString PropertyName_FEATURE_Address = "Address";
    QString PropertyName_OBJECT_ImplicitGeometry = "ImplicitGeometry";
};

class CityFurniture {
    /**
      * @brief
      * Schema Location = "CityGML/cityFurniture.xsd"
      * Class representing the CityGML frn namespace
      */

};

class CityObjectGroup {
    /**
      * @brief
      * Schema Location = "CityGML/cityObjectGroup.xsd"
      * Class representing the CityGML grp namespace
      */

};

class GenericCityObject {
    /**
      * @brief
      * Schema Location = "CityGML/generics.xsd"
      * Class representing the CityGML gen namespace
      */

};

class LandUse {
    /**
      * @brief
      * Schema Location = "CityGML/landUse.xsd"
      * Class representing the CityGML luse namespace
      */

};

class Relief {
    /**
      * @brief
      * Schema Location = "CityGML/relief.xsd"
      * Class representing the CityGML dem namespace
      */

};

class Transportation {
    /**
      * @brief
      * Schema Location = "CityGML/transportation.xsd"
      * Class representing the CityGML tran namespace
      */

};

class Tunnel {
    /**
      * @brief
      * Schema Location = "CityGML/tunnel.xsd"
      * Class representing the CityGML tun namespace
      */

};

class Vegetation {
    /**
      * @brief
      * Schema Location = "CityGML/vegetation.xsd"
      * Class representing the CityGML veg namespace
      */

};

class WaterObject {
    /**
      * @brief
      * Schema Location = "CityGML/waterBody.xsd"
      * Class representing the CityGML wtr namespace
      */

};

class TexturedSurface {
    /**
      * @brief
      * Schema Location = "CityGML/texturedSurface.xsd"
      * Class representing the CityGML tex namespace
      */

};


#endif
