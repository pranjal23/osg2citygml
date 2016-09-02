#pragma once
#ifndef OSGHELPERS_H__
#define OSGHELPERS_H__

#include <sstream>
#include <iomanip>

#include <QDebug>
#include <QException>
#include <QSet>

#include <osg/Group>
#include <osg/Vec3>
#include <osg/Referenced>

#include <osgUtil/Simplifier>
#include <osgUtil/TriStripVisitor>
#include <osgUtil/SmoothingVisitor>
#include <osgUtil/MeshOptimizers>

class OSGHELPERS{
public:
    const static QString DEFAULT_STR(void){return "DEFAULT";}

    static void printPrimSet(osg::PrimitiveSet* prset)
    {
        unsigned int ic;

        qDebug() << "Prim set type - " << prset->getMode() << ", Vertex Ids: ";

        for (ic=0; ic < prset->getNumIndices(); ic++)
        {
            unsigned int vertexId = prset->index(ic);
            qDebug() << vertexId;
        }

        qDebug() << "";
    }

    static QList<QString> getPrimSetVerticesAsString(osg::PrimitiveSet* prset, osg::Vec3Array *verts)
    {
        if(prset->getNumIndices()<3)
        {
            throw new QException();
        }

        QList<QString> list;

        unsigned int ic;
        for (ic=0; ic < prset->getNumIndices(); ic++)
        {
            unsigned int vertexId = prset->index(ic);
            list.push_back(
                        QString::number((* verts)[vertexId].x()) + " " +
                        QString::number((* verts)[vertexId].y()) + " " +
                        QString::number((* verts)[vertexId].z())
                        );
        }

        QString firstVertex = list.at(0);
        list.push_back(firstVertex);

        return list;
    }

    static void printVertexArray(unsigned int vertexId, const osg::Vec3Array *verts)
    {
        osg::notify(osg::WARN) << "Vertex Id: "<< vertexId << ", coordinates: " <<
                                  (* verts)[vertexId].x() << "," <<
                                  (* verts)[vertexId].y() << "," <<
                                  (* verts)[vertexId].z() << std::endl;
    }

    static void printVertex(const osg::Vec3 vert)
    {
        osg::notify(osg::WARN) <<
                                  vert.x() << "," <<
                                  vert.y() << "," <<
                                  vert.z() << std::endl;
    }

    static void printPrimSets(osg::Geode& geode){
        for(unsigned int i=0;i<geode.getNumDrawables();i++)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));

            if(geometry && geometry->getNumPrimitiveSets()>0)
            {
                for (unsigned int ipr=0; ipr<geometry->getNumPrimitiveSets(); ipr++)
                {
                    osg::PrimitiveSet* prset=geometry->getPrimitiveSet(ipr);
                    //printPrimSet(prset);

                    if(prset->getMode()!=osg::PrimitiveSet::TRIANGLES)
                    {
                        qDebug() << "PRSET TYPE: " << prset->getMode() << ", NOT TRIANGLES" ;
                    }
                }
            }
        }
    }

    osg::Geode* getTriangleGeode()
    {
        osg::Geode* pyramidGeode = new osg::Geode();
        osg::Geometry* pyramidGeometry = new osg::Geometry();

        pyramidGeode->addDrawable(pyramidGeometry);

        osg::Vec3Array* pyramidVertices = new osg::Vec3Array;
        pyramidVertices->push_back( osg::Vec3( 0, 0, 0) ); // front left
        pyramidVertices->push_back( osg::Vec3(10, 0, 0) ); // front right
        pyramidVertices->push_back( osg::Vec3(10,10, 0) ); // back right
        pyramidVertices->push_back( osg::Vec3( 0,10, 0) ); // back left
        pyramidVertices->push_back( osg::Vec3( 5, 5,10) ); // peak

        pyramidGeometry->setVertexArray( pyramidVertices );

        osg::DrawElementsUInt* pyramidBase =
                new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
        pyramidBase->push_back(3);
        pyramidBase->push_back(2);
        pyramidBase->push_back(1);
        pyramidBase->push_back(0);
        pyramidGeometry->addPrimitiveSet(pyramidBase);

        osg::DrawElementsUInt* pyramidFaceOne =
                new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        pyramidFaceOne->push_back(0);
        pyramidFaceOne->push_back(1);
        pyramidFaceOne->push_back(4);
        pyramidGeometry->addPrimitiveSet(pyramidFaceOne);

        osg::DrawElementsUInt* pyramidFaceTwo =
                new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        pyramidFaceTwo->push_back(1);
        pyramidFaceTwo->push_back(2);
        pyramidFaceTwo->push_back(4);
        pyramidGeometry->addPrimitiveSet(pyramidFaceTwo);

        osg::DrawElementsUInt* pyramidFaceThree =
                new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        pyramidFaceThree->push_back(2);
        pyramidFaceThree->push_back(3);
        pyramidFaceThree->push_back(4);
        pyramidGeometry->addPrimitiveSet(pyramidFaceThree);

        osg::DrawElementsUInt* pyramidFaceFour =
                new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        pyramidFaceFour->push_back(3);
        pyramidFaceFour->push_back(0);
        pyramidFaceFour->push_back(4);
        pyramidGeometry->addPrimitiveSet(pyramidFaceFour);

        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); //index 0 red
        colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); //index 1 green
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); //index 2 blue
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 3 white
        colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); //index 4 red

        pyramidGeometry->setColorArray(colors);
        pyramidGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

        return pyramidGeode;
    }

};

