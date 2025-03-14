/*
 * Copyright (C) 2005 Frerich Raabe <raabe@kde.org>
 * Copyright (C) 2006, 2009 Apple Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef XPathExpressionNode_h
#define XPathExpressionNode_h

#include "core/dom/Node.h"
#include "core/xml/XPathValue.h"
#include "wtf/HashMap.h"
#include "wtf/Vector.h"
#include "wtf/text/StringHash.h"

namespace blink {

namespace XPath {

struct EvaluationContext {
    STACK_ALLOCATED();
public:
    explicit EvaluationContext(Node&);

    RefPtrWillBeMember<Node> node;
    unsigned long size;
    unsigned long position;
    HashMap<String, String> variableBindings;

    bool hadTypeConversionError;
};

class ParseNode : public NoBaseWillBeGarbageCollectedFinalized<ParseNode> {
public:
    virtual ~ParseNode() { }
    DEFINE_INLINE_VIRTUAL_TRACE() { }
};

class Expression : public ParseNode {
    WTF_MAKE_NONCOPYABLE(Expression); WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(Expression);
public:
    Expression();
    virtual ~Expression();
    DECLARE_VIRTUAL_TRACE();

    virtual Value evaluate(EvaluationContext&) const = 0;

    void addSubExpression(PassOwnPtrWillBeRawPtr<Expression> expr)
    {
        m_isContextNodeSensitive |= expr->m_isContextNodeSensitive;
        m_isContextPositionSensitive |= expr->m_isContextPositionSensitive;
        m_isContextSizeSensitive |= expr->m_isContextSizeSensitive;
        m_subExpressions.append(expr);
    }

    bool isContextNodeSensitive() const { return m_isContextNodeSensitive; }
    bool isContextPositionSensitive() const { return m_isContextPositionSensitive; }
    bool isContextSizeSensitive() const { return m_isContextSizeSensitive; }
    void setIsContextNodeSensitive(bool value) { m_isContextNodeSensitive = value; }
    void setIsContextPositionSensitive(bool value) { m_isContextPositionSensitive = value; }
    void setIsContextSizeSensitive(bool value) { m_isContextSizeSensitive = value; }

    virtual Value::Type resultType() const = 0;

protected:
    unsigned subExprCount() const { return m_subExpressions.size(); }
    Expression* subExpr(unsigned i) { return m_subExpressions[i].get(); }
    const Expression* subExpr(unsigned i) const { return m_subExpressions[i].get(); }

private:
    WillBeHeapVector<OwnPtrWillBeMember<Expression>> m_subExpressions;

    // Evaluation details that can be used for optimization.
    bool m_isContextNodeSensitive;
    bool m_isContextPositionSensitive;
    bool m_isContextSizeSensitive;
};

}

}

#endif // XPathExpressionNode_h
