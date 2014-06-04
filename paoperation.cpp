#include "paoperation.h"

#include <pulse/operation.h>
#include <algorithm>

PaOperation::PaOperation():op(nullptr)
{
}

PaOperation::PaOperation(pa_operation *op):op(op)
{
}

PaOperation::PaOperation(PaOperation&& other):op(other.op)
{
    other.op = nullptr;
}

PaOperation::~PaOperation()
{
    if(op)
    {
        pa_operation_unref (op);
    }
}

PaOperation::operator bool() const
{
    return op != nullptr;
}
PaOperation& PaOperation::operator= (PaOperation&& other)
{
    std::swap (op, other.op);
    return *this;
}

PaOperation& PaOperation::operator= (pa_operation* op)
{
    if(this->op != op)
    {
        if(this->op)
        {
            pa_operation_unref (this->op);
            this->op = nullptr;
        }
        this->op = op;
    }
    return *this;
}