class MathHelper
{
public:
    template <typename T>
    static std::string to_string_with_precision(const T a_value, const int n = 6)
    {
        std::ostringstream out;
        out << std::setprecision(n) << a_value;
        return out.str();
    }
};

class VertexLinks
{
public:
    QSet<unsigned int>* primitiveIds;
    VertexLinks()
    {
        primitiveIds = new QSet<unsigned int>();
    }
};

class Link
{
public:
    osg::ref_ptr<osg::Drawable> drawable;
    unsigned int primitiveIndex = -1;

    /**
     * @brief Method from which weight is generated
     * 1. Maximum weight is 100
     */
    unsigned int weight=0;

    int compare(const Link& link) const
    {
        if(this->drawable != link.drawable)
        {
            if(this->drawable > link.drawable)
            {
                return 1;
            }
            else if(this->drawable < link.drawable)
            {
                return -1;
            }
        }

        if(this->drawable==link.drawable)
        {
            if(this->primitiveIndex > link.primitiveIndex)
            {
                return 1;
            }
            else if(this->primitiveIndex < link.primitiveIndex)
            {
                return -1;
            }
        }

        return 0;
    }

    bool operator == (const Link& d) const{
       return (compare(d)==0);
    }

    bool operator < (const Link& d) const{
      return compare(d)<0;
    }

    bool operator > (const Link& d) const{
      return compare(d)>0;
    }
};

class PrimitiveNode
{
public:
    PrimitiveNode()
    {
        links = new QList<Link>();
    }
    unsigned int nodeId = 0;
    osg::ref_ptr<osg::Drawable> drawable;
    unsigned int primitiveIndex = 0;

    //Citygml related tagging
    QString name_space = OSGHELPERS::DEFAULT_STR();
    QString element_name = OSGHELPERS::DEFAULT_STR();

    osg::Vec3f* faceNormal;
    osg::Vec3f* centroid;

    QList<Link>* links;
};

class TriangleIndexes
{
public:
    unsigned int vertexId1;
    unsigned int vertexId2;
    unsigned int vertexId3;
};

class GraphData  : public osg::Referenced
{
public:
    std::map<unsigned int,PrimitiveNode>* primitivesMap;
    std::map<unsigned int,VertexLinks>* vertexNodeMap; //Links vertexes to primitive id's

    GraphData()
    {
        primitivesMap = new std::map<unsigned int,PrimitiveNode>();
        vertexNodeMap = new std::map<unsigned int,VertexLinks>();
    }
};

class TrianglePrimitivesConverter
{
public:
    TrianglePrimitivesConverter(){}

