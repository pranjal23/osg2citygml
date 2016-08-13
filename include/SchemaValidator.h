#pragma once
#ifndef SCHEMAVALIDATOR_H__
#define SCHEMAVALIDATOR_H__

#include <QtCore>
#include <QtXmlPatterns>

class SchemaValidator
{
public:
    SchemaValidator(void)
    {

    }

    void validate()
    {
        QFile file("schema/CityGML.xsd");
        file.open(QIODevice::ReadOnly);


        QXmlSchema schema;
        schema.load(&file, QUrl::fromLocalFile(file.fileName()));

        if (schema.isValid())
            qDebug() << "CityGML schema is valid";
        else
            qDebug() << "CityGML schema is invalid";
    }
};

#endif //SCHEMAVALIDATOR_H__
