//error file :multiple declaration of a function

int return_number()
{
    int a;
    return 1;
}

int return_number()
{
    return 3+5;
}
int main()
{
    int c;
    int x;
    x=return_number();
}