    bool verbose = false;
    bool printIndexes = false;
    virtual void apply(osg::Group* group)
    {
        unsigned int i;
        for (i = 0; i < group->getNumChildren(); i++)
        {
            osg::Geode* geode = (osg::Geode*)group->getChild(i);
            apply(geode);
        }
    }

    virtual void apply(osg::Geode* geode)
    {

        for(unsigned int i=0;i<geode->getNumDrawables();i++)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));
            geometry->setUseVertexBufferObjects(true);
            osgUtil::TriStripVisitor triStripVisitor;
            geometry->accept(triStripVisitor);
            triStripVisitor.stripify(*geometry);

            std::vector<TriangleIndexes> addPrimSetIndexes;

            for (unsigned int ipr=0; ipr<geometry->getNumPrimitiveSets(); ipr++)
            {
                osg::PrimitiveSet* prset=geometry->getPrimitiveSet(ipr);

                switch (prset->getMode())
                {
                case osg::PrimitiveSet::TRIANGLES:
                {
                    unsigned int ja;
                    for (ja=0; ja<=prset->getNumIndices()-3; )
                    {
                        TriangleIndexes ti;

                        ti.vertexId1 = prset->index(ja);
                        ti.vertexId2 = prset->index(ja+1);
                        ti.vertexId3 = prset->index(ja+2);

                        if(verbose && printIndexes)
                        {
                            osg::notify(osg::WARN) << ti.vertexId1 << ", "
                                                   << ti.vertexId2 << ", "
                                                   << ti.vertexId3 << std::endl;
                        }

                        addPrimSetIndexes.push_back(ti);
                        ja=ja+3;
                    }
                }
                    break;

                case osg::PrimitiveSet::TRIANGLE_STRIP:
                {
                    unsigned int ja;
                    for (ja=0; ja<prset->getNumIndices()-2; ja++)
                    {
                        TriangleIndexes ti;

                        if(ja % 2 == 0)
                        {
                            ti.vertexId1 = prset->index(ja);
                            ti.vertexId2 = prset->index(ja+1);
                            ti.vertexId3 = prset->index(ja+2);
                        }
                        else
                        {
                            ti.vertexId1 = prset->index(ja);
                            ti.vertexId2 = prset->index(ja+2);
                            ti.vertexId3 = prset->index(ja+1);
                        }

                        if(verbose && printIndexes)
                        {
                            osg::notify(osg::WARN) << ti.vertexId1 << ", "
                                                   << ti.vertexId2 << ", "
                                                   << ti.vertexId3 << std::endl;
                        }

                        addPrimSetIndexes.push_back(ti);
                    }
                }
                    break;

                    //TODO: Check and Handle other primitive types such as lines line loops etc
                    //that is not converted into triangle_strip by TriStripVisitor...
                }

            }


            //Free memory
            geometry->getPrimitiveSetList().clear();

            for (unsigned int prn=0; prn<addPrimSetIndexes.size(); prn++)
            {
                osg::DrawElementsUShort* primSet =
                        new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLES, 0);
                TriangleIndexes ti = addPrimSetIndexes[prn];
                primSet->push_back(ti.vertexId1);
                primSet->push_back(ti.vertexId2);
                primSet->push_back(ti.vertexId3);
                geometry->getPrimitiveSetList().push_back(primSet);
            }

            if(verbose)
            {
                osg::notify(osg::WARN) << "Number of primitives: "
                                       << geometry->getNumPrimitiveSets()
                                       << std::endl;

                osg::notify(osg::WARN) << "---- PRINTING PRIMITIVES AFTER CONVERSION ----" << std::endl;

                for (unsigned int ipr=0; ipr<geometry->getNumPrimitiveSets(); ipr++)
                {
                    osg::PrimitiveSet* prset=geometry->getPrimitiveSet(ipr);
                    OSGHELPERS::printPrimSet(prset);
                    osg::notify(osg::WARN) << std::endl;

                    if(prset->getMode()!=osg::PrimitiveSet::TRIANGLES)
                    {
                        qDebug() << "PRSET TYPE: " << prset->getMode() << ", NOT TRIANGLES" ;
                    }
                }
                osg::notify(osg::WARN) << "---- AFTER PRINTING PRIMITIVES ----" << std::endl;
            }

            //Generate New Normals
            osgUtil::SmoothingVisitor sv;
            geometry->accept(sv);
            sv.smooth(*geometry);
        }
    }

    void setVerbose(bool v){
        verbose = v;
    }
};

