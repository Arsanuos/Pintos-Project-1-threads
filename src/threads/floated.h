#define P 15

#define Q 16

#define F (1 << Q)


#define CONVERT_TO_FP(n) ((n) * (F))

#define CONVERT_TO_INT_TOWARD_ZERO(x) ((x) / (F))

#define CONVERT_TO_INT_NEAREST(x) ((x) >= 0 ? ((x) + (F) / 2) / (F) : ((x) - (F) / 2) / (F))

#define ADD(x, y) ((x) + (y))

#define SUB(x, y) ((x) - (y))

#define ADD_INT(x, n) ((x) + (n) * (F))

#define SUB_INT(x, n) ((x) - (n) * (F))

#define MUL(x, y) (((int64_t)(x)) * (y) / (F))

#define MUL_INT(x, n) ((x) * (n))

#define DIV(x, y) (((int64_t)(x)) * (F) / (y))

#define DIV_INT(x, n) ((x) / (n))
