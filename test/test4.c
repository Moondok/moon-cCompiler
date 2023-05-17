int frac(int n) //test recursive function
{
    if(n==1)
        return 1;
    else   //test if -else
        return frac(n-1)*n;
}


int main()
{
    int x=5;
    while(x>0) //test while
    {
        int tmp=frac(x);
        x--;
    }
}