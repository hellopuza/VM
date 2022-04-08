#ifndef COMMON_TREE_TREE_IMPL_H
#define COMMON_TREE_TREE_IMPL_H

#include "Common/Tree/Tree.h"

#include <fstream>

template<typename TYPE>
Tree<TYPE>::Tree(const TYPE& value_) : value(value_) {}

template<typename TYPE>
Tree<TYPE>::Tree(const Tree& obj) : value(obj.value), std::vector<Tree<TYPE>>(obj) {}

template<typename TYPE>
Tree<TYPE>::Tree(Tree&& obj) noexcept : value(std::move(obj.value)), std::vector<Tree<TYPE>>(std::move(obj)) {}

template<typename TYPE>
Tree<TYPE>& Tree<TYPE>::operator=(const Tree& obj)
{
    value = obj.value;
    std::vector<Tree<TYPE>>::operator=(obj);
    return *this;
}

template<typename TYPE>
Tree<TYPE>& Tree<TYPE>::operator=(Tree&& obj) noexcept
{
    value = std::move(obj.value);
    std::vector<Tree<TYPE>>::operator=(std::move(obj));
    return *this;
}

template<typename TYPE>
int Tree<TYPE>::dump(const char* dump_name) const
{
    const char* dot_file_name = "graph.dot";

    std::ofstream dump_file(dot_file_name);
    if (!dump_file.is_open())
    {
        return -1;
    }

    dump_file << "digraph G{\n"
                 " rankdir = HR;\n"
                 " node[shape=box];\n";

    dump(dump_file);

    dump_file << "\tlabelloc=\"t\";"
                 "\tlabel=\""
              << dump_name << "\"; }\n";

    dump_file.close();

    char command[1024] = "";
    sprintf(command, "dot -Tpng -o %s.png %s", dump_name, dot_file_name);
    return system(command);
}

template<typename TYPE>
void Tree<TYPE>::dump(std::ofstream& dump_file) const
{
    dump_file << "\t\"" << this << "\"[shape = box, style = filled, color = black, fillcolor = lightskyblue, label = \""
              << value << "\"]\n";

    for (const auto& node : *this)
    {
        dump_file << "\t\"" << this << "\" -> \"" << &node << "\"\n";
    }

    for (const auto& node : *this)
    {
        node.dump(dump_file);
    }
}

#endif // COMMON_TREE_TREE_IMPL_H