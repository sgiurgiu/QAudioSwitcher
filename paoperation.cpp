#include "paoperation.h"
#include <pulse/operation.h>

auto deleter = [](pa_operation* op){
    pa_operation_unref (op);
};

PaOperation::PaOperation():op(nullptr,deleter)
{
}

PaOperation::PaOperation(pa_operation *op):op(op,deleter)
{
}

PaOperation::operator bool() const noexcept
{
    return op?true:false;
}
