#include "JEBIO/Json/JsonValue.hpp"

#include <JEBTest/JEBTest.hpp>

using namespace JEBIO::Json;

class ExpectedValues
{
public:
    enum ValueType
    {
        String = 1,
        Boolean = 2,
        Real = 4,
        Integer = 8
    };
    ExpectedValues() : m_Flags(0) {}
    bool hasValue(ValueType vt) const {return (m_Flags & vt) != 0;}
    const std::string& string() const {return m_String;}
    void setString(const std::string& s) {m_Flags |= String; m_String = s;}
    bool boolean() const {return m_Boolean;}
    void setBoolean(bool b) {m_Flags |= Boolean; m_Boolean = b;}
    long long integer() const {return m_Integer;}
    void setInteger(long long i) {m_Flags |= Integer; m_Integer = i;}
    double real() const {return m_Real;}
    void setReal(double d) {m_Flags |= Real; m_Real = d;}
private:
    unsigned m_Flags;
    std::string m_String;
    bool m_Boolean;
    double m_Real;
    long long m_Integer;
};

static void testValue(const JsonValue& value,
                      JsonValue::Type type,
                      const ExpectedValues& expected)
{
    JT_EQUAL(value.type(), type);
    if (expected.hasValue(ExpectedValues::Real))
    {
        JT_EQUAL(value.real(), expected.real());
    }
    else
    {
        JT_THROWS(value.real(), JsonValueError);
    }
    if (expected.hasValue(ExpectedValues::Boolean))
    {
        JT_EQUAL(value.boolean(), expected.boolean());
    }
    else
    {
        JT_THROWS(value.boolean(), JsonValueError);
    }
    if (expected.hasValue(ExpectedValues::String))
    {
        JT_EQUAL(value.string(), expected.string());
    }
    else
    {
        JT_THROWS(value.string(), JsonValueError);
    }
    if (expected.hasValue(ExpectedValues::Integer))
    {
        JT_EQUAL(value.integer(), expected.integer());
    }
    else
    {
        JT_THROWS(value.integer(), JsonValueError);
    }
    JT_THROWS(value.object(), JsonValueError);
    JT_THROWS(value.array(), JsonValueError);
}

static void test_Null()
{
    JsonValue value;
    ExpectedValues expected;
    testValue(value, JsonValue::Null, expected);
}

static void test_String()
{
    JsonValue value("text");
    ExpectedValues expected;
    expected.setString("text");
    testValue(value, JsonValue::String, expected);
}

static void test_Integer()
{
    JsonValue value(100);
    ExpectedValues expected;
    expected.setInteger(100);
    expected.setReal(100);
    testValue(value, JsonValue::Integer, expected);
}

static void test_IntegralReal()
{
    JsonValue value(100.0);
    ExpectedValues expected;
    expected.setInteger(100);
    expected.setReal(100);
    testValue(value, JsonValue::Real, expected);
}

static void test_FractionalReal()
{
    JsonValue value(100.5);
    ExpectedValues expected;
    expected.setReal(100.5);
    testValue(value, JsonValue::Real, expected);
}

static void test_Boolean()
{
    JsonValue value(false);
    ExpectedValues expected;
    expected.setBoolean(false);
    testValue(value, JsonValue::Boolean, expected);
}

JT_TEST(test_Null,
        test_String,
        test_Integer,
        test_IntegralReal,
        test_FractionalReal,
        test_Boolean);
