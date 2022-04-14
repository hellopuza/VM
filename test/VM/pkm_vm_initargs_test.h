#include "VM/PkmVMInitArgs.h"

#include <gtest/gtest.h> // NOLINT

TEST(PkmVMInitArgsTest, DefaultCtor) // NOLINT
{
    PkmVMInitArgs initargs;
}

class MainParser : public ::testing::Test
{
public:
    MainParser()
    {
        const char* file1 = "string_first.klass";
        const char* file2 = "string_second.klass";
        const char* app = "MyApp";
        const char* key = "-p";
        int arg_n_ = 4;

        const char** arg_s_ = new const char* [arg_n_];
        arg_s_[0] = app;
        arg_s_[1] = file1;
        arg_s_[2] = file2;
        arg_s_[3] = key;
    };
    ~MainParser()
    {
        delete [] arg_s_;
    };
    const char** arg_s_;
    int arg_n_;
};

TEST_F(MainParser, ParseFilesNum)
{
    int result = parseFilesNum(arg_n_, arg_s_);

    EXPECT_EQ(arg_n_, result);
}