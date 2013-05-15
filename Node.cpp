#include "Node.h"


Node::Node()
{
}


Node::Node(Node* Parent, const char* name)   
{
    this -> parent = Parent;
    this -> name = name;
    gameObject = nullptr;
}

void Node::Update()
{    
    if(!children.empty())
    {
        for(size_t i = 0; i < children.size(); ++i)
        {
            if(children[i] != nullptr)
            {         
                children[i] -> Update();

                if(children[i] -> getGameObject() != nullptr)
                children[i] -> getGameObject() -> changeObjectWorld(this->gameObject -> getCubeWorld());
            }
        }
    }
    
    if(gameObject != nullptr)
    gameObject -> update();
} 

GameObject *Node::getGameObject()
{
    return this -> gameObject;
}

Node* Node::GetChildNodeByName(const char *SearchName)
{
    Node* Retval = nullptr;

    if(!children.empty())
    {
        for(int i = 0; i < children.size(); ++i)
        {
            if(strcmp(children[i]->name, SearchName) == 0)
            {
                Retval = children[i];
                break;
            }

            Retval = children[i] -> GetChildNodeByName(SearchName);

            if(Retval != nullptr)
                return Retval;
        }
    }

    return(Retval);
};

void Node::setNodeGameObject(GameObject &object)
{
    gameObject = &object;
}

void Node::SetParentNode(Node* NewParent)
{
    parent = NewParent;
}

void Node::AddChildNode(Node* childNode)
{
    if(childNode != nullptr)
    {
        if(childNode -> GetParentNode() != nullptr)
        {
            childNode -> SetParentNode(this);
        }

        children.push_back(childNode);
    }
}

Node* Node::GetParentNode() const
{
    return(parent);
}

Node::~Node()
{
}
