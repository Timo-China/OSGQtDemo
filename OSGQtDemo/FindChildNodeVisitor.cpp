#include "FindChildNodeVisitor.h"
#include <osgSim/DOFTransform>


FindChildNodeVisitor::FindChildNodeVisitor(const std::string& node_name)
    :osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
    m_strNodeName(node_name)
{

}


FindChildNodeVisitor::~FindChildNodeVisitor(void)
{
}

void FindChildNodeVisitor::apply(osg::Node& node)
{
    if (node.getName() == m_strNodeName)
    {
        m_FoundNodeList.push_back(&node);
    }
    traverse(node);
}

void FindChildNodeVisitor::apply(osg::Geode& geode)
{
    apply((osg::Node&)geode);
    traverse((osg::Node&)geode);
}

void FindChildNodeVisitor::apply(osg::Transform& node)
{
    osgSim::DOFTransform* dofNode = dynamic_cast<osgSim::DOFTransform*> (&node);
    if (dofNode)
    {
        dofNode->setAnimationOn(false);
    }
    apply((osg::Node&)node);
    traverse((osg::Node&)node);
}

void FindChildNodeVisitor::SetSearchName(const std::string& search_name)
{
    m_strNodeName = search_name;
    m_FoundNodeList.clear();
}

osg::Node* FindChildNodeVisitor::GetFirstNode()
{
    return (*m_FoundNodeList.begin());
}
