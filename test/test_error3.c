//error file: can not execute break in an 'if' scope
int main()
{
    int a=0,b=2;
    if(a==1)
        b=1;
    else
    {
        b=3;
        break;
    }
}