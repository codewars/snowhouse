//          Copyright Joakim Karlsson & Kim Gräsman 2010-2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef SNOWHOUSE_WITHMESSAGEWRAPPER_H
#define SNOWHOUSE_WITHMESSAGEWRAPPER_H

#include "constraints/expressions/expression.h"

namespace snowhouse
{

    template<typename InnerExpression>
    struct WithMessageWrapper //: Expression<WithMessageWrapper<InnerExpression>>
    {
        WithMessageWrapper(const InnerExpression& expr, const std::string& msg)
            :m_expr{ expr }, m_msg{ msg }
        {
        }

        template<typename ActualType>
        bool operator()(const ActualType& actual) const
        {
            return m_expr(actual);
        }

        const InnerExpression m_expr;
        const std::string     m_msg;
    };

    template<typename InnerExpression>
    struct Stringizer<WithMessageWrapper<InnerExpression>>
    {
        static std::string ToString(const WithMessageWrapper<InnerExpression>& expr)
        {
            return Stringize(expr.m_expr);
        }
    };

    template<typename InnerExpression>
    WithMessageWrapper<InnerExpression>WithMessage(const InnerExpression& expr, const std::string& msg) {
        return WithMessageWrapper<InnerExpression>(expr, msg);
    }
}

#endif
