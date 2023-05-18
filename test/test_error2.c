//error file: use a function before it is defined
int main()
{
    int a=3;
    int b=2;
    int c=0,d=func(a,b);
}

int func(int a,int b)
{
    return a+b;
}