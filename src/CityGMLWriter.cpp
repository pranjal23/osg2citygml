#include "CityGMLWriter.h"
#include "CityGMLHelper.h"
#include "GMLHelper.h"
#include "OSGHelpers.h"
#include "UIHelper.h"

CityGMLWriter::CityGMLWriter(QString fileName)
{
    this->fileName = fileName;
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

void CityGMLWriter::writeGeometry(osg::Group* group , QXmlStreamWriter& xmlWriter, QString name_space, QString element_name)
{
    QList<TrianglePrimitive> list;

    unsigned int k;
    for (k = 0; k < group->getNumChildren(); k++)
    {
        osg::Geode* geode = (osg::Geode*)group->getChild(k);
        unsigned int i;
        for(i=0; i < geode->getNumDrawables(); i++)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));

            UserData* userData = dynamic_cast<UserData*>(geometry->getUserData());

            for(std::multimap<unsigned int,TrianglePrimitive>::iterator it = userData->allPrimitivesMap->begin();it!=userData->allPrimitivesMap->end();it++)
            {
                TrianglePrimitive a =(*it).second;
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

    //Write CityGMLElement from UI
    QString elementName2 =  getElementName(CityGMLNamespace::namespace_cityobjectGroup(),CityObjectGroupNamespace::FEATURE_groupMember());
    xmlWriter.writeStartElement(elementName2);

    if(name_space!=OSGHELPERS::DEFAULT_STR())
    {
        QString elementName3 =  getElementName(name_space,element_name);
        xmlWriter.writeStartElement(elementName3);
    }

    unsigned int j;
    for(j=0; j < list.size(); j++)
    {
        QString elementName4 =  getElementName(CityGMLNamespace::namespace_gml(),GMLNamespace::GEOMETRY_Triangle());
        xmlWriter.writeStartElement(elementName4);

        QString elementName5 =  getElementName(CityGMLNamespace::namespace_gml(),GMLNamespace::GEOMETRY_exterior());
        xmlWriter.writeStartElement(elementName5);

        QString elementName6 =  getElementName(CityGMLNamespace::namespace_gml(),GMLNamespace::GEOMETRY_LinearRing());
        xmlWriter.writeStartElement(elementName6);

        QString elementName7 =  getElementName(CityGMLNamespace::namespace_gml(),GMLNamespace::GEOMETRY_posList());
        xmlWriter.writeStartElement(elementName7);

        xmlWriter.writeAttribute(GMLNamespace::AttributeName_srsDimension() ,"3");

        TrianglePrimitive a = list.at(j);
        osg::PrimitiveSet* prset = a.drawable->asGeometry()->getPrimitiveSetList()[a.primitiveIndex];
        osg::Vec3Array* vecArr = dynamic_cast<osg::Vec3Array*>(a.drawable->asGeometry()->getVertexArray());
        xmlWriter.writeCharacters(OSGHELPERS::getPrimSetVerticesAsString(prset,vecArr));

        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();
    }

    if(name_space!=OSGHELPERS::DEFAULT_STR())
    {
        xmlWriter.writeEndElement();
    }

    xmlWriter.writeEndElement();
}

void CityGMLWriter::writeCityObjectGroup(osg::Group* group , QXmlStreamWriter& xmlWriter)
{
    QString elementName =  getElementName(CityGMLNamespace::namespace_cityobjectGroup(),CityObjectGroupNamespace::FEATURE_CityObjectGroup());
    xmlWriter.writeStartElement(elementName);

    UIHelperSingleton* uIHelperSingleton = UIHelperSingleton::getInstance();
    std::vector<QString>* feature_vector = uIHelperSingleton->getFeatrueVector();
    std::map<QString,QString>*name_space_map = uIHelperSingleton->getNameSpaceMap();

    unsigned int i;
    for( i=0; i<feature_vector->size(); i++)
    {
        QString element_name = feature_vector->at(i);
        QString name_space = (*name_space_map)[element_name];

        writeGeometry(group , xmlWriter, name_space, element_name);
    }

    xmlWriter.writeEndElement();
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
