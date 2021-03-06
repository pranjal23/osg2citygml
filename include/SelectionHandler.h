#ifndef SELECTIONHANDLER_H
#define SELECTIONHANDLER_H

#include <QDebug>
#include <QMessageBox>

#include <osg/Group>
#include <osg/Vec3>

#include <osgViewer/Viewer>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osg/Node>
#include <osg/Notify>

#include <osgGA/SphericalManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgViewer/ViewerEventHandlers>

#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/Optimizer>
#include <osgUtil/IntersectionVisitor>

#include <iostream>
#include <map>
#include <utility>

#include "GeometryHandler.h"
#include "OSGWidget.h"

class AddEditColoursToGeometryVisitor : public osg::NodeVisitor
{
public:
    AddEditColoursToGeometryVisitor()
        :osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    {}

    void apply(osg::Geode& geode,
               std::multimap<unsigned int,PrimitiveNode>& selectedPrimitives)
    {
        for(unsigned int i=0;i<geode.getNumDrawables();i++)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
            if (geometry && geometry->getNumPrimitiveSets()>0)
            {
                //Remove Material and Colormark
                geometry->getOrCreateStateSet()->removeAttribute(osg::StateAttribute::MATERIAL);
                geometry->getOrCreateStateSet()->removeAttribute(osg::StateAttribute::COLORMASK);

                //Set all the vertex colors to editable color
                osg::Vec4Array* colours =
                        new osg::Vec4Array(geometry->getNumPrimitiveSets());
                int x = 0;
                osg::Vec4* col = getNewColor(x);
                for(unsigned int ci=0;ci<geometry->getNumPrimitiveSets();ci++)
                {
                    (*colours)[ci].set(col->x(),col->y(),col->z(),col->w());
                }

                if(selectedPrimitives.size()>0)
                {
                    for(std::multimap<unsigned int,PrimitiveNode>::iterator it = selectedPrimitives.begin();it!=selectedPrimitives.end();it++)
                    {
                        PrimitiveNode a =(*it).second;
                        if(a.drawable.get() == geometry->asDrawable())
                        {
                            x = 1;
                            col = getNewColor(x);
                            (*colours)[a.primitiveIndex].set(col->x(),col->y(),col->z(),col->w());
                        }
                    }
                }
                geometry->setColorArray(colours, osg::Array::BIND_PER_PRIMITIVE_SET);
            }
        }
    }

    virtual void apply(osg::Node& node) { traverse(node); }

private:
    osg::Vec4* getNewColor(int x){
        if(x == 0)
            return new osg::Vec4(1.0f,0.5f,0.5f,1.0f);
        else
            return new osg::Vec4(0.5f,1.0f,1.0f,1.0f);
    }

};

class SelectionHandler : public osgGA::GUIEventHandler {
public:
    OSGWidget* osgwidget;
    bool _controlKeyPressed = false;
    std::multimap<unsigned int,PrimitiveNode>* selectedPrimitives;

    AddEditColoursToGeometryVisitor* colorVistor = new AddEditColoursToGeometryVisitor();