class GraphGenerator
{
public:
    unsigned int maxId=0;

    void generate(osg::Group* group)
    {
        generateGraphData(group);
        generateNormals(group);
        generateCentroids(group);
        generateVertexLinks(group);
        generateLinks(group);
    }

    void generateGraphData(osg::Group* group)
    {
        unsigned int k;
        for (k = 0; k < group->getNumChildren(); k++)
        {
            osg::Geode* geode = (osg::Geode*)group->getChild(k);
            unsigned int i;
            for(i=0; i < geode->getNumDrawables(); i++)
            {
                osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));

                GraphData* userData = new GraphData;
                for (unsigned int ipr=0; ipr<geometry->getNumPrimitiveSets(); ipr++)
                {
                    osg::PrimitiveSet* prset=geometry->getPrimitiveSet(ipr);

                    if(prset->getNumIndices()>=3)
                    {
                        PrimitiveNode primitiveNode;
                        primitiveNode.drawable = geometry->asDrawable();
                        primitiveNode.primitiveIndex = ipr;
                        primitiveNode.nodeId = maxId;
                        maxId++;

                        userData->primitivesMap->insert(
                                    std::pair<unsigned int,PrimitiveNode>(ipr,primitiveNode));
                    }

                }
                geometry->getOrCreateUserDataContainer()->setUserData(userData);
            }
        }

    }

    void generateNormals(osg::Group* group)
    {
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

                    osg::PrimitiveSet* prset=geometry->getPrimitiveSet(a.primitiveIndex);
                    osg::Vec3Array* verts= dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
                    if(prset->getNumIndices()>=3)
                    {
                        osg::Vec3f* normal =
                                calculateFaceNormal(&(* verts)[prset->index(0)],
                                &(* verts)[prset->index(1)],
                                &(* verts)[prset->index(2)]);
                        a.faceNormal = normal;
                        (*it).second = a;
                    }
                }
            }
        }
    }

    void generateCentroids(osg::Group* group)
    {
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

                    osg::PrimitiveSet* prset=geometry->getPrimitiveSet(a.primitiveIndex);
                    osg::Vec3Array* verts= dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
                    if(prset->getNumIndices()>=3)
                    {
                        QList<osg::Vec3f> list;
                        for(unsigned int idx=0; idx<prset->getNumIndices(); idx++)
                        {
                            list.push_back((* verts)[prset->index(idx)]);
                        }
                        osg::Vec3f* centroid =
                                calculatePolygonCentroid(list);
                        a.centroid = centroid;
                        (*it).second = a;
                    }
                }
            }
        }
    }

    void generateVertexLinks(osg::Group* group)
    {
        unsigned int k;
        for (k = 0; k < group->getNumChildren(); k++)
        {
            osg::Geode* geode = (osg::Geode*)group->getChild(k);
            unsigned int i;
            for(i=0; i < geode->getNumDrawables(); i++)
            {
                osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));

                GraphData* userData = dynamic_cast<GraphData*>(geometry->getUserData());
                std::map<unsigned int,VertexLinks>* vertexNodeMap = userData->vertexNodeMap;

                for(std::map<unsigned int,PrimitiveNode>::iterator it = userData->primitivesMap->begin();it!=userData->primitivesMap->end();it++)
                {
                    PrimitiveNode a =(*it).second;

                    osg::PrimitiveSet* prset=geometry->getPrimitiveSet(a.primitiveIndex);
                    for(unsigned int idx=0; idx<prset->getNumIndices(); idx++)
                    {
                        unsigned int vertexId = prset->index(idx);
                        VertexLinks vertexLinks = (*vertexNodeMap)[vertexId];
                        vertexLinks.primitiveIds->insert(a.primitiveIndex);

                        if(false)
                        {
                            qDebug() << "Vertex Id: " << QString::number(vertexId) << ", Linked Primitive Ids: ";
                            for(QSet<unsigned int>::iterator it =vertexLinks.primitiveIds->begin(); it != vertexLinks.primitiveIds->end(); it++)
                            {
                                qDebug() << QString::number(*it) << " | ";
                            }
                        }
                    }
                }
            }
        }
    }

    void generateLinks(osg::Group* group)
    {
        unsigned int k;
        for (k = 0; k < group->getNumChildren(); k++)
        {
            osg::Geode* geode = (osg::Geode*)group->getChild(k);
            unsigned int i;
            for(i=0; i < geode->getNumDrawables(); i++)
            {
                osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));

                GraphData* userData = dynamic_cast<GraphData*>(geometry->getUserData());
                std::map<unsigned int,VertexLinks>* vertexNodeMap = userData->vertexNodeMap;

                for(std::map<unsigned int,PrimitiveNode>::iterator it = userData->primitivesMap->begin();it!=userData->primitivesMap->end();it++)
                {
                    PrimitiveNode a =(*it).second;
                    osg::PrimitiveSet* prset=geometry->getPrimitiveSet(a.primitiveIndex);
                    for(unsigned int idx=0; idx<prset->getNumIndices(); idx++)
                    {
                        unsigned int vertexId = prset->index(idx);
                        const VertexLinks vertexLinks = vertexNodeMap->at(vertexId);
                        QSet<unsigned int> set = *(vertexLinks.primitiveIds);
                        for(QSet<unsigned int>::iterator it =set.begin(); it != set.end(); it++)
                        {
                            Link l;
                            l.drawable = geometry->asDrawable();
                            l.primitiveIndex = *it;
                            l.weight = 100;
                            if(!a.links->contains(l))
                                a.links->push_back(l);
                        }
                    }
                    (*it).second = a;
                }
            }
        }
    }

