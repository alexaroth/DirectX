#pragma once
#include "GameObject.h"

class Node
{
public:
    Node();
    Node(Node* Parent = nullptr, const char* Name = nullptr);
    virtual ~Node(); 
    virtual void Update(); 
    Node* GetParentNode() const;
    void SetParentNode(Node* NewParent); 
    void setNodeGameObject(GameObject &object);
    void AddChildNode(Node* ChildNode);
    void RemoveChildNode(Node* ChildNode); 
    GameObject *getGameObject();
    Node* GetChildNodeByName(const char* SearchName);

private:

    Node* parent;
    GameObject *gameObject;
    const char* name;
    vector<Node*> children;
};

