#include <stdio.h>
extern double div1(double, double);
extern double div2(double, double);

double test_fdiv(double x1, double x2)
{   puts("================ TEST ================");
    printf("            dividend :  %lf\n", x1);
    printf("             divisor :  %lf\n", x2);
    printf("Processor fdiv       :  %.16lf\n", x1 / x2);
    printf("SRT-4 fdiv           :  %.16lf\n", div1(x1, x2));
    printf("SRT-4 fdiv (with bug):  %.16lf\n", div2(x1, x2));
    puts("============== TEST END =============="); return div2(x1, x2); }

double test_fdiv_e(double x1, double x2)
{   puts("================ TEST ================");
    printf("            dividend :  %lf\n", x1);
    printf("             divisor :  %lf\n", x2);
    printf("Processor fdiv       :  %22.16le\n", x1 / x2);
    printf("SRT-4 fdiv           :  %22.16le\n", div1(x1, x2));
    printf("SRT-4 fdiv (with bug):  %22.16le\n", div2(x1, x2));
    puts("============== TEST END =============="); return div2(x1, x2); }

double test_fdiv_E(double x1, double x2)
{   puts("================ TEST ================");
    printf("            dividend :  %lf\n", x1);
    printf("             divisor :  %lf\n", x2);
    printf("Processor fdiv       :  %22.16lE\n", x1 / x2);
    printf("SRT-4 fdiv           :  %22.16lE\n", div1(x1, x2));
    printf("SRT-4 fdiv (with bug):  %22.16lE\n", div2(x1, x2));
    puts("============== TEST END =============="); return div2(x1, x2); }

double test_fdiv_es(double x1, double x2)
{   puts("================ TEST ================");
    printf("            dividend :  %22.16le\n", x1);
    printf("             divisor :  %22.16le\n", x2);
    printf("Processor fdiv       :  %22.16le\n", x1 / x2);
    printf("SRT-4 fdiv           :  %22.16le\n", div1(x1, x2));
    printf("SRT-4 fdiv (with bug):  %22.16le\n", div2(x1, x2));
    puts("============== TEST END =============="); return div2(x1, x2); }

double test_fdiv_Es(double x1, double x2)
{   puts("================ TEST ================");
    printf("            dividend :  %22.16lE\n", x1);
    printf("             divisor :  %22.16lE\n", x2);
    printf("Processor fdiv       :  %22.16lE\n", x1 / x2);
    printf("SRT-4 fdiv           :  %22.16lE\n", div1(x1, x2));
    printf("SRT-4 fdiv (with bug):  %22.16lE\n", div2(x1, x2));
    puts("============== TEST END =============="); return div2(x1, x2); }

#include <math.h>

int main(void)
{
    double x, y, z;

    //  simple fp64 division
    puts("\n> Simple FP64 Division");
    x = test_fdiv(4195835.0, 3145727.0);
    //  reciprocal
    puts("\n> Reciprocal");
    x = test_fdiv_e(1.0, 824633702441.0);
    puts("When multiplying the reciprocal,");
    printf("     got %.16lf,\n", x * 824633702441.0);
    printf("expected %.16lf.\n", (1.0 / 824633702441.0) * 824633702441.0);
    //  remainder
    puts("\n> Remainder");
    x = test_fdiv(137489121280.0, 3145727.0);
    puts("When calculating the remainder,");
    printf("     got %18lld,\n",
    137489121280 - (long long)x * 3145727);
    printf("expected %18lld.\n",
    137489121280 - (long long)(137489121280.0 / 3145727.0) * 3145727);
    //  tangent
    puts("\n> Tangent");
    x = 0.8549592142878324;
    y = sin(x), z = cos(x);
    x = test_fdiv(y, z);
    puts("When calculating the tangent value,");
    printf("     got %.16lf,\n", x);
    printf("expected %.16lf.\n", y / z);

    puts("");
    return 0;
}