private:
    osg::Vec3f* calculateFaceNormal(osg::Vec3f* p1,osg::Vec3f* p2,osg::Vec3f* p3)
    {
        osg::Vec3f U;
        U.x() = p2->x() - p1->x();
        U.y() = p2->y() - p1->y();
        U.z() = p2->z() - p1->z();

        osg::Vec3f V;
        V.x() = p3->x() - p1->x();
        V.y() = p3->y() - p1->y();
        V.z() = p3->z() - p1->z();

        osg::Vec3f* normal = new osg::Vec3f;
        normal->x() = (U.y()*V.z()) - (U.z()*V.y());
        normal->y() = (U.z()*V.x()) - (U.x()*V.z());
        normal->z() = (U.x()*V.y()) - (U.y()*V.x());

        normal->normalize();

        return normal;
    }

    osg::Vec3f* calculatePolygonCentroid(QList<osg::Vec3f>& list)
    {

        osg::Vec3f* centroid = new osg::Vec3f;
        centroid->set(0.0f,0.0f,0.0f);

        for(unsigned int i=0; i<list.size();i++)
        {
            osg::Vec3f U = list.at(i);
            centroid->x() += U.x();
            centroid->y() += U.y();
            centroid->z() += U.z();
        }

        centroid->x() = centroid->x()/list.size();
        centroid->y() = centroid->y()/list.size();
        centroid->z() = centroid->z()/list.size();

        return centroid;
    }
};

