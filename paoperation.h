#ifndef PAOPERATION_H
#define PAOPERATION_H

#include <memory>
#include <functional>

struct pa_operation;
class PaOperation
{
    public:
        PaOperation();
        PaOperation(pa_operation* op);
        explicit operator bool() const noexcept;
    private:

    private:
        std::unique_ptr<pa_operation,std::function<void(pa_operation*)>> op;
};

#endif // PAOPERATION_H
