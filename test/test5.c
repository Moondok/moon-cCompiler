double add_float(double a1, double a2) //test double
{
    return a1+a2;
}

int main()
{
    int i=0,t=2;
    double f1=2.0;
    double f2=3.0;

    double re=add_float(f1,f2);

    do //test do-while
    {
        t+i; //reduntant temp, will be dumped in our optimization 
        /* code */
        i++; // the return value of i++ will be dumped then
    } while (i<3);
    
    return 0;
}