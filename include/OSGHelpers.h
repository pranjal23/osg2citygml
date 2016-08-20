#pragma once
#ifndef OSGHELPERS_H__
#define OSGHELPERS_H__

#include <QDebug>
#include <QException>

#include <osg/Group>
#include <osg/Vec3>
#include <osg/Referenced>

#include <osgUtil/Simplifier>
#include <osgUtil/TriStripVisitor>
#include <osgUtil/SmoothingVisitor>

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

class PrimitiveNode
{
public:
    unsigned int nodeId = 0;
    osg::ref_ptr<osg::Drawable> drawable;
    unsigned int primitiveIndex = 0;

    //Citygml related tagging
    QString name_space = OSGHELPERS::DEFAULT_STR();
    QString element_name = OSGHELPERS::DEFAULT_STR();

    osg::Vec3f* faceNormal;
    osg::Vec3f* centroid;
};

class Edge
{
public:
    osg::ref_ptr<osg::Drawable> toDrawable;
    unsigned int primitiveIndex = -1;

    /**
     * @brief Method from which weight is generated
     * 1. Maximum links to a PolygonNode is 2 times the number of vertices in the PolygonNode
     * 2.
     */
    int weight=0;
};

class TriangleIndexes
{
public:
    unsigned int vertexId1;
    unsigned int vertexId2;
    unsigned int vertexId3;
};

class UserData  : public osg::Referenced
{
public:
    std::map<unsigned int,PrimitiveNode>* primitivesMap;

    UserData()
    {
        primitivesMap = new std::map<unsigned int,PrimitiveNode>();
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
        //first convert to triangle strip
        osgUtil::TriStripVisitor triStripVisitor;
        geode->accept(triStripVisitor);

        for(unsigned int i=0;i<geode->getNumDrawables();i++)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));

            std::vector<TriangleIndexes> addPrimSetIndexes;

            for (unsigned int ipr=0; ipr<geometry->getNumPrimitiveSets(); ipr++)
            {
                osg::PrimitiveSet* prset=geometry->getPrimitiveSet(ipr);

                //osg::notify(osg::WARN) << "PRSET CLASS NAME: " << prset->getCompoundClassName();

                //osg::notify(osg::WARN) << "---- PRINTING BEFORE PARSING OF PRIMITIVES ----" << std::endl;
                // for each primitive analyze its type and work accordingly...
                switch (prset->getMode())
                {
                case osg::PrimitiveSet::TRIANGLES:
                {

                    //osg::notify(osg::WARN) << "In triangles - ";

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
                    //osg::notify(osg::WARN) << "In triangle strip - ";

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

            geometry->getPrimitiveSetList().clear();

            for (unsigned int prn=0; prn<addPrimSetIndexes.size(); prn++)
            {
                osg::DrawElementsUShort* primSet =
                        new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLES, 0);
                TriangleIndexes ti = addPrimSetIndexes.at(prn);
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
        }
    }

    void setVerbose(bool v){
        verbose = v;
    }
};

class MetaInformationGenerator
{
public:
    void generate(osg::Group* group)
    {
        generateUserData(group);
        generateNormals(group);
        generateCentroids(group);
    }

    void generateUserData(osg::Group* group)
    {
        //Generate the Normal for each primitive
        unsigned int k;
        for (k = 0; k < group->getNumChildren(); k++)
        {
            osg::Geode* geode = (osg::Geode*)group->getChild(k);
            unsigned int i;
            for(i=0; i < geode->getNumDrawables(); i++)
            {
                osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(i));

                UserData* userData = new UserData;
                for (unsigned int ipr=0; ipr<geometry->getNumPrimitiveSets(); ipr++)
                {

                    PrimitiveNode* primitiveNode = new PrimitiveNode();
                    primitiveNode->drawable = geometry->asDrawable();
                    primitiveNode->primitiveIndex = ipr;

                    userData->primitivesMap->insert(
                             std::pair<unsigned int,PrimitiveNode>(ipr,*primitiveNode));
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

                UserData* userData = dynamic_cast<UserData*>(geometry->getUserData());

                for(std::map<unsigned int,PrimitiveNode>::iterator it = userData->primitivesMap->begin();it!=userData->primitivesMap->end();it++)
                {
                    PrimitiveNode a =(*it).second;

                    osg::PrimitiveSet* prset=geometry->getPrimitiveSet(a.primitiveIndex);
                    osg::Vec3Array* verts= dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
                    if(prset->getNumIndices()>=3)
                    {
                        osg::Vec3f* normal = calculateFaceNormal(&(* verts)[prset->index(0)],
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

class PrimitiveGraphGenerator
{

};

#endif // OSGHELPERS

