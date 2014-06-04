#ifndef PAOPERATION_H
#define PAOPERATION_H

struct pa_operation;
class PaOperation
{
    public:
        PaOperation();
        PaOperation(pa_operation* op);
        PaOperation(const PaOperation& other) = delete;
        PaOperation(PaOperation&& other);
        ~PaOperation();
        operator bool() const;
        PaOperation& operator= (PaOperation other) = delete;
        PaOperation& operator= (PaOperation&& other);
        PaOperation& operator= (pa_operation* op);
    private:
        pa_operation* op;

};

#endif // PAOPERATION_H
