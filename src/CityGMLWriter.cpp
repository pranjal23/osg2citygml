#include "CityGMLWriter.h"

CityGMLWriter::CityGMLWriter(QString fileName)
{
    this->fileName = fileName;
}

void CityGMLWriter::write(osg::Node* node)
{
    //Create CityGML object from OSG Node
    //citygml::CityObject* cityObject = new citygml::CityObject();
}