    SelectionHandler(OSGWidget* widget)
    {
        osgwidget = widget;
        m_xMouseCoordAtLastPress = -1;
        m_yMouseCoordAtLastPress = -1;
        selectedPrimitives = new std::multimap<unsigned int,PrimitiveNode>();
        colorify();
    }

    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
    {
        if(!osgwidget->renderEditableMode)
            return false;


        if ((ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON))
        {

            if (m_xMouseCoordAtLastPress != ea.getX() || m_yMouseCoordAtLastPress != ea.getY())
            {
                return false;
            }

            bool ret = selectIntersectedPrimitives(ea, aa);
            aa.requestRedraw();
            return ret;

        }
        else
        {
            if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
            {
                m_xMouseCoordAtLastPress = ea.getX();
                m_yMouseCoordAtLastPress = ea.getY();
            }
        }

        if ( osgwidget->paintSelectionMode && ea.getEventType() == osgGA::GUIEventAdapter::DRAG)
        {
            bool ret = selectIntersectedPrimitives(ea, aa);
            aa.requestRedraw();
            return ret;
        }

        if(ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
        {
            switch(ea.getKey())
            {
            case 'c':
                clearSelectedList();
                aa.requestRedraw();
                return true;
                break;
            default:
                return false;
            }
        }

        return false;
    }

    void addToSelectedPrimitiveList(QList<PrimitiveNode>& prim_list, bool deselect)
    {
        if(!deselect)
        {
            for(int i=0; i<prim_list.size(); i++)
            {
                selectedPrimitives->insert(
                            std::pair<unsigned int,PrimitiveNode>(prim_list.at(i).primitiveIndex,prim_list.at(i)));
            }
        }
        else
        {
            for(int i=0; i<prim_list.size(); i++)
            {
                std::multimap<unsigned int,PrimitiveNode>::iterator itS;
                bool selected = false;
                std::pair <std::multimap<unsigned int,PrimitiveNode>::iterator, std::multimap<unsigned int,PrimitiveNode>::iterator> ret;
                ret = selectedPrimitives->equal_range(prim_list.at(i).primitiveIndex);
                for (std::multimap<unsigned int,PrimitiveNode>::iterator it=ret.first; it!=ret.second; ++it)
                {
                    PrimitiveNode a = it->second;
                    if(a.drawable == prim_list.at(i).drawable)
                    {
                        itS = it;
                        selected = true;
                        break;
                    }
                }

                if(selected)
                {
                    selectedPrimitives->erase(itS);
                }
            }
        }
    }

    void clearSelectedList()
    {
        selectedPrimitives->clear();
        colorify();
    }

    void colorify()
    {
        if(osgwidget->getEditableModelGroup() == nullptr)
            return;

        int i;
        for (i = 0; i < osgwidget->getEditableModelGroup().get()->getNumChildren(); i++)
        {
            osg::Geode* geode = (osg::Geode*)osgwidget->getEditableModelGroup().get()->getChild(i);
            colorVistor->apply(*geode,*selectedPrimitives);
        }

    }

private:

    bool compareNormals(osg::Vec3f m, osg::Vec3f n)
    {
        double weight = osgwidget->getNormalsDistance();
        if(std::abs(m.x() - n.x()) <= weight
                && std::abs(m.y() - n.y()) <= weight
                && std::abs(m.z() - n.z()) <= weight)
            return true;

        return false;
    }

    void selectBySegmentation(PrimitiveNode& stp)
    {
        bool normals = osgwidget->getNormalsBasedSegmentation();
        bool spatial = osgwidget->getLocationBasedSegmentation();
        bool shapes = osgwidget->getShapeBasedSegmentation();

        bool normalsOnly = normals && !spatial && !shapes;
        bool spatialOnly = !normals && spatial && !shapes;
        bool shapeOnly = !normals && !spatial && shapes;

        bool normalsNspatial = normals && spatial && !shapes;
        bool spatialNshape = !normals && spatial && shapes;

        bool normalsNspatialNshape = normals && spatial && shapes;

        if(normalsOnly)
        {
            segmentByNormalsOnly(stp);
        }
        else if(spatialOnly)
        {
            segmentBySpatialLocationOnly(stp);
        }
        else if(shapeOnly)
        {
            segmentByShapeOnly(stp);
        }
        else if(normalsNspatial)
        {
            segmentByNormalsAndLocation(stp);
        }
        else if(spatialNshape || normalsNspatialNshape)
        {
            segmentBySpatialNShape(stp);
        }
    }

    void segmentByNormalsOnly(PrimitiveNode& stp)
    {
        osg::Vec3f selectedNormal = *(stp.faceNormal);

        QList<PrimitiveNode> list = osgwidget->getAllPolygonNodes();
        for (unsigned int i = 0; i < list.size(); i++)
        {
            PrimitiveNode node = list[i];
            osg::Vec3f currNormal = *(node.faceNormal);

            if(compareNormals(selectedNormal,currNormal))
            {
                selectPrimitive(node);
            }
        }
    }

    void segmentBySpatialLocationOnly(PrimitiveNode& stp)
    {
        QList<unsigned int>* nodesVisited = new QList<unsigned int>();
        segmentBySpatialLocationOnly(stp,nodesVisited);

        nodesVisited->clear();
    }

    void segmentBySpatialLocationOnly(PrimitiveNode &stp, QList<unsigned int>* nodesVisited)
    {
        selectPrimitive(stp);
        if(false)
        {
            qDebug() << "Node selected: " << QString::number(stp.nodeId)
                     << ", number of links: " << QString::number(stp.links->size());
        }
        nodesVisited->push_back(stp.nodeId);
        for(unsigned int i=0; i < stp.links->size(); i++)
        {
            PrimitiveNode node = osgwidget->getPolygonNode(stp.links->at(i).drawable,stp.links->at(i).primitiveIndex);
            if(!nodesVisited->contains(node.nodeId))
            {
                segmentBySpatialLocationOnly(node, nodesVisited);
            }
            else if(false)
            {
               qDebug() << "Iteration: " << QString::number(i) << ", Node: " << QString::number(node.nodeId)
                        << " not visited" ;
            }
        }
    }

    void segmentByNormalsAndLocation(PrimitiveNode& stp)
    {
        QList<unsigned int>* nodesVisited = new QList<unsigned int>();
        QList<PrimitiveNode>* nodesSelected = new QList<PrimitiveNode>();
         osg::Vec3f selectedNormal = *(stp.faceNormal);

        segmentByNormalsAndLocation(stp,selectedNormal,nodesVisited,nodesSelected);

        QList<PrimitiveNode> list = (*nodesSelected);
        for (unsigned int i = 0; i < list.size(); i++)
        {
            selectPrimitive(list[i]);
        }

        nodesSelected->clear();
        nodesVisited->clear();
    }

    void segmentByNormalsAndLocation(PrimitiveNode &stp,osg::Vec3f& selectedNormal, QList<unsigned int>* nodesVisited, QList<PrimitiveNode>* nodesSelected)
    {
        osg::Vec3f currNormal = *(stp.faceNormal);

        nodesVisited->push_back(stp.nodeId);
        if(compareNormals(selectedNormal,currNormal))
        {
            nodesSelected->push_back(stp);

            for(unsigned int i=0; i < stp.links->size(); i++)
            {
                PrimitiveNode node = osgwidget->getPolygonNode(stp.links->at(i).drawable,stp.links->at(i).primitiveIndex);
                if(!nodesVisited->contains(node.nodeId))
                {
                    segmentByNormalsAndLocation(node, selectedNormal, nodesVisited, nodesSelected);
                }
            }
        }
    }

    void segmentByShapeOnly(PrimitiveNode& stp)
    {
        if(osgwidget->shape_to_segment==Shape::Plain)
        {
            osg::PrimitiveSet* prset = stp.drawable->asGeometry()->getPrimitiveSet(stp.primitiveIndex);
            osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(stp.drawable->asGeometry()->getVertexArray());
            if(prset->getNumIndices()>=3)
            {
                osg::Vec3f& U = (* verts)[prset->index(0)];
                osg::Vec3f& V = (* verts)[prset->index(1)];
                osg::Vec3f& W = (* verts)[prset->index(2)];

                QList<PrimitiveNode> list = osgwidget->getAllPolygonNodes();
                for (unsigned int i = 0; i < list.size(); i++)
                {
                    PrimitiveNode node = list[i];

                    if(isCoplanar(U,V,W,*(node.centroid)))
                    {
                        selectPrimitive(node);
                    }
                }
            }
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Wall, Curved and Cylindrical Surfaces require spatial consideration, please check the spatial button.");
            msgBox.exec();
        }
    }

    void segmentBySpatialNShape(PrimitiveNode& stp)
    {
        QList<unsigned int>* nodesVisited = new QList<unsigned int>();
        QList<PrimitiveNode>* nodesSelected = new QList<PrimitiveNode>();

        segmentBySpatialNShape(stp,nodesVisited,nodesSelected);

        QList<PrimitiveNode> list = (*nodesSelected);
        for (unsigned int i = 0; i < list.size(); i++)
        {
            selectPrimitive(list[i]);
        }

        nodesSelected->clear();
        nodesVisited->clear();
    }

    void segmentBySpatialNShape(PrimitiveNode &stp, QList<unsigned int>* nodesVisited, QList<PrimitiveNode>* nodesSelected)
    {
        if(osgwidget->shape_to_segment==Shape::Plain)
        {
            osg::PrimitiveSet* prset = stp.drawable->asGeometry()->getPrimitiveSet(stp.primitiveIndex);
            osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(stp.drawable->asGeometry()->getVertexArray());
            if(prset->getNumIndices()>=3)
            {
                osg::Vec3f& U = (* verts)[prset->index(0)];
                osg::Vec3f& V = (* verts)[prset->index(1)];
                osg::Vec3f& W = (* verts)[prset->index(2)];
                segmentPlainSpatial(U,V,W,stp,nodesVisited,nodesSelected, true);
            }
        }
        else if(osgwidget->shape_to_segment==Shape::Curved)
        {
            segmentCurvedSpatial((*stp.faceNormal),stp,nodesVisited,nodesSelected, true);
        }
        else if(osgwidget->shape_to_segment==Shape::Cylindrical)
        {
            segmentCylindricalSpatial((*stp.faceNormal),stp,nodesVisited,nodesSelected, true);
        }
        else if(osgwidget->shape_to_segment==Shape::Box)
        {
            osg::Vec3f up = *osgwidget->up_vector;
            segmentBoxSpatial((*stp.faceNormal),stp,up,nodesVisited,nodesSelected, true);
        }
    }

    void segmentPlainSpatial(osg::Vec3f& U, osg::Vec3f& V, osg::Vec3f& W, PrimitiveNode &stp, QList<unsigned int>* nodesVisited, QList<PrimitiveNode>* nodesSelected, bool root)
    {
        nodesVisited->push_back(stp.nodeId);
        if(isCoplanar(U,V,W,*(stp.centroid)) || root)
        {
            nodesSelected->push_back(stp);

            for(unsigned int i=0; i < stp.links->size(); i++)
            {
                PrimitiveNode node = osgwidget->getPolygonNode(stp.links->at(i).drawable,stp.links->at(i).primitiveIndex);
                if(!nodesVisited->contains(node.nodeId))
                {
                    segmentPlainSpatial(U,V,W,node,nodesVisited,nodesSelected, false);
                }
            }
        }
    }

    void segmentCurvedSpatial(osg::Vec3f& normal, PrimitiveNode &stp, QList<unsigned int>* nodesVisited, QList<PrimitiveNode>* nodesSelected, bool root)
    {
        nodesVisited->push_back(stp.nodeId);
        if(!isPerpendicular(normal,*(stp.faceNormal)) || root)
        {
            nodesSelected->push_back(stp);

            for(unsigned int i=0; i < stp.links->size(); i++)
            {
                PrimitiveNode node = osgwidget->getPolygonNode(stp.links->at(i).drawable,stp.links->at(i).primitiveIndex);
                if(!nodesVisited->contains(node.nodeId))
                {
                    segmentCurvedSpatial(*(stp.faceNormal),node,nodesVisited,nodesSelected, false);
                }
            }
        }
    }

    void segmentCylindricalSpatial(osg::Vec3f& normal, PrimitiveNode &stp, QList<unsigned int>* nodesVisited, QList<PrimitiveNode>* nodesSelected, bool root)
    {
        nodesVisited->push_back(stp.nodeId);
        if(isNotPerpendicularOrParallel(normal,*(stp.faceNormal)) || root)
        {
            nodesSelected->push_back(stp);

            for(unsigned int i=0; i < stp.links->size(); i++)
            {
                PrimitiveNode node = osgwidget->getPolygonNode(stp.links->at(i).drawable,stp.links->at(i).primitiveIndex);
                if(!nodesVisited->contains(node.nodeId))
                {
                    segmentCylindricalSpatial(*(stp.faceNormal),node,nodesVisited,nodesSelected, false);
                }
            }
        }
    }

    void segmentBoxSpatial(osg::Vec3f& normal, PrimitiveNode &stp, osg::Vec3f& up, QList<unsigned int>* nodesVisited, QList<PrimitiveNode>* nodesSelected, bool root)
    {
        nodesVisited->push_back(stp.nodeId);
        if((isNotParallel(up,*(stp.faceNormal)) && isBoxSurface(normal,*(stp.faceNormal))))
        {
            nodesSelected->push_back(stp);

            for(unsigned int i=0; i < stp.links->size(); i++)
            {
                PrimitiveNode node = osgwidget->getPolygonNode(stp.links->at(i).drawable,stp.links->at(i).primitiveIndex);
                if(!nodesVisited->contains(node.nodeId))
                {
                    segmentBoxSpatial(*(stp.faceNormal),node,up,nodesVisited,nodesSelected, false);
                }
            }
        }
    }

    /**
     * @brief: (W - U)*[(V-U)^(Z - W)]=0
     * Check if four points are coplanar
     */
    bool isCoplanar(const osg::Vec3f& U,const osg::Vec3f& V,const osg::Vec3f& W,const osg::Vec3f& Z)
    {
        double scalartp = ((W - U)*((V-U)^(Z - W)));
        if (scalartp==0)
            return true;

        double weight = osgwidget->getNormalsDistance();
        return weigthedCoplanar(U,V,W,Z,scalartp,weight);

    }

    bool weigthedCoplanar(const osg::Vec3f& U,const osg::Vec3f& V,const osg::Vec3f& W,const osg::Vec3f& Z,double scalartp, double weight)
    {
        if(weight<0)
            return (std::abs(scalartp)>weight && std::abs(scalartp)<0);
        else
            return (std::abs(scalartp)<weight && std::abs(scalartp)>0);
    }

    bool isPerpendicular(const osg::Vec3f& U,const osg::Vec3f& V)
    {
        double weight = osgwidget->getNormalsDistance();
        float dotp = (U*V);
        return dotp < weight;
    }

    bool isNotPerpendicularOrParallel(const osg::Vec3f& U,const osg::Vec3f& V)
    {
        double weight = osgwidget->getNormalsDistance();
        float dotp = (U*V);
        return dotp > weight || dotp < (1.0f - weight);
    }

    bool isBoxSurface(const osg::Vec3f& U,const osg::Vec3f& V)
    {
        double weight = osgwidget->getNormalsDistance();
        float dotp = fabs(U*V);
        //qDebug() << "U.X:" << U.x() << "," << "U.Y:" << U.y() << "," << "U.Z:" << U.z();
        //qDebug() << "V.X:" << V.x() << "," << "V.Y:" << V.y() << "," << "V.Z:" << V.z();

        //qDebug() << "dotp: " << dotp;
        bool boxSurface = dotp >= (1-weight) || dotp <= (0+weight);
        //qDebug() << "Box Surface:" << boxSurface;
        return boxSurface;
    }

    bool isNotParallel(const osg::Vec3f& U,const osg::Vec3f& V)
    {
        double weight = osgwidget->getNormalsDistance();
        float dotp = fabs(U*V);

        if(weight <= 0)
            return dotp < (1.00 + weight);
        else
            return dotp < (1.00 - weight);
    }

    bool selectIntersectedPrimitives(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {

        osgUtil::LineSegmentIntersector::Intersections intersections;
        if (!aa.computeIntersections(ea, intersections)) {
            return false;
        }

        osgUtil::LineSegmentIntersector::Intersection firstIntersection = *intersections.begin();


        if (firstIntersection.drawable == nullptr) {
            return false;
        }


        PrimitiveNode stp = osgwidget->getPolygonNode(firstIntersection.drawable,firstIntersection.primitiveIndex);
        if(osgwidget->getNormalsBasedSegmentation()
                || osgwidget->getLocationBasedSegmentation()
                || osgwidget->getShapeBasedSegmentation())
        {
            selectBySegmentation(stp);
        }
        else
        {
            selectPrimitive(stp);
        }

        colorify();

        return true;
    }

    void selectPrimitive(PrimitiveNode& stp)
    {
        std::multimap<unsigned int,PrimitiveNode>::iterator itS;
        if(osgwidget->getEditableModelGroup() != nullptr)
        {
            if(!osgwidget->selectMode)
            {
                bool selected = false;
                std::pair <std::multimap<unsigned int,PrimitiveNode>::iterator, std::multimap<unsigned int,PrimitiveNode>::iterator> ret;
                ret = selectedPrimitives->equal_range(stp.primitiveIndex);
                for (std::multimap<unsigned int,PrimitiveNode>::iterator it=ret.first; it!=ret.second; ++it)
                {
                    PrimitiveNode a = it->second;
                    if(a.drawable == stp.drawable)
                    {
                        itS = it;
                        selected = true;
                        break;
                    }
                }

                if(selected)
                {
                    selectedPrimitives->erase(itS);
                }
            }

            //Insert into selected primitives list if newly selected
            if(osgwidget->selectMode)
            {
                selectedPrimitives->insert(std::pair<unsigned int,PrimitiveNode>(stp.primitiveIndex,stp));
            }

        }
    }

    int m_xMouseCoordAtLastPress;
    int m_yMouseCoordAtLastPress;
};

#endif //SELECTIONHANDLER_H

