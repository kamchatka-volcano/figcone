#pragma once
#include <gtest/gtest.h>
#include <functional>

template<typename TException>
void assert_exception(
        const std::function<void()>& throwingCode,
        std::function<void(const TException&)> exceptionContentChecker)
{
    try {
        throwingCode();
        FAIL() << "exception wasn't thrown!";
    }
    catch (const TException& e) {
        exceptionContentChecker(e);
    }
    catch (...) {
        FAIL() << "Unexpected exception was thrown";
    }
}
