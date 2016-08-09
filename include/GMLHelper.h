#ifndef GMLNAMESPACE_H__
#define GMLNAMESPACE_H__

#include <QString>

class GMLNamespace {
public:
    /**
     * @brief CityGML Members
     *
     */
    const static QString AttributeName_id(void){return "id";}
    const static QString AttributeName_description(void){return "description";}
    const static QString AttributeName_name(void){return "name";}


    /**
     * @brief Relevant Property Names (<<Gemometry>>)
     */
    const static QString PropertyName_GEOMETRY_geometry(void){return "_Geometry";}
    const static QString PropertyName_GEOMETRY_geometricPrimitive (void){return  "_GeometricPrimitive";}
    const static QString PropertyName_GEOMETRY_DirectPosition (void){return  "DirectPosition";}

    /**
     * @brief Geometry - Solid
     * CompositeSolid has a list of SolidMembers, A SolidMember can contain
     * any Instance of Type _Solid, Solid or CompositeSolid
     */
    const static QString PropertyName_GEOMETRY_solid (void){return  "_Solid";}
    const static QString PropertyName_GEOMETRY_Solid (void){return  "Solid";}
    const static QString PropertyName_GEOMETRY_CompositeSolid (void){return  "CompositeSolid";}
    const static QString PropertyName_GEOMETRY_SolidMember (void){return  "SolidMember";}

    /**
     * @brief Geometry - Curve
     * CompositeSolid has a list of CurveMembers, A CurveMember can contain
     * any Instance of Type _Curve, LineString or CompositeCurve
     */
    const static QString PropertyName_GEOMETRY_curve (void){return  "_Curve";}
    const static QString PropertyName_GEOMETRY_LineString (void){return  "LineString";}
    const static QString PropertyName_GEOMETRY_CompositeCurve (void){return  "CompositeCurve";}
    const static QString PropertyName_GEOMETRY_CurveMember (void){return  "CurveMember";}

    /**
     * @brief Geometry - Point
     */
    const static QString PropertyName_GEOMETRY_Point (void){return  "Point";}

    /**
     * @brief Geometry - Surface
     */
    const static QString PropertyName_GEOMETRY_surface (void){return  "_Surface";}
    const static QString PropertyName_GEOMETRY_Surface (void){return  "Surface";}
    const static QString PropertyName_GEOMETRY_CompositeSurface (void){return  "CompositeSurface";}
    const static QString PropertyName_GEOMETRY_Polygon (void){return  "Polygon";}
    const static QString PropertyName_GEOMETRY_OrientableSurface (void){return  "OrientableSurface";}

};

#endif //GMLNAMESPACE_H__
