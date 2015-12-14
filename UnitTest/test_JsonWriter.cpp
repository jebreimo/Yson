#include "../Yson/JsonWriter.hpp"

#include <sstream>
#include "../Externals/Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    static void test_SimpleObject()
    {
        std::stringstream ss;
        JsonWriter(ss)
            .writeBeginObject()
            .setValueName("name")
            .writeBeginObject()
            .writeEndObject()
            .writeEndObject();
        std::string expected = "\
{\n\
  \"name\": {}\n\
}";
        Y_EQUAL(ss.str(), expected);
    }

    Y_TEST(test_SimpleObject);
}
