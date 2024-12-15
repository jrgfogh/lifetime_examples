#pragma warning(push)
#pragma warning(disable : 26495)
#pragma warning(disable : 26812)
#pragma warning(disable : 26439)

#include <gtest/gtest.h>

#pragma warning(pop)

#include <future>
#include <memory>

TEST(lifetime_examples_tests, NoInput) {
	auto fut = std::async([]() {
			std::cout << "Hello world!" << std::endl;
		});
	fut.get();
}

TEST(lifetime_examples_tests, ValueInput) {
	auto inputA = 25;
	auto fut = std::async([inputA](int inputB) {
			std::cout << "Input A: " << inputA << std::endl;
			std::cout << "Input B: " << inputB << std::endl;
		}, 16);
	fut.get();
}

TEST(lifetime_examples_tests, ExclusiveOwnership) {
	auto inputA = std::make_unique<int>(25);
	auto fut = std::async([inputA = std::move(inputA)](std::unique_ptr<int> inputB) {
			std::cout << "Input A: " << *inputA << std::endl;
			std::cout << "Input B: " << *inputB << std::endl;
		}, std::make_unique<int>(16));
	fut.get();
}

TEST(lifetime_examples_tests, SharedOwnership) {
	auto inputA = std::make_shared<int>(25);
	auto fut = std::async([inputA = inputA](std::shared_ptr<int> inputB) {
			std::cout << "Input A: " << *inputA << std::endl;
			std::cout << "Input B: " << *inputB << std::endl;
		}, std::make_shared<int>(16));
	std::cout << "Input A: " << *inputA << std::endl;
	fut.get();
}

TEST(lifetime_examples_tests, BorrowedData) {
	auto inputA = 25;
	auto inputB = 16;
	auto fut = std::async([&inputA](int &inputB) {
			std::cout << "Input A: " << inputA << std::endl;
			std::cout << "Input B: " << inputB << std::endl;
		}, std::ref(inputB));
	std::cout << "Input B: " << inputA << std::endl;
	fut.get();
}