class GlyphHelper
{
public:
    osg::Geometry* getNormalsGlyphGeometry(const QList<PrimitiveNode>& list)
    {
        osg::Geometry* linesGeom = new osg::Geometry();
        osg::Vec3Array* vertices = new osg::Vec3Array;

        unsigned int ipr = 0;
        unsigned int i = 0;
        for(ipr=0; ipr<list.size();ipr++)
        {
            const PrimitiveNode node = list.at(ipr);

            const osg::Vec3f begin = *(node.centroid);
            const osg::Vec3f end = *(node.centroid) + (*(node.faceNormal)/4);
            vertices->push_back(osg::Vec3f(begin.x(),begin.y(),begin.z()));
            vertices->push_back(osg::Vec3f(end.x(),end.y(),end.z()));
            linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,i,2));
            i=i+2;
        }

        // pass the created vertex array to the points geometry object.
        linesGeom->setVertexArray(vertices);

        // set the colors as before, plus using the aobve
        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back(osg::Vec4(1.0f,1.0f,0.0f,1.0f));
        linesGeom->setColorArray(colors);
        linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

        // set the normal in the same way color.
        osg::Vec3Array* normals = new osg::Vec3Array;
        normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
        linesGeom->setNormalArray(normals);
        linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

        return linesGeom;
    }

    osg::Geometry* getPolygonsGlyphGeometry(QList<PrimitiveNode>& list)
    {
        osg::Geometry* polyGeom = new osg::Geometry();
        osg::Vec3Array* vertices = new osg::Vec3Array;

        unsigned int ipr;
        unsigned int vertCount=0;
        for(ipr=0; ipr < list.size(); ipr++)
        {
            PrimitiveNode node = list.at(ipr);

            /*
            osg::Geometry* geometry = node.drawable->asGeometry();
            osg::PrimitiveSet* prset=geometry->getPrimitiveSet(node.primitiveIndex);
            osg::Vec3Array* verts= dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());


            for(unsigned int j=0; j <= prset->getNumIndices();j++)
            {
                unsigned int q = 0;
                if(j<prset->getNumInstances())
                {
                    q=j;
                }

                const osg::Vec3f vec = verts->at(prset->index(q));
             */

            vertices->push_back(*(node.centroid));
            vertCount++;
            //}
        }

        polyGeom->setVertexArray(vertices);
        polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,vertCount));

        // set the colors as before, plus using the aobve
        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
        polyGeom->setColorArray(colors);
        polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

        // set the normal in the same way color.
        osg::Vec3Array* normals = new osg::Vec3Array;
        normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
        polyGeom->setNormalArray(normals);
        polyGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

        return polyGeom;
    }

};

class EnhancedNode
{
public:
    PrimitiveNode theNode;
    osg::Vec3f theVertex;
    float dist = 0.0f;
};

class PrimitiveGraphEnhancer
{
public:
    PrimitiveGraphEnhancer(){}
    QMap<osg::Drawable, std::vector<int>>* container;

    void generateLinksFromNearbyVertices(std::vector<PrimitiveNode>& vector)
    {
        // 1.0 - Sort list by centroid X
        // 2.0 - Sort list by centroid Y
        // 3.0 - Sort list by centroid Z
        // Create link for 2 * numberOfVerticesInPolygon nearest centroids in each axis
        // If they share vertices or vertices are close, then keep link else delete link
        // Recursively check the links of links if they share vertices or the vertices are close
        // Add 50 weight to links which donot share a vertice but their vertices are very close
        // Edge weights can be used to calculate the distance from the selected polygon (Grab Cut)

        std::vector<PrimitiveNode> xSortedVector(vector);
        std::sort(xSortedVector.begin(),xSortedVector.end(),compareByCentroidsX);



    }

    static bool compareByCentroidsX(const PrimitiveNode & s1, const PrimitiveNode & s2)
    {
        return (s1.centroid->x() < s2.centroid->x());
    }

    void generateTestLinksFromNearbyVertices(osg::Group* group, const int precision)
    {
        // Calculate the Euclidean distance of each vertices of Primitive Nodes from origin and put them into bins
        // Compare all the vertices in the same bin... if the vertexes are close then polygon can be linked
        // Add -50 weight to links which donot share a vertice but their vertices are very close
        // Edge weights can be used to calculate the distance from the selected polygon (Geodesic Distance)

        std::multimap<float, EnhancedNode> euclideanVertexBuckets;
        QList<float> keyList;

        unsigned int k;
        for (k = 0; k < group->getNumChildren(); k++)
        {
            osg::Geode* geode = (osg::Geode*)group->getChild(k);
            unsigned int i;
            for(i=0; i < geode->getNumDrawables(); i++)
            {
                osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));

                GraphData* userData = dynamic_cast<GraphData*>(geometry->getUserData());
                osg::Vec3Array* verts= dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());

