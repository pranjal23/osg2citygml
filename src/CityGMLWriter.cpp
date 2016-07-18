#include "CityGMLWriter.h"

CityGMLWriter::CityGMLWriter(QString fileName)
{
    this->fileName = fileName;
}

void CityGMLWriter::write(osg::Node* node)
{
    QXmlStreamWriter xmlWriter;
    QFile file(this->fileName);

    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(0, "Error!", "Error opening file");
    }
    else
    {
        xmlWriter.setDevice(&file);

        /* Writes a document start with the XML version number. */
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("students");

        xmlWriter.writeStartElement("student");
        /*add one attribute and its value*/
        xmlWriter.writeAttribute("name","Kate");
        /*add one attribute and its value*/
        xmlWriter.writeAttribute("surname","Johns");
        /*add one attribute and its value*/
        xmlWriter.writeAttribute("number","154455");
        /*add character data to tag student */
        xmlWriter.writeCharacters ("Student 1");
        /*close tag student  */
        xmlWriter.writeEndElement();

        /*end tag students*/
        xmlWriter.writeEndElement();
        /*end document */
        xmlWriter.writeEndDocument();
    }
}
