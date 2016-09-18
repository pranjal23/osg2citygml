#include "CityGMLWriter.h"
#include "CityGMLHelper.h"
#include "GMLHelper.h"
#include "GeometryHandler.h"
#include "UIHelper.h"

CityGMLWriter::CityGMLWriter(QString fileName, OSGWidget* osgwidget)
{
    this->fileName = fileName;
    this->osgwidget = osgwidget;
}

static QString getElementName(QString ns, QString key)
{
    return ns + ":" + key;
}

void CityGMLWriter::writeNameSpaces(QXmlStreamWriter& xmlWriter)
{
    xmlWriter.writeDefaultNamespace( CityGMLNamespace::namespace_citygml() );

    xmlWriter.writeNamespace( CityGMLNamespace::namespace_gml_loc(), CityGMLNamespace::namespace_gml() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_xsi_loc(), CityGMLNamespace::namespace_xsi() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_xAL_loc(), CityGMLNamespace::namespace_xAL() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_xlink_loc(), CityGMLNamespace::namespace_xlink() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_smil20lang_loc(), CityGMLNamespace::namespace_smil20lang() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_smil20_loc(), CityGMLNamespace::namespace_smil20() );

    xmlWriter.writeNamespace( CityGMLNamespace::namespace_appearance_loc(), CityGMLNamespace::namespace_appearance() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_building_loc(), CityGMLNamespace::namespace_building() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_bridge_loc(), CityGMLNamespace::namespace_bridge() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_cityfurniture_loc(), CityGMLNamespace::namespace_cityfurniture() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_citygmlbase_loc(), CityGMLNamespace::namespace_citygmlbase() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_cityobjectGroup_loc(), CityGMLNamespace::namespace_cityobjectGroup() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_generics_loc(), CityGMLNamespace::namespace_generics() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_landuse_loc(), CityGMLNamespace::namespace_landuse() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_relief_loc(), CityGMLNamespace::namespace_relief() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_texturedsurface_loc(), CityGMLNamespace::namespace_texturedsurface() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_transportation_loc(), CityGMLNamespace::namespace_transportation() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_tunnel_loc(), CityGMLNamespace::namespace_tunnel() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_vegetation_loc(), CityGMLNamespace::namespace_vegetation() );
    xmlWriter.writeNamespace( CityGMLNamespace::namespace_waterBody_loc(), CityGMLNamespace::namespace_waterBody() );
}

void CityGMLWriter::writeDescription(QXmlStreamWriter& xmlWriter)
{
    QString elementName = getElementName(CityGMLNamespace::namespace_gml(),GMLNamespace::AttributeName_description());
    xmlWriter.writeStartElement(elementName);
    xmlWriter.writeCharacters(CityGMLNamespace::Application_Name());
    xmlWriter.writeEndElement();
}

void CityGMLWriter::writeName(QXmlStreamWriter& xmlWriter)
{
    QFileInfo fileInfo(this->fileName);
    QString filename(fileInfo.fileName());
    QString elementName =  getElementName(CityGMLNamespace::namespace_gml(),GMLNamespace::AttributeName_name());
    xmlWriter.writeStartElement(elementName);
    xmlWriter.writeCharacters(filename);
    xmlWriter.writeEndElement();
}