                for(std::map<unsigned int,PrimitiveNode>::iterator it = userData->primitivesMap->begin();it!=userData->primitivesMap->end();it++)
                {
                    PrimitiveNode a =(*it).second;

                    osg::PrimitiveSet* prset=geometry->getPrimitiveSet(a.primitiveIndex);

                    for(unsigned int idx=0; idx<prset->getNumIndices(); idx++)
                    {
                        osg::Vec3f currVert = (* verts)[prset->index(idx)];
                        EnhancedNode e_node;
                        e_node.theVertex = currVert;
                        e_node.theNode = a;
                        e_node.dist = calculateVertexDistanceFromOrigin(currVert);

                        std::string dist_str = MathHelper::to_string_with_precision(e_node.dist,precision);
                        float precision_bucket_key = QString::fromStdString(dist_str).toFloat();
                        euclideanVertexBuckets.insert(std::pair<float,EnhancedNode>(precision_bucket_key,e_node));

                        if(!keyList.contains(precision_bucket_key))
                            keyList.push_back(precision_bucket_key);
                    }
                }
            }
        }
        //qDebug() << "Number of Buckets: " << QString::number(keyList.size());

        float min_dist = 1.0f;
        if(precision>0)
            min_dist = 1.0f/((float)precision * 10);

        for(unsigned int ki=0; ki<keyList.size(); ki++)
        {
            float key = keyList.at(ki);
            //qDebug() << "In Bucket Id: " << ki << ", key: " << QString::number(key);

            std::pair<std::multimap<float,EnhancedNode>::iterator, std::multimap<float,EnhancedNode>::iterator> ret;
            ret = euclideanVertexBuckets.equal_range(key);

            std::pair<std::multimap<float,EnhancedNode>::iterator, std::multimap<float,EnhancedNode>::iterator> ret2;
            ret2 = euclideanVertexBuckets.equal_range(key);

            for (std::multimap<float,EnhancedNode>::iterator it=ret.first; it!=ret.second; ++it)
            {
                EnhancedNode a = it->second;

                for (std::multimap<float,EnhancedNode>::iterator it2=ret2.first; it2!=ret2.second; ++it2)
                {
                    EnhancedNode b = it2->second;

                    if(a.theNode.nodeId!=b.theNode.nodeId)
                    {
                        //qDebug() << " Comparing: " <<  QString::number(a.theNode.nodeId) << " and " <<  QString::number(b.theNode.nodeId);
                        float dist = std::fabs(a.dist - b.dist);
                        if(dist < min_dist)
                        {

                            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(a.theNode.drawable.get());
                            GraphData* userData = dynamic_cast<GraphData*>(geometry->getUserData());
                            std::map<unsigned int,PrimitiveNode>::iterator nt = userData->primitivesMap->find(a.theNode.primitiveIndex);
                            if (nt != userData->primitivesMap->end())
                            {
                                PrimitiveNode orig =(*nt).second;
                                Link l;
                                l.drawable = b.theNode.drawable;
                                l.primitiveIndex = b.theNode.primitiveIndex;
                                l.weight = 50;
                                if(!orig.links->contains(l))
                                    orig.links->push_back(l);
                                (*nt).second = orig;
                            }
                        }
                    }

                }
            }
        }
    }

private:

    float calculateVertexDistanceFromOrigin(osg::Vec3f& U)
    {
        return std::sqrt(std::pow(U.x(), 2) + std::pow( U.y(), 2) + std::pow(U.z(),2));
    }

    float calculateVertexDistance(osg::Vec3f& U, osg::Vec3f& V)
    {
        return std::sqrt( std::pow(U.x() - V.x(),2) +  std::pow(U.y() - V.y(),2) + std::pow(U.z() - V.z(),2));
    }
};

#endif // OSGHELPERS

