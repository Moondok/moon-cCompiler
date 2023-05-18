//error file: the arguments passed are not match the parameters defined in the function definition.
int func(int a,double b)
{
    return 1;
}

int main()
{
    int a=1,b=2;
    int c;
    c=func(a,b);
    return 0;
}