#ifndef COMMON_TREE_TREE_H
#define COMMON_TREE_TREE_H

#include <vector>

template<typename T>
class Tree
{
public:
    Tree() = default;
    explicit Tree(const T& value);
    Tree(const Tree& obj);
    Tree(Tree&& obj) noexcept;
    ~Tree() = default;

    Tree& operator=(const Tree& obj);
    Tree& operator=(Tree&& obj) noexcept;
    Tree& operator[](size_t branch_ind);

    size_t size() const;
    size_t branches_num() const;
    void clear_branches();

    void push_branch(const Tree& tree);
    void pop_branch();

    T& value();
    const T& value() const;

    int dot_dump(const char* dump_name) const;

private:
    void dot_dump(std::ofstream& dump_file) const;

    std::vector<Tree<T>> branches_;
    T value_;
};

#endif // COMMON_TREE_TREE_H