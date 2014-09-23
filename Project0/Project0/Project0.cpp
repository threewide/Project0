#define BOOST_TEST_MODULE Project0UnitTest
#include <boost\test\unit_test.hpp>
#include "sorted_vector.hpp"
#include <iostream>

using namespace std;

BOOST_AUTO_TEST_CASE(intro) {
	cout << "\nUnit test demo.\n";
	cout << "Last compiled: " << __TIMESTAMP__ << "\n\n";
}

// test the sorting capabilities of the vector
BOOST_AUTO_TEST_CASE(sorted_vector_test)
{
	//create a vector to use as a control test if the elements are sorted correctly
	vector<int> test_vector{ 4, 3, 7, 1, 10 , 1, 3, 8, 9, 7, 5, 4};

	//sort the vector
	sort(test_vector.begin(), test_vector.end());

	//create a sorted vector 
	sorted_vector<int> test_sorted_vector;
	test_sorted_vector.insert(4);
	test_sorted_vector.insert(3);
	test_sorted_vector.insert(7);
	test_sorted_vector.insert(1);
	test_sorted_vector.insert(10);
	test_sorted_vector.insert(1);
	test_sorted_vector.insert(3);
	test_sorted_vector.insert(8);
	test_sorted_vector.insert(9);
	test_sorted_vector.insert(7);
	test_sorted_vector.insert(5);
	test_sorted_vector.insert(4);

	//create a for loop and boost check each value to see if the sorted vector sorted correctly
	for (unsigned i = 0; i < test_vector.size(); i++)
	{
		BOOST_CHECK(test_vector[i] == test_sorted_vector[i]);
	}
}

BOOST_AUTO_TEST_CASE(sorted_vector_operator_equal)
{
	// create two sorted_vectors to test
	sorted_vector<int> test_vector_1;
	for (unsigned i = 1; i <= 10; ++i)
	{
		test_vector_1.insert(i);
	}

	sorted_vector<int> test_vector_2;
	for (unsigned i = 10; i >= 1; --i)
	{
		test_vector_2.insert(i);
	}

	//check the == operator, by checking both the size and values
	BOOST_CHECK(test_vector_1 == test_vector_2);
	BOOST_CHECK(test_vector_1.size() == test_vector_2.size());
}