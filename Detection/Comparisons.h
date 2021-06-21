#pragma once
/* Types for detecting presence of comparison operators on another type, and testing that expected
*    truths for comparisons holds for some specified test cases. NEED -fconcepts CXX FLAG TO COMPILE
*  Example Usage:
*    std::string a = "A"; std::string b = "B"; //Declare test cases
*    Compare::Tester<std::string> tester; //Declare unitype comparison tester (for std::string)
*    //next line returns true and prints nothing for correct std::string implementation since std::string implements
*		(lexical) comparison operators, and "A" is lexically less than "B"
*    tester.Test(a, b, Compare::Assertion::Less);
*/ 
namespace Compare {
	
/* Strong assertions, which imply the result of any defined comparison. Use to assert how
* objects relate in a test. (any comparison contradicting this relation causes the test to fail)*/
enum class Assertion { Equal, Less, Greater };

//this here will test that all defined comparison operators agree with each other, 
//  and an expected (asserted) relative state (Equal, less than, or greater than)
template <typename T1, typename T2 = T1>
struct Tester {
	//Test the assertion forwards, Ex. if assert is Assertion::Less, then this is true if every defined comparison
	//	 does not contradict that obj1 is less than obj2 (Assertion::Less holds on obj1 with respect to obj2).
	bool TestForward(T1 obj1, T2 obj2, Assertion assert);
	
	//Test the reverse assertion to TestForward, Ex. given Assertion::Less, it will test that Assertion::Greater 
	//	holds on obj2 with respect to obj1.
	bool TestBackward(T1 obj1, T2 obj2, Assertion assert);
	