void CityGMLWriter::writeBuildingGeometry(osg::Group* group , QXmlStreamWriter& xmlWriter, QString name_space, QString element_name)
{
    QList<PrimitiveNode> list;

    unsigned int k;
    for (k = 0; k < group->getNumChildren(); k++)
    {
        osg::Geode* geode = (osg::Geode*)group->getChild(k);
        unsigned int i;
        for(i=0; i < geode->getNumDrawables(); i++)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));

            GraphData* userData = dynamic_cast<GraphData*>(geometry->getUserData());

            for(std::map<unsigned int,PrimitiveNode>::iterator it = userData->primitivesMap->begin();it!=userData->primitivesMap->end();it++)
            {
                PrimitiveNode a =(*it).second;
                if(a.element_name == element_name
                        && a.name_space == name_space)
                {
                    list.push_back(a);
                }
            }
        }
    }

    if(list.size()<=0)
        return;

    QString elementName1 =  getElementName(name_space,BuildingNamespace::FEATURE_Building());
    xmlWriter.writeStartElement(elementName1);

    unsigned int j;
    for(j=0; j < list.size(); j++)
    {
        QString elementName2 =  getElementName(name_space,BuildingNamespace::META_boundedBy());
        xmlWriter.writeStartElement(elementName2);

        QString elementName3 =  getElementName(name_space,element_name);
        xmlWriter.writeStartElement(elementName3);

        QString elementName4 =  getElementName(name_space,BuildingNamespace::TYPE_lod2MultiSurface());
        xmlWriter.writeStartElement(elementName4);

        QString elementName5 =  getElementName(CityGMLNamespace::namespace_gml(),GMLNamespace::GEOMETRY_MultiSurface());
        xmlWriter.writeStartElement(elementName5);

        QString elementName6 =  getElementName(CityGMLNamespace::namespace_gml(),GMLNamespace::GEOMETRY_surfaceMember());
        xmlWriter.writeStartElement(elementName6);



        //This can be reused at other places
        {
        QString elementName40 =  getElementName(CityGMLNamespace::namespace_gml(),GMLNamespace::GEOMETRY_Polygon());
        xmlWriter.writeStartElement(elementName40);

        QString elementName50 =  getElementName(CityGMLNamespace::namespace_gml(),GMLNamespace::GEOMETRY_exterior());
        xmlWriter.writeStartElement(elementName50);

        QString elementName60 =  getElementName(CityGMLNamespace::namespace_gml(),GMLNamespace::GEOMETRY_LinearRing());
        xmlWriter.writeStartElement(elementName60);

        PrimitiveNode a = list.at(j);
        osg::PrimitiveSet* prset = a.drawable->asGeometry()->getPrimitiveSetList()[a.primitiveIndex];
        osg::Vec3Array* vecArr = dynamic_cast<osg::Vec3Array*>(a.drawable->asGeometry()->getVertexArray());

        QList<QString> vertexStrlist = OSGHELPERS::getPrimSetVerticesAsString(prset,vecArr);

        unsigned int i;
        for (i=0; i < vertexStrlist.size(); i++)
        {
            QString elementName7 =  getElementName(CityGMLNamespace::namespace_gml(),GMLNamespace::GEOMETRY_pos());
            xmlWriter.writeStartElement(elementName7);
            xmlWriter.writeCharacters(vertexStrlist.at(i));
            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement();//40

        xmlWriter.writeEndElement();//50

        xmlWriter.writeEndElement();//60
        }


        xmlWriter.writeEndElement(); //6

        xmlWriter.writeEndElement(); //5

        xmlWriter.writeEndElement(); //4

        xmlWriter.writeEndElement(); //3

        xmlWriter.writeEndElement(); //2

    }


    xmlWriter.writeEndElement();
}

void CityGMLWriter::writeCityObjectGroup(osg::Group* group , QXmlStreamWriter& xmlWriter)
{
    if(osgwidget->cityObjectGroup)
    {
        QString elementName =  getElementName(CityGMLNamespace::namespace_cityobjectGroup(),CityObjectGroupNamespace::FEATURE_CityObjectGroup());
        xmlWriter.writeStartElement(elementName);

        QString elementName2 =  getElementName(CityGMLNamespace::namespace_cityobjectGroup(),CityObjectGroupNamespace::FEATURE_groupMember());
        xmlWriter.writeStartElement(elementName2);
    }

    UIHelperSingleton* uIHelperSingleton = UIHelperSingleton::getInstance();
    std::vector<QString>* feature_vector = uIHelperSingleton->getFeatrueVector();
    std::map<QString,QString>*name_space_map = uIHelperSingleton->getNameSpaceMap();

    unsigned int i;
    for( i=0; i<feature_vector->size(); i++)
    {
        QString element_name = feature_vector->at(i);
        QString name_space = (*name_space_map)[element_name];

        if(name_space==CityGMLNamespace::namespace_building())
        {
            writeBuildingGeometry(group , xmlWriter, name_space, element_name);
        }
    }

    if(osgwidget->cityObjectGroup)
    {
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();
    }
}

void CityGMLWriter::writeCityObjectMember(osg::Group* group, QXmlStreamWriter& xmlWriter)
{
    QString elementName =  getElementName(CityGMLNamespace::namespace_citygmlbase(),CityGMLBaseNamespace::FEATURE_CityObjectMember());
    xmlWriter.writeStartElement(elementName);

    writeCityObjectGroup(group,xmlWriter);

    xmlWriter.writeEndElement();
}

void CityGMLWriter::write(osg::Group* group)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    //Create CityGML object from OSG Node
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);

    writeNameSpaces(xmlWriter);

    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement(CityGMLBaseNamespace::FEATURE_CityModel());//CityModel
    xmlWriter.writeAttribute(CityGMLNamespace::namespace_xsi_loc(),CityGMLNamespace::ns_xsi_schema_location() ,CityGMLNamespace::schema_locations());

    writeDescription(xmlWriter);
    writeName(xmlWriter);
    writeCityObjectMember(group,xmlWriter);

    xmlWriter.writeEndElement();//CityModel

    file.close();

}
