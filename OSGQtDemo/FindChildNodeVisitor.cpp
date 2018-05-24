#include "FindChildNodeVisitor.h"


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
