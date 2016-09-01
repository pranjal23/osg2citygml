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

        if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        {

            if (m_xMouseCoordAtLastPress != ea.getX() || m_yMouseCoordAtLastPress != ea.getY())
            {
                return false;
            }

            return selectIntersectedPrimitives(ea, aa);

        }
        else if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        {
            m_xMouseCoordAtLastPress = ea.getX();
            m_yMouseCoordAtLastPress = ea.getY();
        }


        if(ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
        {
            switch(ea.getKey())
            {
            case 'c':
                clearSelectedList();
                return true;
                break;
            default:
                return false;
            }
        }

        return false;
    }

    void addToSelectedPrimitiveList(QList<PrimitiveNode>& prim_list)
    {
        for(int i=0; i<prim_list.size(); i++)
        {
            selectedPrimitives->insert(
                        std::pair<unsigned int,PrimitiveNode>(prim_list.at(i).primitiveIndex,prim_list.at(i)));
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
        else if(spatialNshape)
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
            msgBox.setText("Curved and Cylindrical Surfaces require spatial consideration, please check the spatial button.");
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
                    segmentCurvedSpatial(normal,node,nodesVisited,nodesSelected, false);
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
                    segmentCylindricalSpatial(normal,node,nodesVisited,nodesSelected, false);
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
        return ((W - U)*((V-U)^(Z - W)))==0;
    }

    bool isPerpendicular(const osg::Vec3f& U,const osg::Vec3f& V)
    {
        float dotp = (U*V);
        return dotp < 0.1;
    }

    bool isNotPerpendicularOrParallel(const osg::Vec3f& U,const osg::Vec3f& V)
    {
        float dotp = (U*V);
        return dotp > 0.1 || dotp < 0.9;
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

// class to handle events with a pick
class PickHandler : public osgGA::GUIEventHandler
{
public:

    PickHandler():
        _mx(0.0),_my(0.0),
        _usePolytopeIntersector(true),
        _useWindowCoordinates(true) {}

    ~PickHandler() {}

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
        osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
        if (!viewer) return false;

        switch(ea.getEventType())
        {
            case(osgGA::GUIEventAdapter::KEYUP):
            {
                if (ea.getKey()=='p')
                {
                    _usePolytopeIntersector = !_usePolytopeIntersector;
                    if (_usePolytopeIntersector)
                    {
                        osg::notify(osg::NOTICE)<<"Using PolytopeIntersector"<<std::endl;
                    } else {
                        osg::notify(osg::NOTICE)<<"Using LineSegmentIntersector"<<std::endl;
                    }
                }
                else if (ea.getKey()=='c')
                {
                    _useWindowCoordinates = !_useWindowCoordinates;
                    if (_useWindowCoordinates)
                    {
                        osg::notify(osg::NOTICE)<<"Using window coordinates for picking"<<std::endl;
                    } else {
                        osg::notify(osg::NOTICE)<<"Using projection coordiates for picking"<<std::endl;
                    }
                }
                return false;
            }
            case(osgGA::GUIEventAdapter::PUSH):
            case(osgGA::GUIEventAdapter::MOVE):
            {
                _mx = ea.getX();
                _my = ea.getY();
                return false;
            }
            case(osgGA::GUIEventAdapter::RELEASE):
            {
                if (_mx == ea.getX() && _my == ea.getY())
                {
                    // only do a pick if the mouse hasn't moved
                    pick(ea,viewer);
                }
                return true;
            }

            default:
                return false;
        }

        aa.requestRedraw();
    }

    void pick(const osgGA::GUIEventAdapter& ea, osgViewer::Viewer* viewer)
    {
        osg::Node* scene = viewer->getSceneData();
        if (!scene) return;

        osg::notify(osg::NOTICE)<<std::endl;

        osg::Node* node = 0;
        osg::Group* parent = 0;

        if (_usePolytopeIntersector)
        {
            osgUtil::PolytopeIntersector* picker;
            if (_useWindowCoordinates)
            {
                // use window coordinates
                // remap the mouse x,y into viewport coordinates.
                osg::Viewport* viewport = viewer->getCamera()->getViewport();
                double mx = viewport->x() + (int)((double )viewport->width()*(ea.getXnormalized()*0.5+0.5));
                double my = viewport->y() + (int)((double )viewport->height()*(ea.getYnormalized()*0.5+0.5));

                // half width, height.
                double w = 5.0f;
                double h = 5.0f;
                picker = new osgUtil::PolytopeIntersector( osgUtil::Intersector::WINDOW, mx-w, my-h, mx+w, my+h );
            } else {
                double mx = ea.getXnormalized();
                double my = ea.getYnormalized();
                double w = 0.05;
                double h = 0.05;
                picker = new osgUtil::PolytopeIntersector( osgUtil::Intersector::PROJECTION, mx-w, my-h, mx+w, my+h );
            }
            osgUtil::IntersectionVisitor iv(picker);

            viewer->getCamera()->accept(iv);

            if (picker->containsIntersections())
            {
                //osgUtil::PolytopeIntersector::Intersection intersection = picker->getFirstIntersection();

                osgUtil::PolytopeIntersector::Intersections intersections = picker->getIntersections();
                for(osgUtil::PolytopeIntersector::Intersections::iterator it = intersections.begin();
                    it != intersections.end(); ++it)
                {
                    const osgUtil::PolytopeIntersector::Intersection& intersection = *it;
                    qDebug() <<"  primitiveIndex "<<intersection.primitiveIndex;
                }
            }

        }
        else
        {
            osgUtil::LineSegmentIntersector* picker;
            if (!_useWindowCoordinates)
            {
                // use non dimensional coordinates - in projection/clip space
                picker = new osgUtil::LineSegmentIntersector( osgUtil::Intersector::PROJECTION, ea.getXnormalized(),ea.getYnormalized() );
            } else {
                // use window coordinates
                // remap the mouse x,y into viewport coordinates.
                osg::Viewport* viewport = viewer->getCamera()->getViewport();
                float mx = viewport->x() + (int)((float)viewport->width()*(ea.getXnormalized()*0.5f+0.5f));
                float my = viewport->y() + (int)((float)viewport->height()*(ea.getYnormalized()*0.5f+0.5f));
                picker = new osgUtil::LineSegmentIntersector( osgUtil::Intersector::WINDOW, mx, my );
            }
            osgUtil::IntersectionVisitor iv(picker);

            viewer->getCamera()->accept(iv);

            if (picker->containsIntersections())
            {
                osgUtil::LineSegmentIntersector::Intersection intersection = picker->getFirstIntersection();
               // osg::notify(osg::NOTICE)<<"Picked "<<intersection.localIntersectionPoint<<std::endl;

                osg::NodePath& nodePath = intersection.nodePath;
                node = (nodePath.size()>=1)?nodePath[nodePath.size()-1]:0;
                parent = (nodePath.size()>=2)?dynamic_cast<osg::Group*>(nodePath[nodePath.size()-2]):0;

            }
        }
    }


protected:

    float _mx,_my;
    bool _usePolytopeIntersector;
    bool _useWindowCoordinates;
};

#endif //SELECTIONHANDLER_H

