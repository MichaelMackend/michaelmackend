// Definition of the node
#pragma once
//(MEM) i added pragma once. see other pragma once comment.
template <class Type>
struct nodeType
{
	Type info;
	nodeType<Type> *link;
};

