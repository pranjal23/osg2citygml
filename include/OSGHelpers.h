#ifndef OSGHELPERS
#define OSGHELPERS

#include <QDebug>

#include <osg/Group>
#include <osg/Vec3>

#include <osgUtil/Simplifier>
#include <osgUtil/TriStripVisitor>

void printPrimSet(osg::PrimitiveSet* prset)
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

void printVertexArray(unsigned int vertexId, const osg::Vec3Array *verts)
{
    osg::notify(osg::WARN) << "Vertex Id: "<< vertexId << ", coordinates: " <<
                                  (* verts)[vertexId].x() << "," <<
                                  (* verts)[vertexId].y() << "," <<
                                  (* verts)[vertexId].z() << std::endl;
}

void printVertex(const osg::Vec3 vert)
{
    osg::notify(osg::WARN) <<
                                  vert.x() << "," <<
                                  vert.y() << "," <<
                                  vert.z() << std::endl;
}

void printPrimSets(osg::Geode& geode){
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

class TriangleIndexes
{
public:
    unsigned int vertexId1;
    unsigned int vertexId2;
    unsigned int vertexId3;
};

class ConvertToTrianglePrimitives
{
public:

    ConvertToTrianglePrimitives(){}

    bool verbose = false;
    bool printIndexes = false;
    virtual void apply(osg::Geode* geode)
    {
        /*
        //convert to triangle strip
        osgUtil::TriStripVisitor triStripVisitor;
        triStripVisitor.apply(*geode);
        triStripVisitor.stripify();
        */

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

            //osg::notify(osg::WARN) << "---- PRINTING BEFORE REMOVAL OF PRIMITIVES ----" << std::endl;
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

            osg::notify(osg::WARN) << "Number of primitives: "
                                   << geometry->getNumPrimitiveSets()
                                   << std::endl;


            if(verbose)
            {
                osg::notify(osg::WARN) << "---- PRINTING PRIMITIVES AFTER CONVERSION ----" << std::endl;

                for (unsigned int ipr=0; ipr<geometry->getNumPrimitiveSets(); ipr++)
                {
                    osg::PrimitiveSet* prset=geometry->getPrimitiveSet(ipr);
                    printPrimSet(prset);
                    osg::notify(osg::WARN) << std::endl;

                    if(prset->getMode()!=osg::PrimitiveSet::TRIANGLES)
                    {
                        qDebug() << "PRSET TYPE: " << prset->getMode() << ", NOT TRIANGLES" ;
                    }
                }
                osg::notify(osg::WARN) << "---- AFTER PRINTING PRIMITIVES ----" << std::endl;
            }

        }

        /*
        //Simplify before returning...
        osgUtil::Simplifier simplifier;
        simplifier.apply(*geode);
        */

    }

    void setVerbose(bool v){
        verbose = v;
    }

};


/*
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
*/

#endif // OSGHELPERS

