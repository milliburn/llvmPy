#include <llvmPy/Instr.h>
#include <llvmPy/PyObj/PyObj.h>
#include <catch2.h>
#include <fakeit.h>
using namespace llvmPy;
using namespace fakeit;

static void
checkMethodPtr(PyStr &s, std::string name, PyObj*(*method)(PyStr **))
{
    PyFunc &f = s.py__getattr__(name)->as<PyFunc>();

    CHECK(f.getFrame() == nullptr);
    CHECK(f.getLabel() == reinterpret_cast<void *>(method));
}

TEST_CASE("type: PyStr", "[types][PyStr]") {
    PyStr s1(""), s2("a"), s3("test"), s4("4"), s5("-5"), s6("0");
    PyStr s7("True"), s8("False");

    SECTION("py__len__(): return the length of the string") {
        CHECK(s1.py__len__() == 0);
        CHECK(s2.py__len__() == 1);
        CHECK(s3.py__len__() == 4);
    }

    SECTION("py__str__(): return the string identity") {
        CHECK(s1.py__str__() == "");
        CHECK(s2.py__str__() == "a");
        CHECK(s3.py__str__() == "test");
    }

    SECTION("py__int__(): return the parsed integer if applicable") {
        CHECK(s4.py__int__() == 4);
        CHECK(s5.py__int__() == -5);
    }

    SECTION("py__bool__(): return the truthiness of the string") {
        CHECK(!s1.py__bool__());
        CHECK(s2.py__bool__());
        CHECK(s6.py__bool__());

        // Verify that the string isn't being parsed.
        CHECK(s7.py__bool__());
        CHECK(s8.py__bool__());
    }

    SECTION("py__eq__(): compare strings by value") {
        CHECK(s1.py__eq__(s1));
        CHECK(!s1.py__eq__(s2));
        CHECK(!s2.py__eq__(s1));
        CHECK(s2.py__eq__(s2));

        // Same value, different reference.
        PyStr a("a"), b("a");
        CHECK(a.py__eq__(b));
    }

    SECTION("py__ne__(): is the negation of py__eq__()") {
        Mock<PyStr> spy(s1);
        Spy(Method(spy, py__eq__));
        bool rv = spy.get().py__ne__(s1);
        Verify(Method(spy, py__eq__)).Once();
        VerifyNoOtherInvocations(spy);
        CHECK(!rv);
    }

    SECTION("py__add__(): concatenate strings") {
        CHECK(s1.py__add__(s1)->as<PyStr>().getValue() == "");
        CHECK(s1.py__add__(s2)->as<PyStr>().getValue() == "a");
        CHECK(s2.py__add__(s2)->as<PyStr>().getValue() == "aa");
        CHECK(s2.py__add__(s3)->as<PyStr>().getValue() == "atest");
    }

    SECTION("py__getattr__() returns PyFunc instances of string methods") {
        CHECK(!s1.py__getattr__("noSuchMethod"));
        checkMethodPtr(s1, "upper", PyStr::py_upper);
        checkMethodPtr(s1, "capitalize", PyStr::py_capitalize);
    }

    SECTION("py_upper(): convert string to uppercase") {
        auto *s1p = &s1, *s7p = &s7;
        auto *r1 = PyStr::py_upper(&s1p);
        CHECK(r1->as<PyStr>().getValue() == "");
        auto *r7 = PyStr::py_upper(&s7p);
        CHECK(r7->as<PyStr>().getValue() == "TRUE");

    }

    SECTION("py_capitalize(): capitalize string") {
        auto *s1p = &s1, *s3p = &s3;
        auto *r1 = PyStr::py_capitalize(&s1p);
        CHECK(r1->as<PyStr>().getValue() == "");
        auto *r3 = PyStr::py_capitalize(&s3p);
        CHECK(r3->as<PyStr>().getValue() == "Test");
    }
}
