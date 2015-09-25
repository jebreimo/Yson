#include "../Yson/JsonWriter.hpp"

#include <sstream>
#include "../Externals/Ytest/Ytest.hpp"

namespace
{
    using namespace Yson;

    static void test_SimpleObject()
    {
        std::stringstream ss;
        JsonWriter writer(ss);
        writer.beginObject();
        writer.setValueName("name");
        writer.beginObject();
        writer.endObject();
        writer.endObject();
        std::string expected = "\
{\n\
  \"name\": {}\n\
}";
        Y_EQUAL(ss.str(), expected);
    }

    Y_TEST(test_SimpleObject);
}
