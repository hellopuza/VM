#ifndef COMMON_TREE_TREE_H
#define COMMON_TREE_TREE_H

#include <vector>

template<typename TYPE>
struct Tree : std::vector<Tree<TYPE>>
{
    TYPE value;

    Tree() = default;
    explicit Tree(const TYPE& value_);
    Tree(const Tree& obj);
    Tree(Tree&& obj) noexcept;
    ~Tree() = default;

    Tree& operator=(const Tree& obj);
    Tree& operator=(Tree&& obj) noexcept;

    int dump(const char* dump_name) const;

private:
    void dump(std::ofstream& dump_file) const;
};

#endif // COMMON_TREE_TREE_H