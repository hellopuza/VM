#include "Compiler/AST/AST.h"

namespace ast {

AST::AST(std::shared_ptr<ASNode> value) : Tree(std::move(value)) {}

int AST::dot_dump(const char* dump_name) const
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

    dot_dump(dump_file);

    dump_file << "\tlabelloc=\"t\";"
                 "\tlabel=\""
              << dump_name << "\"; }\n";

    dump_file.close();

    const size_t max_line = 1024;

    char command[max_line] = "";
    sprintf(command, "dot -Tpng -o %s.png %s", dump_name, dot_file_name);
    return system(command);
}

void AST::dot_dump(std::ofstream& dump_file) const
{
    dump_file << "\t\"" << this << "\"[shape = box, style = filled, color = black, fillcolor = lightskyblue, label = \""
              << value_->print() << "\"]\n";

    for (const auto& node : branches_)
    {
        dump_file << "\t\"" << this << "\" -> \"" << &node << "\"\n";
    }

    for (const auto& node : branches_)
    {
        static_cast<const AST*>(&node)->dot_dump(dump_file);
    }
}

} // namespace ast