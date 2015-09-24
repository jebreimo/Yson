#include "JEBIO/Json/JsonWriter.hpp"

#include <sstream>
#include <JEBTest/JEBTest.hpp>

using namespace JEBIO::Json;

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
    JT_EQUAL(ss.str(), expected);
}

JT_TEST(test_SimpleObject);