	//Test the assertion forwards and backwards.
	bool Test(T1 obj1, T2 obj2, Assertion assert) {
		return TestForward(obj1, obj2, assert) && TestBackward(obj1, obj2, assert);
	}
};

//order defines relative states. o1 can be less than OR equal to o2, same between o2 and o3.
template <typename T1, typename T2 = T1, typename T3 = T2>
bool TriCompare(const T1 o1, const T2 o2, const T3 o3);

/*************************************************************************************
//************************* IMPLEMENTATION DETAILS BELOW *****************************
**************************************************************************************/
#define DISPROVE_CHECK(x) if (!(x)) {\
fprintf(stderr, "\033[31mFAILED CHECK - %s\n       FILE: %s\n       LINE: %d\033[0m\n", #x, __FILE__, __LINE__);\
return false;\
}

//concepts to check that certain members are defined
template <typename T1, typename T2>
concept EqDefined = requires(const T1 & obj1, const T2 & obj2) { obj1.operator==(obj2); };

template <typename T1, typename T2>
concept NotEqDefined = requires(const T1 & obj1, const T2 & obj2) { obj1.operator!=(obj2); };

template <typename T1, typename T2>
concept LessDefined = requires(const T1 & obj1, const T2 & obj2) { obj1.operator<(obj2); };

template <typename T1, typename T2>
concept GreaterDefined = requires(const T1 & obj1, const T2 & obj2) { obj1.operator>(obj2); };

template <typename T1, typename T2>
concept LeqDefined = requires(const T1 & obj1, const T2 & obj2) { obj1.operator<=(obj2); };

template <typename T1, typename T2>
concept GeqDefined = requires(const T1 & obj1, const T2 & obj2) { obj1.operator>=(obj2); };

template <typename T1, typename T2>
bool Tester<T1, T2>::TestForward(T1 obj1, T2 obj2, Assertion assert) {
		if (assert == Assertion::Equal) {
			if constexpr (EqDefined<T1, T2>) {
				DISPROVE_CHECK(obj1 == obj2)
			}
			if constexpr (NotEqDefined<T1, T2>) {
				DISPROVE_CHECK(!(obj1 != obj2))
			}
			if constexpr (LessDefined<T1, T2>) {
				DISPROVE_CHECK(!(obj1 < obj2))
			}
			if constexpr (GreaterDefined<T1, T2>) {
				DISPROVE_CHECK(!(obj1 > obj2))
			}
			if constexpr (LeqDefined<T1, T2>) {
				DISPROVE_CHECK((obj1 <= obj2))
			}
			if constexpr (GeqDefined<T1, T2>) {
				DISPROVE_CHECK((obj1 >= obj2))
			}
		}
		if (assert == Assertion::Less) {
			if constexpr (EqDefined<T1, T2>) {
				DISPROVE_CHECK(!(obj1 == obj2))
			}
			if constexpr (NotEqDefined<T1, T2>) {
				DISPROVE_CHECK((obj1 != obj2))
			}
			if constexpr (LessDefined<T1, T2>) {
				DISPROVE_CHECK((obj1 < obj2))
			}
			if constexpr (GreaterDefined<T1, T2>) {
				DISPROVE_CHECK(!(obj1 > obj2))
			}
			if constexpr (LeqDefined<T1, T2>) {
				DISPROVE_CHECK((obj1 <= obj2))
			}
			if constexpr (GeqDefined<T1, T2>) {
				DISPROVE_CHECK(!(obj1 >= obj2))
			}
		}
		if (assert == Assertion::Greater) {
			if constexpr (EqDefined<T1, T2>) {
				DISPROVE_CHECK(!(obj1 == obj2))
			}
			if constexpr (NotEqDefined<T1, T2>) {
				DISPROVE_CHECK((obj1 != obj2))
			}
			if constexpr (LessDefined<T1, T2>) {
				DISPROVE_CHECK(!(obj1 < obj2))
			}
			if constexpr (GreaterDefined<T1, T2>) {
				DISPROVE_CHECK((obj1 > obj2))
			}
			if constexpr (LeqDefined<T1, T2>) {
				DISPROVE_CHECK(!(obj1 <= obj2))
			}
			if constexpr (GeqDefined<T1, T2>) {
				DISPROVE_CHECK((obj1 >= obj2))
			}
		}
		return true;
}
	
template <typename T1, typename T2>
bool Tester<T1, T2>::TestBackward(T1 obj1, T2 obj2, Assertion assert) {
		if (assert == Assertion::Less) {
			return TestForward(obj2, obj1, Assertion::Greater);
		}
		else if (assert == Assertion::Greater) {
			return TestForward(obj2, obj1, Assertion::Less);
		}
		else {
			return TestForward(obj2, obj1, Assertion::Equal);
		}
}
	
//Test to ensure that equality and inequality are exact opposites (nothing can be both true and false, or neither true nor false)
template <typename T1, typename T2>
bool IsLogicallyValid(const T1 o1, const T2 o2) { //!( ((T & F) | (!T & !F)) | ((Tr & Fr) | (!Tr & !Fr)) )
	return !((o1 == o2 && o1 != o2) || (!(o1 == o2) && !(o1 != o2)) || 
		(o2 == o1 && o2 != o1) || (!(o2 == o1) && !(o2 != o1)));
}

template <typename T1, typename T2, typename T3>
bool TriCompare(const T1 o1, const T2 o2, const T3 o3) {
	Tester<T1, T2> test1;
	DISPROVE_CHECK(IsLogicallyValid(o1, o2))
	else if (o1 == o2) {
		DISPROVE_CHECK(test1.Test(o1, o2, Assertion::Equal))
	} else {
		DISPROVE_CHECK(test1.Test(o1, o2, Assertion::Less))
	}
	
	Tester<T2, T3> test2;
	DISPROVE_CHECK(IsLogicallyValid(o2, o3))
	else if (o2 == o3) {
		DISPROVE_CHECK(test2.Test(o2, o3, Assertion::Equal))
	} else {
		DISPROVE_CHECK(test2.Test(o2, o3, Assertion::Less))
	}
	
	Tester<T1, T3> test3;
	DISPROVE_CHECK(IsLogicallyValid(o1, o3))
	else if (o1 == o3) {
		DISPROVE_CHECK(test3.Test(o1, o3, Assertion::Equal))
	} else {
		DISPROVE_CHECK(test3.Test(o1, o3, Assertion::Less))
	}
	return true;
}

}
