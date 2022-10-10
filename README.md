# fdiv-bug
P5 Pentium FDIV bug simulation code

```
> Simple FP64 Division
================ TEST ================
            dividend :  4195835.000000
             divisor :  3145727.000000
Processor fdiv       :  1.3338204491362411
SRT-4 fdiv           :  1.3338204491362411
SRT-4 fdiv (with bug):  1.3337390689020376
============== TEST END ==============

> Reciprocal
================ TEST ================
            dividend :  1.000000
             divisor :  824633702441.000000
Processor fdiv       :  1.2126596294086669e-012
SRT-4 fdiv           :  1.2126596294086671e-012
SRT-4 fdiv (with bug):  1.2126596248911579e-012
============== TEST END ==============
When multiplying the reciprocal,
     got 0.9999999962747098,
expected 0.9999999999999999.

> Remainder
================ TEST ================
            dividend :  137489121280.000000
             divisor :  3145727.000000
Processor fdiv       :  43706.6284772963480000
SRT-4 fdiv           :  43706.6284772963480000
SRT-4 fdiv (with bug):  43703.9618097819690000
============== TEST END ==============
When calculating the remainder,
     got           11414199,
expected            1977018.

> Tangent
================ TEST ================
            dividend :  0.754544
             divisor :  0.656249
Processor fdiv       :  1.1497828165660371
SRT-4 fdiv           :  1.1497828165660371
SRT-4 fdiv (with bug):  1.1497828163886421
============== TEST END ==============
When calculating the tangent value,
     got 1.1497828163886421,
expected 1.1497828165660371.
```
