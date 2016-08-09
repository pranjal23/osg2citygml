#include "CityGMLWriter.h"
#include "CityGMLHelper.h"
#include "GMLHelper.h"
#include <QString>
#include <QFileInfo>

CityGMLWriter::CityGMLWriter(QString fileName)
{
    this->fileName = fileName;
}

static QString getElementTagFrom_Namespace_Keyword(QString ns, QString key)
{
    return ns + ":" + key;
}

void CityGMLWriter::writeDescription(QXmlStreamWriter& xmlWriter)
{
    QString elementName = getElementTagFrom_Namespace_Keyword(CityGMLNamespace::namespace_gml(),GMLNamespace::AttributeName_description());
    xmlWriter.writeStartElement(elementName);
    xmlWriter.writeCharacters(CityGMLNamespace::Application_Name());
    xmlWriter.writeEndElement();
}

void CityGMLWriter::writeName(QXmlStreamWriter& xmlWriter)
{
    QFileInfo fileInfo(this->fileName);
    QString filename(fileInfo.fileName());
    QString elementName =  getElementTagFrom_Namespace_Keyword(CityGMLNamespace::namespace_gml(),GMLNamespace::AttributeName_name());
    xmlWriter.writeStartElement(elementName);
    xmlWriter.writeCharacters(filename);
    xmlWriter.writeEndElement();
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

void CityGMLWriter::write(osg::Node* node)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    //Create CityGML object from OSG Node
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);

    writeNameSpaces(xmlWriter);

    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement(CityGMLBaseNamespace::PropertyName_FEATURE_CityModel());//CityModel
    xmlWriter.writeAttribute(CityGMLNamespace::namespace_xsi_loc(),CityGMLNamespace::ns_xsi_schema_location() ,CityGMLNamespace::schema_locations());

    writeDescription(xmlWriter);
    writeName(xmlWriter);

    xmlWriter.writeEndElement();//CityModel

    file.close();

}
