#ifndef GMLHELPER_H__
#define GMLHELPER_H__

#include <QString>

class GML {

    /**
     * @brief CityGML Members
     *
     */

    QString namespace_xsi[2] = {"xsi", "http://www.w3.org/2001/XMLSchema-instance"};

    QString namespace_gml[2] = {"gml", "http://www.opengis.net/gml"};

    QString AttributeName_id = "id";

    /*
     * Relevant Property Names (<<Gemometry>>)
     */
    QString PropertyName_GEOMETRY_geometry = "_Geometry";
    QString PropertyName_GEOMETRY_geometricPrimitive = "_GeometricPrimitive";
    QString PropertyName_GEOMETRY_DirectPosition = "DirectPosition";

    /*
     * Geometry - Solid
     * CompositeSolid has a list of SolidMembers, A SolidMember can contain
     * any Instance of Type _Solid, Solid or CompositeSolid
     */
    QString PropertyName_GEOMETRY_solid = "_Solid";
    QString PropertyName_GEOMETRY_Solid = "Solid";
    QString PropertyName_GEOMETRY_CompositeSolid = "CompositeSolid";
    QString PropertyName_GEOMETRY_SolidMember = "SolidMember";

    /*
     * Geometry - Curve
     * CompositeSolid has a list of CurveMembers, A CurveMember can contain
     * any Instance of Type _Curve, LineString or CompositeCurve
     */
    QString PropertyName_GEOMETRY_curve = "_Curve";
    QString PropertyName_GEOMETRY_LineString = "LineString";
    QString PropertyName_GEOMETRY_CompositeCurve = "CompositeCurve";
    QString PropertyName_GEOMETRY_CurveMember = "CurveMember";

    /*
     * Geometry - Point
     */
    QString PropertyName_GEOMETRY_Point = "Point";

    /*
     * Geometry - Surface
     */
    QString PropertyName_GEOMETRY_surface = "_Surface";
    QString PropertyName_GEOMETRY_Surface = "Surface";
    QString PropertyName_GEOMETRY_CompositeSurface = "CompositeSurface";
    QString PropertyName_GEOMETRY_Polygon = "Polygon";
    QString PropertyName_GEOMETRY_OrientableSurface = "OrientableSurface";

};

#endif //GMLHELPER_H__
