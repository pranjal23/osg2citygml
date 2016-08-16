#pragma once
#ifndef CITYGMLHELPER_H__
#define CITYGMLHELPER_H__

#include <QString>

class CityGMLNamespace {
    /**
     * @brief CityGML Members
     *
     */

public:
    const static QString Application_Name(void){return "OSG2CITYGML";}

    static const QString namespace_citygml(void){return "http://www.opengis.net/citygml/2.0";}

    static const QString namespace_gml(void){return "gml";}
    static const QString namespace_gml_loc(void){return "http://www.opengis.net/gml";}

    static const QString namespace_xsi(void){return "xsi";}
    static const QString namespace_xsi_loc(void){return "http://www.w3.org/2001/XMLSchema-instance";}


    static const QString namespace_xlink(void){return "xlink";}
    static const QString namespace_xlink_loc(void){return "http://www.w3.org/1999/xlink"; }

    static const QString namespace_smil20lang(void){return "smil20lang";}
    static const QString namespace_smil20lang_loc(void){return "http://www.w3.org/2001/SMIL20/Language";}

    static const QString namespace_smil20(void){return "smil20";}
    static const QString namespace_smil20_loc(void){return "http://www.w3.org/2001/SMIL20/";}

    static const QString namespace_xAL(void){return "xAL";}
    static const QString namespace_xAL_loc(void){return "urn:oasis:names:tc:ciq:xsdschema:xAL:2.0";}

    static const QString namespace_appearance(void){return "app";}
    static const QString namespace_appearance_loc(void){return "http://www.opengis.net/citygml/appearance/2.0";}

    static const QString namespace_bridge(void){return "brdg";}
    static const QString namespace_bridge_loc(void){return "http://www.opengis.net/citygml/bridge/2.0";}

    static const QString namespace_building(void){return "bldg";}
    static const QString namespace_building_loc(void){return "http://www.opengis.net/citygml/building/2.0";}

    static const QString namespace_citygmlbase(void){return "core";}
    const static QString namespace_citygmlbase_loc(void){return "http://www.opengis.net/citygml/citygmlbase/2.0";}

    const static QString namespace_cityfurniture(void){return "frn";}
    const static QString namespace_cityfurniture_loc(void){return "http://www.opengis.net/citygml/cityfurniture/2.0";}

    const static QString namespace_cityobjectGroup(void){return "grp";}
    const static QString namespace_cityobjectGroup_loc(void){return "http://www.opengis.net/citygml/cityobjectgroup/2.0";}

    const static QString namespace_generics(void){return "gen";}
    const static QString namespace_generics_loc(void){return "http://www.opengis.net/citygml/generics/2.0";}

    const static QString namespace_landuse(void){return "luse";}
    const static QString namespace_landuse_loc(void){return "http://www.opengis.net/citygml/landuse/2.0";}

    const static QString namespace_relief(void){return "dem";}
    const static QString namespace_relief_loc(void){return "http://www.opengis.net/citygml/relief/2.0";}

    const static QString namespace_transportation(void){return "tran";}
    const static QString namespace_transportation_loc(void){return "http://www.opengis.net/citygml/transportation/2.0";}

    const static QString namespace_tunnel(void){return "tun";}
    const static QString namespace_tunnel_loc(void){return "http://www.opengis.net/citygml/tunnel/2.0";}

    const static QString namespace_vegetation(void){return "veg";}
    const static QString namespace_vegetation_loc(void){return "http://www.opengis.net/citygml/vegetation/2.0";}

    const static QString namespace_waterBody(void){return "wtr";}
    const static QString namespace_waterBody_loc(void){return "http://www.opengis.net/citygml/waterbody/2.0";}

    const static QString namespace_texturedsurface(void){return "tex";}
    const static QString namespace_texturedsurface_loc(void){return "http://www.opengis.net/citygml/texturedsurface/2.0";}

