//          Copyright Joakim Karlsson & Kim Gräsman 2010-2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef SNOWHOUSE_ASSERT_H
#define SNOWHOUSE_ASSERT_H

#include "assertionexception.h"
#include "fluent/expressionbuilder.h"

#include <string>
#include <sstream>

// clang-format off
#define SNOWHOUSE_ASSERT_THAT(P1, P2, FAILURE_HANDLER) \
  ::snowhouse::ConfigurableAssert<FAILURE_HANDLER>::That((P1), (P2), __FILE__, __LINE__)

#ifndef SNOWHOUSE_NO_MACROS
# define AssertThat(P1, P2) \
  SNOWHOUSE_ASSERT_THAT((P1), (P2), ::snowhouse::DefaultFailureHandler)
#endif
// clang-format on

namespace snowhouse
{
  auto WithMessage = [](const std::string& msg) { return [&]() { return msg; }; };

  struct DefaultFailureHandler
  {
    template<typename ExpectedType, typename ActualType>
    static void Handle(const ExpectedType& expected, const ActualType& actual, const std::string& message, const char* file_name, int line_number)
    {
      std::ostringstream str;

      if (!message.empty()) {
          str << message << std::endl;
      }

      str << "Expected: " << snowhouse::Stringize(expected) << std::endl;
      str << "Actual: " << snowhouse::Stringize(actual) << std::endl;

      throw AssertionException(str.str(), file_name, line_number);
    }

    template<typename ExpectedType, typename ActualType>
    static void Handle(const ExpectedType& expected, const ActualType& actual, const char* file_name, int line_number)
    {
        Handle(expected, actual, "", file_name, line_number);
    }

    static void Handle(const std::string& message)
    {
      throw AssertionException(message);
    }
  };

  template<typename FailureHandler>
  struct ConfigurableAssert
  {

    template<typename ActualType, typename ConstraintListType, typename MessageSupplierType>
    static void That(const ActualType& actual, ExpressionBuilder<ConstraintListType> expression, const MessageSupplierType& messageSupplier, const char* file_name = "", int line_number = 0)
    {
      try
      {
        ResultStack result;
        OperatorStack operators;
        expression.Evaluate(result, operators, actual);

        while (!operators.empty())
        {
          ConstraintOperator* op = operators.top();
          op->PerformOperation(result);
          operators.pop();
        }

        if (result.empty())
        {
          throw InvalidExpressionException("The expression did not yield any result");
        }

        if (!result.top())
        {
          FailureHandler::Handle(expression, actual, messageSupplier(), file_name, line_number);
        }
      }
      catch (const InvalidExpressionException& e)
      {
        FailureHandler::Handle("Malformed expression: \"" + snowhouse::Stringize(expression) + "\"\n" + e.what());
      }
    }

    template<typename ActualType, typename ConstraintListType>
    static void That(const ActualType& actual, ExpressionBuilder<ConstraintListType> expression, const char* file_name = "", int line_number = 0)
    {
        That(actual, expression, []() { return std::string(); }, file_name, line_number);
    }

    template<typename ConstraintListType, typename MessageSupplierType>
    static void That(const char* actual, ExpressionBuilder<ConstraintListType> expression, const MessageSupplierType& messageSupplier, const char* file_name = "", int line_number = 0)
    {
        return That(std::string(actual), expression, messageSupplier, file_name, line_number);
    }

    template<typename ConstraintListType>
    static void That(const char* actual, ExpressionBuilder<ConstraintListType> expression, const char* file_name = "", int line_number = 0)
    {
      return That(actual, expression, []() { return std::string(); }, file_name, line_number);
    }

    template<typename ActualType, typename ExpressionType, typename MessageSupplierType>
    static void That(const ActualType& actual, const ExpressionType& expression, const MessageSupplierType& messageSupplier, const char* file_name = "", int line_number = 0)
    {
        if (!expression(actual))
        {
            FailureHandler::Handle(expression, actual, messageSupplier(), file_name, line_number);
        }
    }

    template<typename ActualType, typename ExpressionType>
    static void That(const ActualType& actual, const ExpressionType& expression, const char* file_name = "", int line_number = 0)
    {
        That(actual, expression, []() { return std::string(); }, file_name, line_number);
    }

    template<typename ExpressionType, typename MessageSupplierType>
    static void That(const char* actual, const ExpressionType& expression, const MessageSupplierType& messageSupplier, const char* file_name = "", int line_number = 0)
    {
        return That(std::string(actual), expression, messageSupplier, file_name, line_number);
    }

    template<typename ExpressionType>
    static void That(const char* actual, const ExpressionType& expression, const char* file_name = "", int line_number = 0)
    {
      return That(actual, expression, []() { return std::string(); },  file_name, line_number);
    }

    /*
    //Unfortunately, this overload ends up being ambigous and clashes with existing ones.
    //Can be fixed with traits or a bit of metaprogramming, but do not know how ;) Yet.
    //To assert on booleans with a custom message, one can use `Assert::That(boolvalue, IsTrue(), msgsupplier)` or
    //`Assert::That(boolvalue, Is().True(), msgsupplier)` (or respective equivalents `IsFalse()`/`Is().False()`.
    template <typename MessageSupplierType>
    static void That(bool actual, const MessageSupplierType& messageSupplier)
    {
        if (!actual)
        {
            std::string customMessage = messageSupplier();
            std::string originalMessage("Expected: true\nActual: false");

            std::string message = customMessage.length() ? (customMessage + '\n' + originalMessage) : originalMessage;
            FailureHandler::Handle(message);
        }
    }    
    */
    static void That(bool actual)
    {
        if (!actual)
        {
            FailureHandler::Handle("Expected: true\nActual: false");
        }
    }

    static void Failure(const std::string& message)
    {
      FailureHandler::Handle(message);
    }
  };

  using Assert = ConfigurableAssert<DefaultFailureHandler>;
}

#endif