    const static QString ns_xsi_schema_location(void) {return "schemaLocation";}
    const static QString schema_locations(void){return "http://www.opengis.net/citygml/relief/1.0 "
                                      "http://schemas.opengis.net/citygml/relief/1.0/relief.xsd "
                                      "http://www.opengis.net/citygml/landuse/1.0 "
                                      "http://schemas.opengis.net/citygml/landuse/1.0/landUse.xsd "
                                      "http://www.opengis.net/citygml/building/1.0 "
                                      "http://schemas.opengis.net/citygml/building/1.0/building.xsd "
                                      "http://www.opengis.net/citygml/cityobjectgroup/1.0 "
                                      "http://schemas.opengis.net/citygml/cityobjectgroup/1.0/cityObjectGroup.xsd "
                                      "http://www.opengis.net/citygml/cityfurniture/1.0 "
                                      "http://schemas.opengis.net/citygml/cityfurniture/1.0/cityFurniture.xsd "
                                      "http://www.opengis.net/citygml/appearance/1.0 "
                                      "http://schemas.opengis.net/citygml/appearance/1.0/appearance.xsd "
                                      "http://www.opengis.net/citygml/texturedsurface/1.0 "
                                      "http://schemas.opengis.net/citygml/texturedsurface/1.0/texturedSurface.xsd  "
                                      "http://www.opengis.net/citygml/transportation/1.0 "
                                      "http://schemas.opengis.net/citygml/transportation/1.0/transportation.xsd "
                                      "http://www.opengis.net/citygml/waterbody/1.0 "
                                      "http://schemas.opengis.net/citygml/waterbody/1.0/waterBody.xsd "
                                      "http://www.opengis.net/citygml/vegetation/1.0 "
                                      "http://schemas.opengis.net/citygml/vegetation/1.0/vegetation.xsd "
                                      "http://www.opengis.net/citygml/generics/1.0 "
                                      "http://schemas.opengis.net/citygml/generics/1.0/generics.xsd";}

};

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
public:
    const static QString FEATURE_Building(void){return "Building";}
    const static QString FEATURE_BuildingPart(void){return "BuildingPart";}
    const static QString FEATURE_RoofSurface(void){return "RoofSurface";}
    const static QString FEATURE_WallSurface(void){return "WallSurface";}
    const static QString FEATURE_GroundSurface(void){return "GroundSurface";}
    const static QString FEATURE_ClosureSurface(void){return "ClosureSurface";}
    const static QString FEATURE_CeilingSurface(void){return "CeilingSurface";}
    const static QString FEATURE_InteriorWallSurface(void){return "InteriorWallSurface";}
    const static QString FEATURE_FloorSurface(void){return "FloorSurface";}
    const static QString FEATURE_OuterCeilingSurface(void){return "OuterCeilingSurface";}
    const static QString FEATURE_OuterFloorSurface(void){return "OuterFloorSurface";}

    const static QString FEATURE_Door(void){return "Door";}
    const static QString FEATURE_Window(void){return "Window";}

    const static QString TYPE_lod1MultiSurface(void){return "lod1MultiSurface";}
    const static QString TYPE_lod2MultiSurface(void){return "lod2MultiSurface";}
    const static QString TYPE_lod3MultiSurface(void){return "lod3MultiSurface";}
    const static QString TYPE_lod4MultiSurface(void){return "lod4MultiSurface";}

    const static QString META_boundedBy(void){return "boundedBy";}
};

class CityGMLBaseNamespace {
    /**
      * @brief
      * Schema Location = "CityGML/cityGMLBase.xsd";
      * Class representing the CityGML core namespace
      */
public:
    const static QString FEATURE_CityModel(void){return "CityModel";}
    const static QString FEATURE_CityObjectMember(void){return "cityObjectMember";}
    const static QString FEATURE_Address(void){return "Address";}
    const static QString OBJECT_ImplicitGeometry(void){return "ImplicitGeometry";}
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
public:
    const static QString FEATURE_CityObjectGroup(void){return "CityObjectGroup";}
    const static QString FEATURE_groupMember(void){return "groupMember";}
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
